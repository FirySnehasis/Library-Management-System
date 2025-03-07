#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <ctime>
#include <algorithm>
#include <sstream>
#include <memory>

using namespace std;

time_t getCurrentDate() 
{
    return time(nullptr) / (60 * 60 * 24); // Convert seconds to days
}

// Base User class
class User {
protected:
    string name;
    int id;
    string role;

public:
    User(string name, int id, string role) : name(name), id(id), role(role) {}

    string getName() const { return name; }
    int getId() const { return id; }
    string getRole() const { return role; }

    virtual void display() const {
        cout << "Name: " << name << ", ID: " << id << ", Role: " << role << endl;
    }

    virtual ~User() {}
};

// Derived Student class
class Student : public User 
{
    private:
        int borrowedBooks;
        vector<pair<string, time_t>> currentBooks; 
        double fine;
    
    public:
        Student(string name, int id) : User(name, id, "Student"), borrowedBooks(0), fine(0) {}
    
        const vector<pair<string, time_t>>& getCurrentBooks() const { return currentBooks; }
    
        void updateFines(time_t currentDate) {
            double currentFine = 0;
            for (const auto& book : currentBooks) {
                time_t borrowDate = book.second;
                int daysOverdue = currentDate - borrowDate - 15; // 15 days borrowing period
                if (daysOverdue > 0) {
                    currentFine += daysOverdue * 10;
                }
            }
            fine = currentFine;
        }
    
        bool borrowBook(const string& bookTitle, time_t borrowDate) {
            // First updating the fines to check for overdue books
            updateFines(borrowDate);
            
            if (borrowedBooks >= 3) {
                cout << "Borrowing limit reached. Cannot borrow more books." << endl;
                return false;
            }
            if (fine > 0) {
                cout << "Outstanding fine of " << fine << " rupees. Please clear the fine to borrow books." << endl;
                return false;
            }
            currentBooks.push_back({bookTitle, borrowDate});
            borrowedBooks++;
            return true;
        }
    
        bool returnBook(const string& bookTitle, time_t returnDate) {
            // First updating  all the fines
            updateFines(returnDate);
            
            auto it = find_if(currentBooks.begin(), currentBooks.end(), [&](const pair<string, time_t>& book) {
                return book.first == bookTitle;
            });
    
            if (it != currentBooks.end()) 
            {
                // Book is being returned
                currentBooks.erase(it);
                borrowedBooks--;
                return true;
            }
            cout << "Book not found in your borrowed list." << endl;
            return false;
        }
    
        double getFine() const { return fine; }
        void setFine(double newFine) { fine = newFine; }
    
        void payFine() {
            if (fine > 0) {
                cout << "Paid fine of " << fine << " rupees." << endl;
                fine = 0;
            } else {
                cout << "No outstanding fines." << endl;
            }
        }
    
        void display() const override {
            // Updating the  fines before displaying
            const_cast<Student*>(this)->updateFines(time(nullptr) / (60 * 60 * 24));
            
            User::display();
            cout << "Borrowed Books: ";
            for (const auto& book : currentBooks) 
            {
                cout << book.first << ", ";
                
                // Displaying the overdue status for each book
                time_t currentDate = time(nullptr) / (60 * 60 * 24);
                int daysOverdue = currentDate - book.second - 15;
                if (daysOverdue > 0) 
                {
                    cout << "(Overdue by " << daysOverdue << " days) ";
                }
            }
            cout << "\nOutstanding Fine: " << fine << " rupees" << endl;
        }
    };


class Faculty : public User 
{
    private:
        int borrowedBooks;
        vector<pair<string, time_t>> currentBooks;
    
    public:
        Faculty(string name, int id) : User(name, id, "Faculty"), borrowedBooks(0) {}
    
        const vector<pair<string, time_t>>& getCurrentBooks() const { return currentBooks; }
    
        // Checking if any current books are overdue by more than 60 days
        bool hasOverdueBooks(time_t currentDate) const {
            for (const auto& bookPair : currentBooks) {
                const string& title = bookPair.first;
                time_t borrowDate = bookPair.second;
                int daysOverdue = currentDate - borrowDate - 30; // 30 days borrowing period
                
                if (daysOverdue > 60) 
                {
                    return true; // Found at least one book overdue by more than 60 days
                }
            }
            return false; 
        }
    
        bool borrowBook(const string& bookTitle, time_t borrowDate) 
        {
            if (borrowedBooks >= 5) 
            {
                cout << "Borrowing limit reached. Cannot borrow more books." << endl;
                return false;
            }
            
            if (hasOverdueBooks(borrowDate)) 
            {
                cout << "Cannot borrow additional books. You have an overdue book for more than 60 days." << endl;
                return false;
            }
            
            currentBooks.push_back({bookTitle, borrowDate});
            borrowedBooks++;
            return true;
        }
    
        bool returnBook(const string& bookTitle, time_t returnDate) 
        {
            auto it = find_if(currentBooks.begin(), currentBooks.end(), [&](const pair<string, time_t>& book) {
                return book.first == bookTitle;
            });
    
            if (it != currentBooks.end()) 
            {
                time_t borrowDate = it->second;
                int daysOverdue = returnDate - borrowDate - 30; // 30 days borrowing period for faculty
                
                if (daysOverdue > 60) 
                {
                    cout << "This book is overdue by more than 60 days." << endl;
                }
                
                currentBooks.erase(it);
                borrowedBooks--;
                return true;
            }
            cout << "Book not found in your borrowed list." << endl;
            return false;
        }
    
        void display() const override {
            User::display();
            cout << "Borrowed Books: ";
            for (const auto& book : currentBooks) {
                cout << book.first << ", ";
            }
            cout << endl;
            
            // Here we are dynamically checking for overdue books when displaying
            time_t currentDate = time(nullptr) / (60 * 60 * 24); // Current date in days
            if (hasOverdueBooks(currentDate)) 
            {
                cout << "You have an overdue book for more than 60 days. Borrowing is restricted." << endl;
            }
        }
    };
// Book class
class Book 
{
private:
    string title;
    string author;
    string publisher;
    int year;
    string ISBN;
    string status;
    string reservedBy;

public:
    Book(string title, string author, string publisher, int year, string ISBN)
        : title(title), author(author), publisher(publisher), year(year), ISBN(ISBN), status("Available"), reservedBy("") {}

    string getTitle() const { return title; }
    string getAuthor() const { return author; }
    string getPublisher() const { return publisher; }
    int getYear() const { return year; }
    string getISBN() const { return ISBN; }
    string getStatus() const { return status; }
    string getReservedBy() const { return reservedBy; }

    void setStatus(const string& newStatus) { status = newStatus; }
    void setReservedBy(const string& userName) { reservedBy = userName; }

    void display() const {
        cout << "Title: " << title << ", Author: " << author << ", Publisher: " << publisher
             << ", Year: " << year << ", ISBN: " << ISBN << ", Status: " << status;
        if (status == "Reserved") 
        {
            cout << ", Reserved By: " << reservedBy;
        }
        cout << endl;
    }
};

// Derived Librarian class
class Librarian : public User 
{
public:
    Librarian(string name, int id) : User(name, id, "Librarian") {}

    void addBook(vector<Book>& books, const Book& book) 
    {
        books.push_back(book);
        cout << "Book added: " << book.getTitle() << endl;
    }

    void removeBook(vector<Book>& books, const string& bookTitle) 
    {
        auto it = find_if(books.begin(), books.end(), [&](const Book& book) {
            return book.getTitle() == bookTitle;
        });

        if (it != books.end()) 
        {
            books.erase(it);
            cout << "Book removed: " << bookTitle << endl;
        } 
        else 
        {
            cout << "Book not found." << endl;
        }
    }

    void displayBooks(const vector<Book>& books) const {
        cout << "Available Books:" << endl;
        for (const auto& book : books) 
        {
            book.display();
        }
        cout << endl;
    }
};


class Account 
{
    private:
        shared_ptr<User> user;
        vector<string> borrowedBooks;
        double fine;
    
    public:
        Account(shared_ptr<User> user) : user(user), fine(0) 
        {
            // Initializing borrowedBooks based on user's currentBooks
            syncBorrowedBooks();
        }
    
        void syncBorrowedBooks() 
        {
            borrowedBooks.clear();
    
            if (user->getRole() == "Student") 
            {
                Student* student = dynamic_cast<Student*>(user.get());
                if (student) 
                {
                    for (const auto& bookPair : student->getCurrentBooks()) 
                    {
                        borrowedBooks.push_back(bookPair.first);
                    }
                }
            } 
            else if (user->getRole() == "Faculty") 
            {
                Faculty* faculty = dynamic_cast<Faculty*>(user.get());
                if (faculty) 
                {
                    for (const auto& bookPair : faculty->getCurrentBooks()) 
                    {
                        borrowedBooks.push_back(bookPair.first);
                    }
                }
            }
        }
    
        shared_ptr<User> getUser() const { return user; }
    
        void borrowBook(const string& bookTitle, time_t borrowDate, vector<Book>& books) {
            // First checking if the book exists and is available for borrowing
            auto bookIt = find_if(books.begin(), books.end(), [&](const Book& book) {
                return book.getTitle() == bookTitle && book.getStatus() == "Available";
            });
    
            if (bookIt == books.end()) 
            {
                cout << "Book not found or not available." << endl;
                return;
            }
    
            bool success = false;
            if (user->getRole() == "Student") 
            {
                Student* student = dynamic_cast<Student*>(user.get());
                if (student) 
                {
                    success = student->borrowBook(bookTitle, borrowDate);
                }
            } 
            else if (user->getRole() == "Faculty") 
            {
                Faculty* faculty = dynamic_cast<Faculty*>(user.get());
                if (faculty) 
                {
                    success = faculty->borrowBook(bookTitle, borrowDate);
                }
            } 
            else 
            {
                cout << "Librarians cannot borrow books." << endl;
                return;
            }
    
            if (success) 
            {
                borrowedBooks.push_back(bookTitle);
                bookIt->setStatus("Borrowed");
                bookIt->setReservedBy(user->getName());
                cout << "Book borrowed successfully: " << bookTitle << endl;
            }
        }
    
        void returnBook(const string& bookTitle, time_t returnDate, vector<Book>& books) 
        {
            bool success = false;
            if (user->getRole() == "Student") 
            {
                Student* student = dynamic_cast<Student*>(user.get());
                if (student) 
                {
                    success = student->returnBook(bookTitle, returnDate);
                }
            } 
            else if (user->getRole() == "Faculty") 
            {
                Faculty* faculty = dynamic_cast<Faculty*>(user.get());
                if (faculty) 
                {
                    success = faculty->returnBook(bookTitle, returnDate);
                }
            } 
            else 
            {
                cout << "Librarians cannot return books." << endl;
                return;
            }
    
            if (success) 
            {
                // Updating the borrowedBooks list
                auto it = find(borrowedBooks.begin(), borrowedBooks.end(), bookTitle);
                if (it != borrowedBooks.end()) 
                {
                    borrowedBooks.erase(it);
                }
                
                auto bookIt = find_if(books.begin(), books.end(), [&](const Book& book) {
                    return book.getTitle() == bookTitle;
                });
                
                if (bookIt != books.end()) 
                {
                    bookIt->setStatus("Available");
                    bookIt->setReservedBy("");
                    cout << "Book returned successfully: " << bookTitle << endl;
                } 
                else 
                {
                    cout << "Warning: Book not found in library collection." << endl;
                }
                syncBorrowedBooks();
            }
        }
    
        void payFine() 
        {
            if (user->getRole() == "Student") 
            {
                Student* student = dynamic_cast<Student*>(user.get());
                if (student) 
                {
                    student->payFine();
                    // Here we are updating the account's fine value
                    fine = student->getFine();
                }
            } 
            else 
            {
                cout << "No fines for faculty or librarians." << endl;
            }
        }
    
        void display() const {
            user->display();
            cout << "Borrowed Books: ";
            for (const auto& book : borrowedBooks) {
                cout << book << ", ";
            }
            cout << "\nOutstanding Fine: " << fine << " rupees" << endl;
        }
    };
// Library class
class Library 
{
private:
    vector<Book> books;
    vector<Account> accounts;

public:
    vector<Book>& getBooks() { return books; }
    const vector<Book>& getBooks() const { return books; }

    void addBook(const Book& book, const Librarian& librarian) 
    {
        books.push_back(book);
        cout << "Librarian " << librarian.getName() << " added the book: " << book.getTitle() << endl;
    }

    void removeBook(const string& bookTitle, const Librarian& librarian) 
    {
        auto it = find_if(books.begin(), books.end(), [&](const Book& book) {
            return book.getTitle() == bookTitle;
        });

        if (it != books.end()) 
        {
            books.erase(it);
            cout << "Librarian " << librarian.getName() << " removed the book: " << bookTitle << endl;
        } 
        else 
        {
            cout << "Book not found." << endl;
        }
    }

    void addAccount(Account account, const Librarian& librarian) 
    {
        cout << "Librarian " << librarian.getName() << " added the account for: " << account.getUser()->getName() << endl;
        accounts.push_back(std::move(account));
    }

    void removeAccount(const string& userName, const Librarian& librarian) 
    {
        auto it = find_if(accounts.begin(), accounts.end(), [&](const Account& account) {
            return account.getUser()->getName() == userName;
        });

        if (it != accounts.end()) 
        {
            accounts.erase(it);
            cout << "Librarian " << librarian.getName() << " removed the account for: " << userName << endl;
        } 
        else 
        {
            cout << "Account not found." << endl;
        }
    }

    void displayBooks() const {
        cout << "Library Books:" << endl;
        for (const auto& book : books) {
            book.display();
        }
    }

    void displayAccounts() const {
        cout << "Library Accounts:" << endl;
        for (const auto& account : accounts) {
            account.display();
        }
    }

    vector<Account>& getAccounts() { return accounts; }

    // Loading the library state from files
    void loadState() 
    {
        try {
            ifstream bookFile("books.txt");
            if (bookFile.is_open()) {
                string line;
                while (getline(bookFile, line)) {
                    stringstream ss(line);
                    string title, author, publisher, yearStr, ISBN, status, reservedBy;
                    getline(ss, title, ',');
                    getline(ss, author, ',');
                    getline(ss, publisher, ',');
                    getline(ss, yearStr, ',');
                    getline(ss, ISBN, ',');
                    getline(ss, status, ',');
                    getline(ss, reservedBy, ',');

                    Book book(title, author, publisher, stoi(yearStr), ISBN);
                    book.setStatus(status);
                    book.setReservedBy(reservedBy);
                    books.push_back(book);
                }
                bookFile.close();
                cout << "Books loaded successfully." << endl;
            } else {
                cout << "No existing books file found. Starting with empty library." << endl;
            }
        } catch (const exception& e) {
            cerr << "Error loading books: " << e.what() << endl;
        }

        try {
            ifstream accountFile("accounts.txt");
            if (accountFile.is_open()) {
                string accountLine;
                while (getline(accountFile, accountLine)) {
                    stringstream ss(accountLine);
                    string name, idStr, role, bookStr, fineStr;
                    getline(ss, name, ',');
                    getline(ss, idStr, ',');
                    getline(ss, role, ',');

                    int id = stoi(idStr);
                    shared_ptr<User> newUser;

                    if (role == "Student") {
                        auto student = make_shared<Student>(name, id);
                        while (getline(ss, bookStr, ',') && !bookStr.empty() && bookStr.find(':') != string::npos) {
                            stringstream bookSS(bookStr);
                            string bookTitle;
                            time_t borrowDate;
                            getline(bookSS, bookTitle, ':');
                            bookSS >> borrowDate;
                            student->borrowBook(bookTitle, borrowDate);
                        }
                        
                        // The last item should be the fine
                        if (!bookStr.empty() && bookStr.find(':') == string::npos) {
                            fineStr = bookStr;
                        } else if (getline(ss, fineStr, ',')) {
                            // Here we are extracting the fine if there's still more to read
                        }
                        
                        try {
                            if (!fineStr.empty()) {
                                student->setFine(stod(fineStr));
                            }
                        } catch (const exception& e) {
                            cerr << "Error parsing fine: " << e.what() << " for " << name << endl;
                            student->setFine(0);
                        }
                        newUser = student;
                    } else if (role == "Faculty") {
                        auto faculty = make_shared<Faculty>(name, id);
                        while (getline(ss, bookStr, ',') && !bookStr.empty() && bookStr.find(':') != string::npos) {
                            stringstream bookSS(bookStr);
                            string bookTitle;
                            time_t borrowDate;
                            getline(bookSS, bookTitle, ':');
                            bookSS >> borrowDate;
                            faculty->borrowBook(bookTitle, borrowDate);
                        }
                        newUser = faculty;
                    } else if (role == "Librarian") {
                        newUser = make_shared<Librarian>(name, id);
                    }

                    if (newUser) {
                        accounts.push_back(Account(newUser));
                    }
                }
                accountFile.close();
                cout << "Accounts loaded successfully." << endl;
            } else {
                cout << "No existing accounts file found. Starting with empty accounts." << endl;
                // Creating a default librarian account if there are no accounts
                if (accounts.empty()) {
                    auto defaultLibrarian = make_shared<Librarian>("Admin", 1);
                    accounts.push_back(Account(defaultLibrarian));
                    cout << "Created default librarian account (Name: Admin, ID: 1)" << endl;
                }
            }
        } catch (const exception& e) {
            cerr << "Error loading accounts: " << e.what() << endl;
            // Creating a default librarian account if there was an error
            if (accounts.empty()) {
                auto defaultLibrarian = make_shared<Librarian>("Admin", 1);
                accounts.push_back(Account(defaultLibrarian));
                cout << "Created default librarian account (Name: Admin, ID: 1)" << endl;
            }
        }
    }

    // Saving the library state to files
    void saveState() const {
        try {
            ofstream bookFile("books.txt");
            if (!bookFile.is_open()) {
                cerr << "Error: Could not open books.txt for writing" << endl;
                return;
            }
            
            for (const auto& book : books) {
                bookFile << book.getTitle() << "," << book.getAuthor() << "," << book.getPublisher() << ","
                         << book.getYear() << "," << book.getISBN() << "," << book.getStatus() << "," << book.getReservedBy() << "\n";
            }
            bookFile.close();
            cout << "Books saved successfully." << endl;
        } catch (const exception& e) {
            cerr << "Error saving books: " << e.what() << endl;
        }

        try {
            ofstream accountFile("accounts.txt");
            if (!accountFile.is_open()) {
                cerr << "Error: Could not open accounts.txt for writing" << endl;
                return;
            }
            
            for (const auto& account : accounts) {
                if (account.getUser() == nullptr) continue;
                
                accountFile << account.getUser()->getName() << "," << account.getUser()->getId() << ","
                           << account.getUser()->getRole() << ",";
                
                if (account.getUser()->getRole() == "Student") {
                    Student* student = dynamic_cast<Student*>(account.getUser().get());
                    if (student) {
                        for (const auto& book : student->getCurrentBooks()) {
                            accountFile << book.first << ":" << book.second << ",";
                        }
                        accountFile << student->getFine();
                    }
                } else if (account.getUser()->getRole() == "Faculty") {
                    Faculty* faculty = dynamic_cast<Faculty*>(account.getUser().get());
                    if (faculty) {
                        for (const auto& book : faculty->getCurrentBooks()) {
                            accountFile << book.first << ":" << book.second << ",";
                        }
                    }
                }
                
                accountFile << "\n";
            }
            accountFile.close();
            cout << "Accounts saved successfully." << endl;
        } catch (const exception& e) {
            cerr << "Error saving accounts: " << e.what() << endl;
        }
    }
};

int main() 
{
    Library library;

    // Loading library state from files (if they exist)
    library.loadState();

    while (true) 
    {
        cout << "\n------------------------------------------" << endl;
        cout << "Welcome to the Library Management System!" << endl;
        cout << "------------------------------------------" << endl;
        cout << "Please select your role:" << endl;
        cout << "1. Student" << endl;
        cout << "2. Faculty" << endl;
        cout << "3. Librarian" << endl;
        cout << "4. Exit" << endl;
        cout << "Enter your choice: ";

        int roleChoice;
        cin >> roleChoice;

        if (roleChoice == 4) 
        {
            // Save state before exiting
            library.saveState();
            cout << "Thank you for using the Library Management System. Goodbye!" << endl;
            break;
        }

        string userName;
        int userId;
        cout << "Enter your name: ";
        cin.ignore(); // Here we are clearing the input buffer
        getline(cin, userName);
        cout << "Enter your ID: ";
        cin >> userId;
        cin.ignore(); // Here we are clearing the input buffer

        // Here we have implemented the faculty role handling
        if (roleChoice == 2) 
        {
            bool accountFound = false;
            Account* facultyAccount = nullptr;
            
            for (auto& account : library.getAccounts()) 
            {
                if (account.getUser()->getName() == userName && 
                    account.getUser()->getId() == userId) {
                    

                    if (account.getUser()->getRole() != "Faculty") 
                    {
                        cout << "Error: User '" << userName << "' exists but is not a faculty member." << endl;
                        cout << "This user has role: " << account.getUser()->getRole() << endl;
                        accountFound = true;
                        break;
                    }
                    
                    facultyAccount = &account;
                    accountFound = true;
                    break;
                }
            }
            
            if (!accountFound) 
            {
                cout << "No account found with name '" << userName << "' and ID " << userId << "." << endl;
                cout << "Please contact a librarian to create an account." << endl;
                continue;
            }
            
            if (facultyAccount == nullptr) 
            {
                continue;
            }

            Faculty* faculty = dynamic_cast<Faculty*>(facultyAccount->getUser().get());
            if (!faculty) 
            {
                cout << "Error: Failed to access faculty data. Please try again." << endl;
                continue;
            }

            bool facultySessionActive = true;
            while (facultySessionActive) 
            {
                cout << "\n------------------------------------------" << endl;
                cout << "Faculty Menu:" << endl;
                cout << "------------------------------------------" << endl;
                cout << "1. Borrow Book" << endl;
                cout << "2. Return Book" << endl;
                cout << "3. Display Account" << endl;
                cout << "4. Display Available Books" << endl;
                cout << "5. Exit" << endl;
                cout << "Enter your choice: ";

                int facultyChoice;
                cin >> facultyChoice;

                if (facultyChoice == 5) 
                {
                    facultySessionActive = false;
                    break;
                }

                if (facultyChoice == 1) 
                {
                    string bookTitle;
                    cout << "Enter the book title: ";
                    cin.ignore();
                    getline(cin, bookTitle);
                    time_t currentDate = getCurrentDate();
                    facultyAccount->borrowBook(bookTitle, currentDate, library.getBooks());
                    library.saveState();
                } 
                else if (facultyChoice == 2) 
                {
                    string bookTitle;
                    cout << "Enter the book title: ";
                    cin.ignore();
                    getline(cin, bookTitle);
                    time_t currentDate = getCurrentDate();
                    facultyAccount->returnBook(bookTitle, currentDate, library.getBooks());
                    library.saveState();
                } 
                else if (facultyChoice == 3) 
                {
                    faculty->display();
                } 
                else if (facultyChoice == 4) 
                {
                    library.displayBooks();
                } 
                else 
                {
                    cout << "Invalid choice. Please try again." << endl;
                }
            }
        } 
        else if (roleChoice == 1) 
        {
            bool accountFound = false;
            Account* studentAccount = nullptr;
            
            for (auto& account : library.getAccounts()) {
                if (account.getUser()->getName() == userName && 
                    account.getUser()->getId() == userId) 
                    {
                    if (account.getUser()->getRole() != "Student") 
                    {
                        cout << "Error: User '" << userName << "' exists but is not a student." << endl;
                        cout << "This user has role: " << account.getUser()->getRole() << endl;
                        accountFound = true;
                        break;
                    }
                    
                    studentAccount = &account;
                    accountFound = true;
                    break;
                }
            }
            
            if (!accountFound) 
            {
                cout << "No account found with name '" << userName << "' and ID " << userId << "." << endl;
                cout << "Please contact a librarian to create an account." << endl;
                continue;
            }
            
            if (studentAccount == nullptr) 
            {
                continue;
            }

            Student* student = dynamic_cast<Student*>(studentAccount->getUser().get());
            if (!student) 
            {
                cout << "Error: Failed to access student data. Please try again." << endl;
                continue;
            }

            bool studentSessionActive = true;
            while (studentSessionActive) 
            {
                cout << "\n------------------------------------------" << endl;
                cout << "Student Menu:" << endl;
                cout << "------------------------------------------" << endl;
                cout << "1. Borrow Book" << endl;
                cout << "2. Return Book" << endl;
                cout << "3. Pay Fine" << endl;
                cout << "4. Display Account" << endl;
                cout << "5. Display Available Books" << endl;
                cout << "6. Exit" << endl;
                cout << "Enter your choice: ";

                int studentChoice;
                cin >> studentChoice;

                if (studentChoice == 6) 
                {
                    studentSessionActive = false;
                    break;
                }

                if (studentChoice == 1) 
                {
                    string bookTitle;
                    cout << "Enter the book title: ";
                    cin.ignore();
                    getline(cin, bookTitle);
                    time_t currentDate = getCurrentDate();
                    
                    // Here we are using the Account's borrowBook method which properly updates both user and book data
                    studentAccount->borrowBook(bookTitle, currentDate, library.getBooks());
                    library.saveState();
                } 
                else if (studentChoice == 2) 
                {
                    string bookTitle;
                    cout << "Enter the book title: ";
                    cin.ignore();
                    getline(cin, bookTitle);
                    time_t currentDate = getCurrentDate();
                    studentAccount->returnBook(bookTitle, currentDate, library.getBooks());
                    library.saveState();
                } 
                else if (studentChoice == 3) 
                {
                    studentAccount->payFine();
                    library.saveState();
                } 
                else if (studentChoice == 4) 
                {
                    student->display();
                } 
                else if (studentChoice == 5) 
                {
                    library.displayBooks();
                } 
                else 
                {
                    cout << "Invalid choice. Please try again." << endl;
                }
            }
        } 
        else if (roleChoice == 3) 
        {
            bool accountFound = false;
            Account* librarianAccount = nullptr;
            
            for (auto& account : library.getAccounts()) 
            {
                if (account.getUser()->getName() == userName && 
                    account.getUser()->getId() == userId) {
                    

                    if (account.getUser()->getRole() != "Librarian") 
                    {
                        cout << "Error: User '" << userName << "' exists but is not a librarian." << endl;
                        cout << "This user has role: " << account.getUser()->getRole() << endl;
                        accountFound = true;
                        break;
                    }
                    
                    librarianAccount = &account;
                    accountFound = true;
                    break;
                }
            }
            
            if (!accountFound) 
            {
                cout << "No account found with name '" << userName << "' and ID " << userId << "." << endl;
                if (userName == "Admin" && userId == 1) 
                {
                    cout << "Creating default librarian account..." << endl;
                    auto defaultLibrarian = make_shared<Librarian>("Admin", 1);
                    library.getAccounts().push_back(Account(defaultLibrarian));
                    librarianAccount = &library.getAccounts().back();
                    library.saveState();
                } 
                else 
                {
                    cout << "Please use the default librarian account (Admin, ID: 1) to create new accounts." << endl;
                    continue;
                }
            }
            
            if (librarianAccount == nullptr) 
            {
                continue;
            }

            Librarian* librarian = dynamic_cast<Librarian*>(librarianAccount->getUser().get());
            if (!librarian) 
            {
                cout << "Error: Failed to access librarian data. Please try again." << endl;
                continue;
            }

            bool librarianSessionActive = true;
            while (librarianSessionActive) 
            {
                cout << "\n------------------------------------------" << endl;
                cout << "Librarian Menu:" << endl;
                cout << "------------------------------------------" << endl;
                cout << "1. Add Book" << endl;
                cout << "2. Remove Book" << endl;
                cout << "3. Add Account" << endl;
                cout << "4. Remove Account" << endl;
                cout << "5. Display Books" << endl;
                cout << "6. Display Accounts" << endl;
                cout << "7. Exit" << endl;
                cout << "Enter your choice: ";

                int librarianChoice;
                cin >> librarianChoice;

                if (librarianChoice == 7) 
                {
                    librarianSessionActive = false;
                    break;
                }

                if (librarianChoice == 1) 
                {
                    string title, author, publisher, ISBN;
                    int year;
                    
                    cout << "Enter book title: ";
                    cin.ignore();
                    getline(cin, title);
                    
                    cout << "Enter author: ";
                    getline(cin, author);
                    
                    cout << "Enter publisher: ";
                    getline(cin, publisher);
                    
                    cout << "Enter year: ";
                    cin >> year;
                    
                    cout << "Enter ISBN: ";
                    cin.ignore();
                    getline(cin, ISBN);
                    
                    Book newBook(title, author, publisher, year, ISBN);
                    librarian->addBook(library.getBooks(), newBook);
                    library.saveState();
                } 
                else if (librarianChoice == 2) 
                {
                    string bookTitle;
                    cout << "Enter the book title to remove: ";
                    cin.ignore();
                    getline(cin, bookTitle);
                    
                    librarian->removeBook(library.getBooks(), bookTitle);
                    library.saveState();
                } 
                else if (librarianChoice == 3) 
                {
                    string newUserName;
                    int newUserId;
                    int userType;
                    
                    cout << "Enter new user name: ";
                    cin.ignore();
                    getline(cin, newUserName);
                    
                    cout << "Enter new user ID: ";
                    cin >> newUserId;
                    
                    cout << "Select user type:" << endl;
                    cout << "1. Student" << endl;
                    cout << "2. Faculty" << endl;
                    cout << "3. Librarian" << endl;
                    cout << "Enter choice: ";
                    cin >> userType;
                    
                    shared_ptr<User> newUser;
                    if (userType == 1) 
                    {
                        newUser = make_shared<Student>(newUserName, newUserId);
                    } 
                    else if (userType == 2) 
                    {
                        newUser = make_shared<Faculty>(newUserName, newUserId);
                    } 
                    else if (userType == 3) 
                    {
                        newUser = make_shared<Librarian>(newUserName, newUserId);
                    } 
                    else 
                    {
                        cout << "Invalid user type. Account creation failed." << endl;
                        continue;
                    }
                    
                    Account newAccount(newUser);
                    library.addAccount(std::move(newAccount), *librarian);
                    library.saveState();
                } 
                else if (librarianChoice == 4) 
                {
                    string userName;
                    cout << "Enter the user name to remove: ";
                    cin.ignore();
                    getline(cin, userName);
                    
                    library.removeAccount(userName, *librarian);
                    library.saveState();
                } 
                else if (librarianChoice == 5) 
                {
                    library.displayBooks();
                } 
                else if (librarianChoice == 6) 
                {
                    library.displayAccounts();
                } 
                else 
                {
                    cout << "Invalid choice. Please try again." << endl;
                }
            }
        } 
        else 
        {
            cout << "Invalid role choice. Please try again." << endl;
        }
    }

    return 0;
}