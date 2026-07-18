#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QAbstractItemView>
#include <QSqlDatabase>
#include <QSqlTableModel>
#include <QSqlError>
#include <QSqlQuery>
#include <QMessageBox>
#include <QDebug>
#include <QDate>
#include <utility>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    db = QSqlDatabase::addDatabase("QODBC");
    db.setDatabaseName(
        "DRIVER={MySQL ODBC 9.7 Unicode Driver};"
        "SERVER=localhost;"
        "PORT=3306;"
        "DATABASE=library_management;"
        "UID=root;"
        "PWD=Password;"
        );

    if (!db.open()) {
        QMessageBox::critical(this, "Database Error", db.lastError().text());
        qDebug() << db.lastError().text();
        return;
    }
    qDebug() << "Connected to MySQL successfully!";

    loadBooksTab();
    loadMembersTab();
    populateComboBoxes();
    loadBorrowTab();
    loadReportsTab();
    populateBookFormCombos();
}

MainWindow::~MainWindow()
{
    delete ui;
}

// ---------------------------------------------------------------------
// Books tab
// ---------------------------------------------------------------------
void MainWindow::loadBooksTab()
{
    booksModel = new QSqlTableModel(this, db);
    booksModel->setTable("Books");
    booksModel->select();

    // Friendly column headers (index depends on Books table column order:
    // 0=book_id, 1=title, 2=isbn, 3=author_id, 4=category_id, 5=total_copies, 6=available_copies)
    booksModel->setHeaderData(0, Qt::Horizontal, "ID");
    booksModel->setHeaderData(1, Qt::Horizontal, "Title");
    booksModel->setHeaderData(2, Qt::Horizontal, "ISBN");
    booksModel->setHeaderData(3, Qt::Horizontal, "Author ID");
    booksModel->setHeaderData(4, Qt::Horizontal, "Category ID");
    booksModel->setHeaderData(5, Qt::Horizontal, "Total Copies");
    booksModel->setHeaderData(6, Qt::Horizontal, "Available");

    ui->booksTableView->setModel(booksModel);
    ui->booksTableView->resizeColumnsToContents();
}

// ---------------------------------------------------------------------
// Add Book form (Books tab)
// ---------------------------------------------------------------------
void MainWindow::populateBookFormCombos()
{
    // Author combo: shows "name" but stores author_id as itemData
    ui->comboAuthor->clear();
    QSqlQuery authorQuery(db);
    if (authorQuery.exec("SELECT author_id, name FROM Authors ORDER BY name")) {
        while (authorQuery.next()) {
            int id = authorQuery.value(0).toInt();
            QString name = authorQuery.value(1).toString();
            ui->comboAuthor->addItem(name, id);
        }
    } else {
        qDebug() << "Failed to load authors:" << authorQuery.lastError().text();
    }

    // Category combo: shows "category_name" but stores category_id as itemData
    ui->comboCategory->clear();
    QSqlQuery categoryQuery(db);
    if (categoryQuery.exec("SELECT category_id, category_name FROM Categories ORDER BY category_name")) {
        while (categoryQuery.next()) {
            int id = categoryQuery.value(0).toInt();
            QString name = categoryQuery.value(1).toString();
            ui->comboCategory->addItem(name, id);
        }
    } else {
        qDebug() << "Failed to load categories:" << categoryQuery.lastError().text();
    }
}

void MainWindow::clearAddBookForm()
{
    ui->lineEditTitle->clear();
    ui->lineEditIsbn->clear();
    ui->lineEditCopies->clear();
    if (ui->comboAuthor->count() > 0)
        ui->comboAuthor->setCurrentIndex(0);
    if (ui->comboCategory->count() > 0)
        ui->comboCategory->setCurrentIndex(0);
}

void MainWindow::on_btnSaveBook_clicked()
{
    QString title = ui->lineEditTitle->text().trimmed();
    QString isbn  = ui->lineEditIsbn->text().trimmed();
    QString copiesText = ui->lineEditCopies->text().trimmed();

    // --- Validation ---
    if (title.isEmpty()) {
        QMessageBox::warning(this, "Missing Title", "Please enter a book title.");
        ui->lineEditTitle->setFocus();
        return;
    }
    if (isbn.isEmpty()) {
        QMessageBox::warning(this, "Missing ISBN", "Please enter an ISBN.");
        ui->lineEditIsbn->setFocus();
        return;
    }
    if (ui->comboAuthor->currentIndex() < 0 || ui->comboCategory->currentIndex() < 0) {
        QMessageBox::warning(this, "Missing Selection", "Please select both an author and a category.");
        return;
    }

    bool copiesOk = false;
    int copies = copiesText.toInt(&copiesOk);
    if (!copiesOk || copies <= 0) {
        QMessageBox::warning(this, "Invalid Copies", "Total copies must be a whole number greater than 0.");
        ui->lineEditCopies->setFocus();
        return;
    }

    int authorId   = ui->comboAuthor->currentData().toInt();
    int categoryId = ui->comboCategory->currentData().toInt();

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO Books (title, isbn, author_id, category_id, total_copies, available_copies) "
        "VALUES (:title, :isbn, :authorId, :categoryId, :copies, :copies)"
        );
    query.bindValue(":title", title);
    query.bindValue(":isbn", isbn);
    query.bindValue(":authorId", authorId);
    query.bindValue(":categoryId", categoryId);
    query.bindValue(":copies", copies);

    if (!query.exec()) {
        // Most likely failure here: duplicate ISBN (isbn is UNIQUE in the schema)
        QMessageBox::critical(this, "Save Failed", query.lastError().text());
        return;
    }

    QMessageBox::information(this, "Book Added", QString("\"%1\" was added successfully.").arg(title));

    clearAddBookForm();
    booksModel->select();
    ui->booksTableView->resizeColumnsToContents();
    populateComboBoxes(); // refresh Borrow tab's book combo too, since a new book now has stock
}

// ---------------------------------------------------------------------
// Members tab
// ---------------------------------------------------------------------
void MainWindow::loadMembersTab()
{
    membersModel = new QSqlTableModel(this, db);
    membersModel->setTable("Members");
    membersModel->select();

    // 0=member_id, 1=name, 2=email, 3=phone, 4=join_date
    membersModel->setHeaderData(0, Qt::Horizontal, "ID");
    membersModel->setHeaderData(1, Qt::Horizontal, "Name");
    membersModel->setHeaderData(2, Qt::Horizontal, "Email");
    membersModel->setHeaderData(3, Qt::Horizontal, "Phone");
    membersModel->setHeaderData(4, Qt::Horizontal, "Joined");

    ui->membersTableView->setModel(membersModel);
    ui->membersTableView->resizeColumnsToContents();
}

// ---------------------------------------------------------------------
// Borrow tab
// ---------------------------------------------------------------------
void MainWindow::populateComboBoxes()
{
    // Members combo: shows "name" but stores member_id as itemData
    ui->comboMember->clear();
    QSqlQuery memberQuery(db);
    if (memberQuery.exec("SELECT member_id, name FROM Members ORDER BY name")) {
        while (memberQuery.next()) {
            int id = memberQuery.value(0).toInt();
            QString name = memberQuery.value(1).toString();
            ui->comboMember->addItem(name, id);
        }
    } else {
        qDebug() << "Failed to load members:" << memberQuery.lastError().text();
    }

    // Books combo: shows "title" but stores book_id as itemData.
    // Only list books that currently have a copy available.
    ui->comboBook->clear();
    QSqlQuery bookQuery(db);
    if (bookQuery.exec("SELECT book_id, title FROM Books WHERE available_copies > 0 ORDER BY title")) {
        while (bookQuery.next()) {
            int id = bookQuery.value(0).toInt();
            QString title = bookQuery.value(1).toString();
            ui->comboBook->addItem(title, id);
        }
    } else {
        qDebug() << "Failed to load books:" << bookQuery.lastError().text();
    }
}

void MainWindow::loadBorrowTab()
{
    borrowModel = new QSqlTableModel(this, db);
    borrowModel->setTable("BorrowedBooks");
    borrowModel->select();

    borrowModel->setHeaderData(0, Qt::Horizontal, "Borrow ID");
    borrowModel->setHeaderData(1, Qt::Horizontal, "Book ID");
    borrowModel->setHeaderData(2, Qt::Horizontal, "Member ID");
    borrowModel->setHeaderData(3, Qt::Horizontal, "Borrowed On");
    borrowModel->setHeaderData(4, Qt::Horizontal, "Due Date");
    borrowModel->setHeaderData(5, Qt::Horizontal, "Returned On");

    ui->borrowTableView->setModel(borrowModel);

    ui->borrowTableView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->borrowTableView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->borrowTableView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    ui->borrowTableView->resizeColumnsToContents();
}
void MainWindow::on_btnBorrow_clicked()
{
    if (ui->comboMember->currentIndex() < 0 || ui->comboBook->currentIndex() < 0) {
        QMessageBox::warning(this, "Missing Selection", "Please select both a member and a book.");
        return;
    }

    int memberId = ui->comboMember->currentData().toInt();
    int bookId   = ui->comboBook->currentData().toInt();
    QDate borrowDate = ui->dateBorrow->date();
    QDate dueDate     = ui->dateDue->date();

    if (dueDate <= borrowDate) {
        QMessageBox::warning(this, "Invalid Dates", "Due date must be after the borrow date.");
        return;
    }

    // --- Hard-stop stock check: re-query the DB directly instead of trusting
    // the combo box, in case stock changed since the combo was last populated. ---
    QSqlQuery stockCheck(db);
    stockCheck.prepare("SELECT available_copies FROM Books WHERE book_id = :bookId");
    stockCheck.bindValue(":bookId", bookId);
    if (!stockCheck.exec() || !stockCheck.next()) {
        QMessageBox::critical(this, "Borrow Failed", "Could not verify book stock. Please try again.");
        return;
    }
    int availableCopies = stockCheck.value(0).toInt();
    if (availableCopies <= 0) {
        QMessageBox::warning(this, "No Copies Available",
                             "This book has no copies left to borrow. Please choose another title.");
        populateComboBoxes(); // refresh combo so the out-of-stock book drops off the list
        return;
    }

    // --- Insert + decrement together as one transaction ---
    if (!db.transaction()) {
        QMessageBox::critical(this, "Borrow Failed", "Could not start a database transaction.");
        return;
    }

    QSqlQuery query(db);
    query.prepare(
        "INSERT INTO BorrowedBooks (book_id, member_id, borrow_date, due_date) "
        "VALUES (:bookId, :memberId, :borrowDate, :dueDate)"
        );
    query.bindValue(":bookId", bookId);
    query.bindValue(":memberId", memberId);
    query.bindValue(":borrowDate", borrowDate);
    query.bindValue(":dueDate", dueDate);

    if (!query.exec()) {
        db.rollback();
        QMessageBox::critical(this, "Borrow Failed", query.lastError().text());
        return;
    }

    // Decrement available copies for the borrowed book.
    // The "AND available_copies > 0" guard means this UPDATE affects 0 rows
    // if stock hit zero between our check above and now — we verify that below.
    QSqlQuery updateQuery(db);
    updateQuery.prepare(
        "UPDATE Books SET available_copies = available_copies - 1 "
        "WHERE book_id = :bookId AND available_copies > 0"
        );
    updateQuery.bindValue(":bookId", bookId);
    if (!updateQuery.exec() || updateQuery.numRowsAffected() == 0) {
        db.rollback();
        QMessageBox::warning(this, "No Copies Available",
                             "This book just ran out of copies. Please choose another title.");
        populateComboBoxes();
        return;
    }

    if (!db.commit()) {
        db.rollback();
        QMessageBox::critical(this, "Borrow Failed", "Could not save the transaction.");
        return;
    }

    borrowModel->select();
    booksModel->select();
    populateComboBoxes();
    ui->borrowTableView->resizeColumnsToContents();
}

void MainWindow::on_btnReturn_clicked()
{
    const QModelIndexList selected = ui->borrowTableView->selectionModel()->selectedRows();
    if (selected.isEmpty()) {
        QMessageBox::warning(this, "No Selection", "Please select a borrow record to return.");
        return;
    }

    int borrowIdColumn = borrowModel->fieldIndex("borrow_id");
    int bookIdColumn    = borrowModel->fieldIndex("book_id");
    int returnDateColumn = borrowModel->fieldIndex("return_date");

    for (const QModelIndex &index : std::as_const(selected)) {
        int borrowId = borrowModel->data(borrowModel->index(index.row(), borrowIdColumn)).toInt();
        int bookId    = borrowModel->data(borrowModel->index(index.row(), bookIdColumn)).toInt();
        bool alreadyReturned = !borrowModel->data(borrowModel->index(index.row(), returnDateColumn)).isNull();

        if (alreadyReturned) {
            continue; // skip records that already have a return date
        }

        QSqlQuery query(db);
        query.prepare("UPDATE BorrowedBooks SET return_date = :today WHERE borrow_id = :id");
        query.bindValue(":today", QDate::currentDate());
        query.bindValue(":id", borrowId);

        if (!query.exec()) {
            QMessageBox::critical(this, "Return Failed", query.lastError().text());
            return;
        }

        // Increment available copies for the returned book
        QSqlQuery updateQuery(db);
        updateQuery.prepare(
            "UPDATE Books SET available_copies = available_copies + 1 "
            "WHERE book_id = :bookId AND available_copies < total_copies"
            );
        updateQuery.bindValue(":bookId", bookId);
        if (!updateQuery.exec()) {
            qDebug() << "Failed to update available_copies:" << updateQuery.lastError().text();
        }
    }

    borrowModel->select();
    booksModel->select();
    populateComboBoxes();
    ui->borrowTableView->resizeColumnsToContents();
}

// ---------------------------------------------------------------------
// Reports tab
// ---------------------------------------------------------------------
void MainWindow::loadReportsTab()
{
    ui->comboReportType->clear();
    ui->comboReportType->addItem("All Books", "books");
    ui->comboReportType->addItem("All Members", "members");
    ui->comboReportType->addItem("Overdue Borrows", "overdue");
    ui->comboReportType->addItem("Currently Borrowed", "borrowed");
    ui->comboReportType->addItem("Most Borrowed Books", "most_borrowed");
    ui->comboReportType->addItem("Monthly Borrow Stats", "monthly_stats");
    ui->comboReportType->addItem("Books by Category", "by_category");
}

void MainWindow::on_btnRunReport_clicked()
{
    currentReportType = ui->comboReportType->currentData().toString();
    runReport(currentReportType);
}

void MainWindow::on_btnRefreshReport_clicked()
{
    if (currentReportType.isEmpty()) {
        QMessageBox::information(this, "No Report Selected",
                                 "Run a report first, then use Refresh to reload it.");
        return;
    }
    runReport(currentReportType);
}

void MainWindow::runReport(const QString &reportType)
{
    // Hang on to whatever model is currently attached so we can free it
    // once the new one is safely in place. Every report model here is a
    // fresh heap allocation parented to `this`, so without this cleanup
    // each Run/Refresh click would leak the previous report's model.
    QAbstractItemModel *oldModel = ui->reportsTableView->model();

    QSqlTableModel *reportModel = new QSqlTableModel(this, db);

    if (reportType == "books") {
        reportModel->setTable("Books");
        reportModel->select();
    } else if (reportType == "members") {
        reportModel->setTable("Members");
        reportModel->select();
    } else if (reportType == "overdue") {
        reportModel->setQuery(
            "SELECT bb.borrow_id, m.name, b.title, bb.borrow_date, bb.due_date, "
            "DATEDIFF(CURDATE(), bb.due_date) AS days_overdue "
            "FROM BorrowedBooks bb "
            "JOIN Members m ON m.member_id = bb.member_id "
            "JOIN Books b ON b.book_id = bb.book_id "
            "WHERE bb.return_date IS NULL AND bb.due_date < CURDATE() "
            "ORDER BY days_overdue DESC",
            db
            );
        reportModel->setHeaderData(0, Qt::Horizontal, "Borrow ID");
        reportModel->setHeaderData(1, Qt::Horizontal, "Member");
        reportModel->setHeaderData(2, Qt::Horizontal, "Book");
        reportModel->setHeaderData(3, Qt::Horizontal, "Borrowed On");
        reportModel->setHeaderData(4, Qt::Horizontal, "Due Date");
        reportModel->setHeaderData(5, Qt::Horizontal, "Days Overdue");
    } else if (reportType == "borrowed") {
        reportModel->setQuery(
            "SELECT bb.borrow_id, m.name, b.title, bb.borrow_date, bb.due_date "
            "FROM BorrowedBooks bb "
            "JOIN Members m ON m.member_id = bb.member_id "
            "JOIN Books b ON b.book_id = bb.book_id "
            "WHERE bb.return_date IS NULL",
            db
            );
        reportModel->setHeaderData(0, Qt::Horizontal, "Borrow ID");
        reportModel->setHeaderData(1, Qt::Horizontal, "Member");
        reportModel->setHeaderData(2, Qt::Horizontal, "Book");
        reportModel->setHeaderData(3, Qt::Horizontal, "Borrowed On");
        reportModel->setHeaderData(4, Qt::Horizontal, "Due Date");
    } else if (reportType == "most_borrowed") {
        reportModel->setQuery(
            "SELECT b.title, COUNT(bb.borrow_id) AS times_borrowed "
            "FROM BorrowedBooks bb "
            "JOIN Books b ON bb.book_id = b.book_id "
            "GROUP BY b.book_id, b.title "
            "ORDER BY times_borrowed DESC "
            "LIMIT 10",
            db
            );
        reportModel->setHeaderData(0, Qt::Horizontal, "Title");
        reportModel->setHeaderData(1, Qt::Horizontal, "Times Borrowed");
    } else if (reportType == "monthly_stats") {
        reportModel->setQuery(
            "SELECT DATE_FORMAT(borrow_date, '%Y-%m') AS month, COUNT(*) AS total_borrowed "
            "FROM BorrowedBooks "
            "GROUP BY DATE_FORMAT(borrow_date, '%Y-%m') "
            "ORDER BY month",
            db
            );
        reportModel->setHeaderData(0, Qt::Horizontal, "Month");
        reportModel->setHeaderData(1, Qt::Horizontal, "Total Borrowed");
    } else if (reportType == "by_category") {
        reportModel->setQuery(
            "SELECT c.category_name, COUNT(b.book_id) AS total_books "
            "FROM Categories c "
            "LEFT JOIN Books b ON c.category_id = b.category_id "
            "GROUP BY c.category_id, c.category_name "
            "HAVING COUNT(b.book_id) > 0 "
            "ORDER BY total_books DESC",
            db
            );
        reportModel->setHeaderData(0, Qt::Horizontal, "Category");
        reportModel->setHeaderData(1, Qt::Horizontal, "Total Books");
    }

    if (reportModel->lastError().isValid()) {
        QMessageBox::critical(this, "Report Failed", reportModel->lastError().text());
        delete reportModel;
        return;
    }

    ui->reportsTableView->setModel(reportModel);
    ui->reportsTableView->resizeColumnsToContents();

    // Safe to delete unconditionally: reportsTableView only ever holds
    // report-specific models created in this function, never the
    // persistent booksModel/membersModel/borrowModel members.
    if (oldModel) {
        oldModel->deleteLater();
    }
}

// ---------------------------------------------------------------------
// Tab switching
// ---------------------------------------------------------------------
void MainWindow::on_tabWidget_currentChanged(int index)
{
    // Optional: refresh data whenever the user switches tabs
    switch (index) {
    case 0: booksModel->select(); break;
    case 1: membersModel->select(); break;
    case 2: populateComboBoxes(); borrowModel->select(); break;
    default: break;
    }
}
