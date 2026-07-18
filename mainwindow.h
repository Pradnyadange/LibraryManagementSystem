#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlTableModel>
QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE
class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void on_btnBorrow_clicked();
    void on_btnReturn_clicked();
    void on_btnRunReport_clicked();
    void on_btnRefreshReport_clicked();   // NEW — refreshes the currently-shown report
    void on_tabWidget_currentChanged(int index);
    void on_btnSaveBook_clicked();
private:
    Ui::MainWindow *ui;
    QSqlDatabase db;
    // Models kept as members so we can refresh/select on them later
    QSqlTableModel *booksModel   = nullptr;
    QSqlTableModel *membersModel = nullptr;
    QSqlTableModel *borrowModel  = nullptr;
    void loadBooksTab();
    void loadMembersTab();
    void loadBorrowTab();      // refreshes combo boxes + borrow table
    void populateComboBoxes(); // fills comboMember and comboBook
    void loadReportsTab();     // sets up the report-type combo box
    void populateBookFormCombos(); // fills comboAuthor and comboCategory
    void clearAddBookForm();       // resets the Add Book form fields
    void runReport(const QString &reportType); // NEW — shared logic for run/refresh
    QString currentReportType;                 // NEW — remembers last-run report type
};
#endif // MAINWINDOW_H