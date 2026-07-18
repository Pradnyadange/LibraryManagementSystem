# Library Management System

A desktop application built with **Qt 6 (C++/Widgets)** and **MySQL**, allowing library staff to manage books, members, and borrow/return transactions, with built-in reporting.

## Features

- **Books** — view all books with author and category info; add new books via form
- **Members** — view all registered library members
- **Borrow** — borrow a book for a member with due-date tracking, and mark books as returned
- **Reports** — 7 built-in reports: All Books, All Members, Overdue Borrows, Currently Borrowed, Most Borrowed Books, Monthly Borrow Stats, Books by Category

## Tech Stack

| Layer | Technology |
|---|---|
| UI / Application | Qt 6.11 (Widgets, Designer) |
| Language | C++17 |
| Database | MySQL 8.0 |
| DB Connectivity | Qt SQL module via ODBC (`QODBC` driver), using MySQL Connector/ODBC |
| Build System | CMake |

## Database Schema

5 tables: `Authors`, `Categories`, `Books`, `Members`, `BorrowedBooks`, with foreign keys linking Books → Authors/Categories, and BorrowedBooks → Books/Members. Full schema with constraints (`CHECK`, `UNIQUE`, `NOT NULL`) is in `schema/full_setup.sql`.

**Key business rule enforced at the DB level:** `available_copies <= total_copies` — a book can never show more copies available than it owns.

## Setup Instructions

### 1. Database
1. Install MySQL Server (8.0+) and MySQL Workbench
2. Run `schema/full_setup.sql` in Workbench — this creates the database, all tables, indexes, and seed data (20 authors, 8 categories, 27 books, 10 members, 36 borrow records spanning 3 months)

### 2. ODBC Driver
1. Install **MySQL Connector/ODBC** (64-bit) — download from `dev.mysql.com/downloads/connector/odbc/`, or via MySQL Installer's "Add" feature
2. Open **ODBC Data Sources (64-bit)** (Windows search)
3. Under **System DSN**, add a new entry using the MySQL ODBC driver
4. Configure: Server `localhost`, Port `3306`, Database `library_management`, your MySQL user/password
5. Test the connection — should say "Connection successful"

### 3. Qt Environment
1. Install **Qt 6.11+** with the **Desktop (MinGW or MSVC)** kit via the Qt Online Installer
2. Install **Qt Creator** (bundled with the above)
3. Ensure the project's `CMakeLists.txt` links the `Sql` component (`find_package(Qt6 REQUIRED COMPONENTS Core Widgets Sql)`)

### 4. Run
1. Open the project folder in Qt Creator (open `CMakeLists.txt`)
2. Update the connection string in `mainwindow.cpp` if your MySQL username/password differ from the defaults
3. Build (Ctrl+B) and Run (Ctrl+R)

## Project Structure

```
LibraryManagementSystem/
├── CMakeLists.txt
├── main.cpp
├── mainwindow.h
├── mainwindow.cpp
├── mainwindow.ui
└── schema/
    └── full_setup.sql
```

## Known Limitations

- Connection string currently hardcodes DB credentials in `mainwindow.cpp` — for production use, move to a config file or environment variables
- No user authentication / login screen — assumes single trusted staff user
- No pagination on tables — fine at current data scale (dozens of books/members), would need it at larger scale

## Possible Future Enhancements

- Search/filter bar on the Books tab
- Export reports to CSV/PDF
- Fine calculation for overdue books
- Login system with role-based access (admin vs. staff)
