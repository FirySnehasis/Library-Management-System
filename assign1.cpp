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
class User 
{
protected:
    string buddyName;       
    int buddyID;            
    string jobType;         

public:
    User(string nm, int idd, string type) : buddyName(nm), buddyID(idd), jobType(type) {}

    string getName() const { return buddyName; }
    int getId() const { return buddyID; }
    string getRole() const { return jobType; }

    virtual void display() const {
        cout << "Yo! I'm " << buddyName << " (ID: " << buddyID << "), rockin' as a " << jobType << "!" << endl;
    }

    virtual ~User() {}
};

// Derived Student class
class Student : public User 
{
private:
    int bookCount;                         
    vector<pair<string, time_t>> borrowedBooks;    
    double outstandingFine;                       

public:
    Student(string nm, int idd) : User(nm, idd, "Student"), bookCount(0), outstandingFine(0) {}
    
    const vector<pair<string, time_t>>& getCurrentBooks() const { return borrowedBooks; }
    
    void updateFines(time_t now) {
        double totalFine = 0;
        for (const auto& book : borrowedBooks) {
            time_t bday = book.second;
            int lateDays = now - bday - 15;
            if (lateDays > 0)
                totalFine += lateDays * 10;
        }
        outstandingFine = totalFine;
    }
    
    bool borrowBook(const string& bookTitle, time_t bDay) {
        updateFines(bDay);
        if (bookCount >= 3) {
            cout << "You've reached the borrowing limit. Please return a book first." << endl;
            return false;
        }
        if (outstandingFine > 0) {
            cout << "You have an outstanding fine of " << outstandingFine << " rupees. Please pay it before borrowing more books." << endl;
            return false;
        }
        borrowedBooks.push_back({bookTitle, bDay});
        bookCount++;
        cout << "Book '" << bookTitle << "' borrowed successfully. Total books now: " << bookCount << "." << endl;
        return true;
    }
    
    bool returnBook(const string& bookTitle, time_t retDay) {
        updateFines(retDay);
        auto it = find_if(borrowedBooks.begin(), borrowedBooks.end(), [&](const pair<string, time_t>& pair) {
            return pair.first == bookTitle;
        });
        if (it != borrowedBooks.end()) {
            borrowedBooks.erase(it);
            bookCount--;
            cout << "Book '" << bookTitle << "' returned. Total books now: " << bookCount << "." << endl;
            return true;
        }
        cout << "The book '" << bookTitle << "' is not in your borrowed list." << endl;
        return false;
    }
    
    double getFine() const { return outstandingFine; }
    void setFine(double newFine) { outstandingFine = newFine; }
    
    void payFine() {
        if (outstandingFine > 0) {
            cout << "You paid " << outstandingFine << " rupees. Thank you." << endl;
            outstandingFine = 0;
        } else {
            cout << "You don't have any outstanding fines." << endl;
        }
    }
    
    void display() const override {
        const_cast<Student*>(this)->updateFines(getCurrentDate());
        User::display();
        cout << "Borrowed Books: ";
        for (const auto& bk : borrowedBooks) {
            cout << bk.first << ", ";
            int late = (int)(getCurrentDate() - bk.second - 15);
            if (late > 0)
                cout << "(Overdue by " << late << " days) ";
        }
        cout << "\nOutstanding Fine: " << outstandingFine << " rupees" << endl;
    }
};


class Faculty : public User 
{
private:
    int bookCount;   
    vector<pair<string, time_t>> borrowedBooks;   
    
public:
    Faculty(string nm, int idd) : User(nm, idd, "Faculty"), bookCount(0) {}
    
    const vector<pair<string, time_t>>& getCurrentBooks() const { return borrowedBooks; }
    
    bool hasOverdueBooks(time_t now) const {
        for (const auto& bk : borrowedBooks) {
            int late = now - bk.second - 30;
            if (late > 60)
                return true;
        }
        return false;
    }
    
    bool borrowBook(const string& bookTitle, time_t bDay) {
        if (bookCount >= 5) {
            cout << "You've reached your borrowing limit. Please return a book first." << endl;
            return false;
        }
        if (hasOverdueBooks(bDay)) {
            cout << "You have a book overdue by more than 60 days. Return it before borrowing new ones." << endl;
            return false;
        }
        borrowedBooks.push_back({bookTitle, bDay});
        bookCount++;
        cout << "Book '" << bookTitle << "' borrowed successfully." << endl;
        return true;
    }
    
    bool returnBook(const string& bookTitle, time_t retDay) {
        auto it = find_if(borrowedBooks.begin(), borrowedBooks.end(), [&](const pair<string, time_t>& pair) {
            return pair.first == bookTitle;
        });
        if (it != borrowedBooks.end()) {
            int late = retDay - it->second - 30;
            if (late > 60)
                cout << "Note: This book is very overdue." << endl;
            borrowedBooks.erase(it);
            bookCount--;
            cout << "Book '" << bookTitle << "' returned successfully." << endl;
            return true;
        }
        cout << "Book '" << bookTitle << "' not found in your borrowed list." << endl;
        return false;
    }
    
    void display() const override {
        User::display();
        cout << "Borrowed Books: ";
        for (const auto& bk : borrowedBooks) {
            cout << bk.first << ", ";
        }
        cout << endl;
        if (hasOverdueBooks(getCurrentDate()))
            cout << "Please note: You have a book overdue by more than 60 days." << endl;
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
    Librarian(string nm, int idd) : User(nm, idd, "Librarian") {}
    
    void addBook(vector<Book>& books, const Book& book) {
        books.push_back(book);
        cout << "Boom! Added the book: " << book.getTitle() << " to the gig." << endl;
    }
    
    void removeBook(vector<Book>& books, const string& booTitle) {
        auto it = find_if(books.begin(), books.end(), [&](const Book& bk) {
            return bk.getTitle() == booTitle;
        });
        if (it != books.end()) {
            books.erase(it);
            cout << "Removed '" << booTitle << "'—gone like last night's pizza!" << endl;
        } else {
            cout << "Bummer, '" << booTitle << "' was not found." << endl;
        }
    }
    
    void displayBooks(const vector<Book>& books) const {
        cout << "Here's the cool collection:" << endl;
        for (const auto& bk : books)
            bk.display();
        cout << endl;
    }
};


class Account 
{
private:
    shared_ptr<User> user;
    vector<string> borrowedBooksList;  
    double fineAmount;          
    
public:
    Account(shared_ptr<User> usr) : user(usr), fineAmount(0) 
    {
        syncBorrowedBooks();
    }
    
    void syncBorrowedBooks() {
        borrowedBooksList.clear();
        if (user->getRole() == "Student") {
            Student* stu = dynamic_cast<Student*>(user.get());
            if (stu) {
                for (const auto& pair : stu->getCurrentBooks())
                    borrowedBooksList.push_back(pair.first);
            }
        } else if (user->getRole() == "Faculty") {
            Faculty* fac = dynamic_cast<Faculty*>(user.get());
            if (fac) {
                for (const auto& pair : fac->getCurrentBooks())
                    borrowedBooksList.push_back(pair.first);
            }
        }
    }
    
    shared_ptr<User> getUser() const { return user; }
    
    void borrowBook(const string& bookTitle, time_t bDay, vector<Book>& books) {
        auto it = find_if(books.begin(), books.end(), [&](const Book& bk) {
            return bk.getTitle() == bookTitle && bk.getStatus() == "Available";
        });
        if (it == books.end()) {
            cout << "The book '" << bookTitle << "' is not available." << endl;
            return;
        }
        bool success = false;
        if (user->getRole() == "Student") {
            Student* stu = dynamic_cast<Student*>(user.get());
            if (stu)
                success = stu->borrowBook(bookTitle, bDay);
        } else if (user->getRole() == "Faculty") {
            Faculty* fac = dynamic_cast<Faculty*>(user.get());
            if (fac)
                success = fac->borrowBook(bookTitle, bDay);
        } else {
            cout << "Librarians do not borrow books." << endl;
            return;
        }
        if (success) {
            borrowedBooksList.push_back(bookTitle);
            it->setStatus("Borrowed");
            it->setReservedBy(user->getName());
            cout << "Book '" << bookTitle << "' has been borrowed." << endl;
        }
    }
    
    void returnBook(const string& bookTitle, time_t rDay, vector<Book>& books) {
        bool success = false;
        if (user->getRole() == "Student") {
            Student* stu = dynamic_cast<Student*>(user.get());
            if (stu)
                success = stu->returnBook(bookTitle, rDay);
        } else if (user->getRole() == "Faculty") {
            Faculty* fac = dynamic_cast<Faculty*>(user.get());
            if (fac)
                success = fac->returnBook(bookTitle, rDay);
        } else {
            cout << "Librarians do not return books." << endl;
            return;
        }
        if (success) {
            auto it = find(borrowedBooksList.begin(), borrowedBooksList.end(), bookTitle);
            if (it != borrowedBooksList.end())
                borrowedBooksList.erase(it);
            auto bookIt = find_if(books.begin(), books.end(), [&](const Book& bk) {
                return bk.getTitle() == bookTitle;
            });
            if (bookIt != books.end()) {
                bookIt->setStatus("Available");
                bookIt->setReservedBy("");
                cout << "Book '" << bookTitle << "' returned successfully." << endl;
            } else {
                cout << "Warning: The book '" << bookTitle << "' was not found in the library collection." << endl;
            }
            syncBorrowedBooks();
        }
    }
    
    void payFine() {
        if (user->getRole() == "Student") {
            Student* stu = dynamic_cast<Student*>(user.get());
            if (stu) {
                stu->payFine();
                fineAmount = stu->getFine();
            }
        } else {
            cout << "No fines applicable for your role." << endl;
        }
    }
    
    void display() const {
        user->display();
        cout << "Borrowed Books: ";
        for (const auto& b : borrowedBooksList)
            cout << b << ", ";
        cout << "\nOutstanding Fine: " << fineAmount << " rupees" << endl;
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
    
    void addBook(const Book& bk, const Librarian& lib) {
        books.push_back(bk);
        cout << "Librarian " << lib.getName() << " just tossed in '" << bk.getTitle() << "'." << endl;
    }
    
    void removeBook(const string& booTitle, const Librarian& lib) {
        auto it = find_if(books.begin(), books.end(), [&](const Book& bk) {
            return bk.getTitle() == booTitle;
        });
        if (it != books.end()) {
            books.erase(it);
            cout << "Librarian " << lib.getName() << " booted out '" << booTitle << "'." << endl;
        } else {
            cout << "Oops, '" << booTitle << "' couldn't be found." << endl;
        }
    }
    
    void addAccount(Account acc, const Librarian& lib) {
        cout << "Librarian " << lib.getName() << " added account for " << acc.getUser()->getName() << "." << endl;
        accounts.push_back(std::move(acc));
    }
    
    void removeAccount(const string& usrName, const Librarian& lib) {
        auto it = find_if(accounts.begin(), accounts.end(), [&](const Account& acc) {
            return acc.getUser()->getName() == usrName;
        });
        if (it != accounts.end()) {
            accounts.erase(it);
            cout << "Librarian " << lib.getName() << " axed account for " << usrName << "." << endl;
        } else {
            cout << "Account for " << usrName << " not found." << endl;
        }
    }
    
    void displayBooks() const {
        cout << "Library Books:" << endl;
        for (const auto& bk : books)
            bk.display();
    }
    
    void displayAccounts() const {
        cout << "Library Accounts:" << endl;
        for (const auto& acc : accounts)
            acc.display();
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
        cout << "Welcome to the the IITK Library Management System!" << endl;
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
