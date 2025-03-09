LIBRARY MANAGEMENT SYSTEM
==========================

OVERVIEW
--------
This Library Management System is a comprehensive C++ application designed to manage library resources, user accounts, and book transactions. It supports three types of users:
- Students
- Faculty members
- Librarians

Each user type has different permissions and capabilities within the system. The application features persistent data storage, allowing user and book information to be saved between sessions.

GETTING STARTED
--------------

Installation
-----------
1. Compile the source code using a C++ compiler:
   g++ -std=c++11 -o assign1 assign1.cpp

2. Run the executable:
   ./assign1

First-time Setup
---------------

According to the problem statement, I have already created a databse for books and the user in books.txt and accounts.txt respectively.
The librarian name is Admin and its ID is 1.
I have also added some students and faculty with their names,id and status in accounts.txt file.
While adding/removing users make sures you use the right name and id. While creating new id, see from the accounts.txt file which user id is already used.
For using the predefined accounts.txt, you can see the names of the students, faculty and the librarian along with their id. For eg, Majnu is a student with id 2, Dr. Shubham is a faculty with id 7 and Admin is the librarian with id 1.
But for generality purposes, I have implemented the below mentioned features
- On first run, the system will create a default librarian account:
  - Username: Admin
  - ID: 1
- Use this account to set up other user accounts and add books to the library.

USER ROLES AND FEATURES
----------------------

1. STUDENT
----------

Students can:
- Borrow up to 3 books at a time
- Return borrowed books
- Pay fines for overdue books
- View their account details
- Browse available books

Borrowing Rules for Students:
- Maximum 3 books at a time
- 15-day borrowing period
- Fine of 10 rupees per day for each overdue book
- Cannot borrow new books if there is an outstanding fine

Student Menu Options:
1. Borrow Book: Enter the title of the book you wish to borrow
2. Return Book: Enter the title of the book you wish to return
3. Pay Fine: Clear any outstanding fines on your account
4. Display Account: View your current account status, borrowed books, and fines
5. Display Available Books: Browse all books in the library collection
6. Exit: Return to the main menu

2. FACULTY
----------

Faculty members can:
- Borrow up to 5 books at a time
- Return borrowed books
- View their account details
- Browse available books

Borrowing Rules for Faculty:
- Maximum 5 books at a time
- 30-day borrowing period
- No fines, but cannot borrow additional books if any book is overdue by more than 60 days

Faculty Menu Options:
1. Borrow Book: Enter the title of the book you wish to borrow
2. Return Book: Enter the title of the book you wish to return
3. Display Account: View your current account status and borrowed books
4. Display Available Books: Browse all books in the library collection
5. Exit: Return to the main menu

3. LIBRARIAN
------------

Librarians have administrative control over the system and can:
- Add new books to the library
- Remove books from the library
- Create user accounts (for students, faculty, or other librarians)
- Remove user accounts
- View all books in the library
- View all user accounts

Librarian Menu Options:
1. Add Book: Add a new book to the library (requires title, author, publisher, year, and ISBN)
2. Remove Book: Remove a book from the library by title
3. Add Account: Create a new user account (select student, faculty, or librarian)
4. Remove Account: Remove a user account by name
5. Display Books: View all books in the library collection
6. Display Accounts: View all user accounts in the system
7. Exit: Return to the main menu

USING THE SYSTEM
---------------

Login
-----
1. Select your role (Student, Faculty, or Librarian)
2. Enter your name and ID
3. If your account exists, you'll be directed to your role-specific menu

For First-time Users
-------------------
If you don't have an account:
1. Ask a librarian to create an account for you
2. If no librarian account exists, use the default Admin account (Name: Librarian, ID:9)

Data Persistence
---------------
The system saves all changes to files:
- books.txt: Contains all book information
- accounts.txt: Contains all user account information

These files are automatically updated when:
- Books are added or removed
- User accounts are created or deleted
- Books are borrowed or returned
- Fines are paid

BOOK MANAGEMENT
--------------

Book Information
---------------
Each book in the system includes:
- Title
- Author
- Publisher
- Publication year
- ISBN
- Status (Available, Borrowed, Reserved)
- Reserved by (name of user who has borrowed/reserved the book)

Book Status
----------
- Available: Book can be borrowed
- Borrowed: Book is currently checked out to a user
- Reserved: Book is reserved for a specific user

ACCOUNT MANAGEMENT
-----------------

Student Accounts
---------------
- Can have up to 3 books at once
- Incur fines for overdue books (10 rupees per day past the 15-day limit)
- Must pay fines before borrowing more books

Faculty Accounts
---------------
- Can have up to 5 books at once
- Have a 30-day borrowing period
- Don't incur fines but are restricted from borrowing if a book is more than 60 days overdue

Librarian Accounts
-----------------
- Have administrative privileges
- Cannot borrow books
- Can manage the entire library system

TIPS FOR USING THE SYSTEM
------------------------

1. For Students: Remember to return books on time to avoid fines. Check your account regularly to stay updated on due dates.

2. For Faculty: While you don't have fines, having books overdue by more than 60 days will prevent you from borrowing additional materials.

3. For Librarians: 
   - Regularly back up the books.txt and accounts.txt files
   - When creating new accounts, ensure each user has a unique ID
   - Be careful when removing books that are currently borrowed

4. For All Users: The system automatically saves after every transaction, so you can safely exit at any time without losing data.

TROUBLESHOOTING
--------------

- Account not found: Contact a librarian to create an account
- Cannot borrow books: Check for fines (students) or overdue books (faculty)
- Book not found: Check that you're entering the exact title as it appears in the library

SYSTEM REQUIREMENTS
------------------

- C++ compiler (C++11 or higher)
- Standard libraries: iostream, fstream, vector, string, ctime, algorithm, sstream, memory
