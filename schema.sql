DROP DATABASE IF EXISTS library_management;
CREATE DATABASE library_management;
USE library_management;
CREATE TABLE Authors (
    author_id     INT PRIMARY KEY AUTO_INCREMENT,
    name          VARCHAR(100) NOT NULL,
    nationality   VARCHAR(50)
);

CREATE TABLE Categories (
    category_id   INT PRIMARY KEY AUTO_INCREMENT,
    category_name VARCHAR(50) NOT NULL UNIQUE
);

CREATE TABLE Books (
    book_id           INT PRIMARY KEY AUTO_INCREMENT,
    title             VARCHAR(200) NOT NULL,
    isbn              VARCHAR(20) NOT NULL UNIQUE,
    author_id         INT NOT NULL,
    category_id       INT NOT NULL,
    total_copies      INT NOT NULL DEFAULT 1,
    available_copies  INT NOT NULL DEFAULT 1,
    FOREIGN KEY (author_id)   REFERENCES Authors(author_id),
    FOREIGN KEY (category_id) REFERENCES Categories(category_id),
    CHECK (total_copies >= 0),
    CHECK (available_copies >= 0),
    CHECK (available_copies <= total_copies)
);

CREATE TABLE Members (
    member_id     INT PRIMARY KEY AUTO_INCREMENT,
    name          VARCHAR(100) NOT NULL,
    email         VARCHAR(100) NOT NULL UNIQUE,
    phone         VARCHAR(20),
    join_date     DATE NOT NULL DEFAULT (CURRENT_DATE)
);

CREATE TABLE BorrowedBooks (
    borrow_id     INT PRIMARY KEY AUTO_INCREMENT,
    book_id       INT NOT NULL,
    member_id     INT NOT NULL,
    borrow_date   DATE NOT NULL DEFAULT (CURRENT_DATE),
    due_date      DATE NOT NULL,
    return_date   DATE,
    FOREIGN KEY (book_id)   REFERENCES Books(book_id),
    FOREIGN KEY (member_id) REFERENCES Members(member_id),
    CHECK (due_date > borrow_date)
);

CREATE INDEX idx_books_author    ON Books(author_id);
CREATE INDEX idx_books_category  ON Books(category_id);
CREATE INDEX idx_borrow_book     ON BorrowedBooks(book_id);
CREATE INDEX idx_borrow_member   ON BorrowedBooks(member_id);
CREATE INDEX idx_borrow_due      ON BorrowedBooks(due_date);

INSERT INTO Authors (name, nationality) VALUES
('J. K. Rowling', 'British'),
('William Shakespeare', 'English'),
('George Orwell', 'British'),
('Jane Austen', 'English'),
('Charles Dickens', 'English'),
('Leo Tolstoy', 'Russian'),
('Fyodor Dostoevsky', 'Russian'),
('Anton Chekhov', 'Russian'),
('Mark Twain', 'American'),
('Ernest Hemingway', 'American'),
('F. Scott Fitzgerald', 'American'),
('Harper Lee', 'American'),
('Stephen King', 'American'),
('Agatha Christie', 'British'),
('Arthur Conan Doyle', 'British'),
('Paulo Coelho', 'Brazilian'),
('Gabriel García Márquez', 'Colombian'),
('Haruki Murakami', 'Japanese'),
('Rabindranath Tagore', 'Indian'),
('R. K. Narayan', 'Indian');

INSERT INTO Categories (category_name) VALUES
('Fantasy'),
('Drama'),
('Dystopian Fiction'),
('Classic Literature'),
('Mystery'),
('Adventure'),
('Romance'),
('Philosophy');

INSERT INTO Books (title, isbn, author_id, category_id, total_copies, available_copies) VALUES
('Harry Potter and the Philosopher''s Stone', '9780747532699', 1, 1, 10, 10),
('Harry Potter and the Chamber of Secrets', '9780747538493', 1, 1, 8, 8),
('Harry Potter and the Prisoner of Azkaban', '9780747542155', 1, 1, 7, 7),
('Hamlet', '9780743477123', 2, 2, 6, 6),
('Macbeth', '9780743477109', 2, 2, 5, 5),
('Romeo and Juliet', '9780743477116', 2, 2, 8, 8),
('Othello', '9780743477550', 2, 2, 4, 4),
('1984', '9780451524935', 3, 3, 12, 12),
('Animal Farm', '9780451526342', 3, 3, 10, 10),
('Homage to Catalonia', '9780156421171', 3, 3, 3, 3),
('Down and Out in Paris and London', '9780156262248', 3, 3, 4, 4);

INSERT INTO Members (name, email, phone) VALUES
('Aarav Sharma', 'aarav.sharma@example.com', '9876543210'),
('Priya Patel', 'priya.patel@example.com', '9123456780'),
('Rahul Verma', 'rahul.verma@example.com', '9988776655'),
('Sneha Iyer', 'sneha.iyer@example.com', '9012345678'),
('Vikram Singh', 'vikram.singh@example.com', '9090909090'),
('Ananya Gupta', 'ananya.gupta@example.com', '9876501234'),
('Rohan Mehta', 'rohan.mehta@example.com', '9765432109'),
('Neha Joshi', 'neha.joshi@example.com', '9345678901'),
('Karan Malhotra', 'karan.malhotra@example.com', '9456123789'),
('Ishita Roy', 'ishita.roy@example.com', '9898989898');

INSERT INTO Books (title, isbn, author_id, category_id, total_copies, available_copies) VALUES
('Pride and Prejudice', '9780141439518', 4, 7, 6, 6),
('Great Expectations', '9780141439563', 5, 4, 5, 5),
('War and Peace', '9780199232765', 6, 4, 4, 4),
('Crime and Punishment', '9780486415871', 7, 8, 5, 5),
('The Adventures of Tom Sawyer', '9780486400778', 9, 6, 7, 7),
('The Old Man and the Sea', '9780684801223', 10, 4, 6, 6),
('The Great Gatsby', '9780743273565', 11, 4, 8, 8),
('To Kill a Mockingbird', '9780061120084', 12, 4, 9, 9),
('The Shining', '9780307743657', 13, 5, 5, 5),
('Murder on the Orient Express', '9780062693662', 14, 5, 6, 6),
('The Adventures of Sherlock Holmes', '9781420951264', 15, 5, 7, 7),
('The Alchemist', '9780062315007', 16, 6, 10, 10),
('One Hundred Years of Solitude', '9780060883287', 17, 4, 5, 5),
('Norwegian Wood', '9780375704024', 18, 7, 4, 4),
('Gitanjali', '9781420937751', 19, 8, 3, 3),
('Malgudi Days', '9780143039646', 20, 6, 4, 4);

INSERT INTO BorrowedBooks (book_id, member_id, borrow_date, due_date, return_date) VALUES
(1, 1, '2026-07-01', '2026-07-15', '2026-07-12'),
(2, 2, '2026-07-02', '2026-07-16', NULL),
(3, 3, '2026-07-03', '2026-07-17', '2026-07-15'),
(4, 4, '2026-07-04', '2026-07-18', NULL),
(5, 5, '2026-07-05', '2026-07-19', '2026-07-18'),
(6, 6, '2026-07-06', '2026-07-20', NULL),
(7, 7, '2026-07-07', '2026-07-21', '2026-07-20'),
(8, 8, '2026-07-08', '2026-07-22', NULL),
(9, 9, '2026-07-09', '2026-07-23', '2026-07-22'),
(10, 10, '2026-07-10', '2026-07-24', NULL);

INSERT INTO BorrowedBooks (book_id, member_id, borrow_date, due_date, return_date) VALUES

(12, 1, '2026-05-02', '2026-05-16', '2026-05-14'),
(13, 2, '2026-05-04', '2026-05-18', '2026-05-20'),
(14, 3, '2026-05-06', '2026-05-20', NULL),              
(15, 4, '2026-05-08', '2026-05-22', '2026-05-19'),
(16, 5, '2026-05-10', '2026-05-24', NULL),              
(17, 6, '2026-05-12', '2026-05-26', '2026-05-25'),

(18, 7, '2026-06-01', '2026-06-15', '2026-06-14'),
(19, 8, '2026-06-03', '2026-06-17', NULL),
(20, 9, '2026-06-05', '2026-06-19', '2026-06-18'),
(21, 10, '2026-06-07', '2026-06-21', NULL),
(22, 1, '2026-06-09', '2026-06-23', '2026-06-20'),
(23, 2, '2026-06-11', '2026-06-25', '2026-06-24'),
(24, 3, '2026-06-13', '2026-06-27', NULL),
(1, 4, '2026-06-15', '2026-06-29', '2026-06-28'),
(2, 5, '2026-06-18', '2026-07-02', '2026-07-01'),
(3, 6, '2026-06-20', '2026-07-04', NULL),

(25, 7, '2026-07-01', '2026-07-15', '2026-07-12'),
(26, 8, '2026-07-02', '2026-07-16', NULL),
(27, 9, '2026-07-03', '2026-07-17', '2026-07-15'),
(4, 10, '2026-07-04', '2026-07-18', NULL),
(5, 1, '2026-07-05', '2026-07-19', '2026-07-18'),
(6, 2, '2026-07-06', '2026-07-20', NULL),
(7, 3, '2026-07-07', '2026-07-21', '2026-07-20'),
(8, 4, '2026-07-08', '2026-07-22', NULL),
(9, 5, '2026-07-09', '2026-07-23', '2026-07-22'),
(10, 6, '2026-07-10', '2026-07-24', NULL);
SELECT 
    b.book_id,
    b.title,
    a.name AS author,
    c.category_name AS category,
    b.total_copies,
    b.available_copies
FROM Books b
JOIN Authors a ON b.author_id = a.author_id
JOIN Categories c ON b.category_id = c.category_id
ORDER BY b.title;

SELECT * FROM Authors;
SELECT * FROM Categories;
SELECT * FROM Books;
SELECT * FROM Members;
SELECT * FROM BorrowedBooks;
SELECT 
    b.title,
    COUNT(bb.borrow_id) AS times_borrowed
FROM BorrowedBooks bb
JOIN Books b ON bb.book_id = b.book_id
GROUP BY b.book_id, b.title
ORDER BY times_borrowed DESC
LIMIT 10;
SELECT 
    m.name,
    b.title,
    bb.due_date,
    DATEDIFF(CURDATE(), bb.due_date) AS days_overdue
FROM BorrowedBooks bb
JOIN Members m ON bb.member_id = m.member_id
JOIN Books b ON bb.book_id = b.book_id
WHERE bb.return_date IS NULL 
  AND bb.due_date < CURDATE()
ORDER BY days_overdue DESC;
SELECT 
    DATE_FORMAT(borrow_date, '%Y-%m') AS month,
    COUNT(*) AS total_borrowed
FROM BorrowedBooks
GROUP BY DATE_FORMAT(borrow_date, '%Y-%m')
ORDER BY month;
SELECT 
    c.category_name,
    COUNT(b.book_id) AS total_books
FROM Categories c
LEFT JOIN Books b ON c.category_id = b.category_id
GROUP BY c.category_id, c.category_name
HAVING COUNT(b.book_id) > 0
ORDER BY total_books DESC;
