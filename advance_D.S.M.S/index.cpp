// default username = admin and password=admin123

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>
#include <algorithm>
#include <cstring> // For password encryption
#include "utils.h"

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
// Implementation of getch() for non-Windows systems
char getch() {
    char buf = 0;
    struct termios old = {0};
    if (tcgetattr(0, &old) < 0) perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN] = 1;
    old.c_cc[VTIME] = 0;
    if (tcsetattr(0, TCSANOW, &old) < 0) perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0) perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0) perror("tcsetattr ~ICANON");
    return buf;
}
#endif

using namespace std;

// Structure to store user credentials
struct User {
    string username;
    string passwordHash; // Storing hashed password, not plain text
    bool isAdmin;
    
    User(const string& user, const string& hash, bool admin = false)
        : username(user), passwordHash(hash), isAdmin(admin) {}
};

// Structure to store item information
struct Item {
    string name;
    int price;
    int quantity;
    bool isSold;
    
    // Constructor
    Item(const string& n, int p, int q, bool sold = false)
        : name(n), price(p), quantity(q), isSold(sold) {}
};

// Structure to store transaction information
struct Transaction {
    int year, month, date;
    vector<Item> items;
    string username; // Add username to track who made the transaction
    
    // Constructor
    Transaction(int y, int m, int d, const string& user) 
        : year(y), month(m), date(d), username(user) {}
};

// Global variables
vector<User> users;
User* currentUser = nullptr;
const string usersFile = "users.dat";

// Simple encryption function (not secure, but adequate for this example)
string encryptPassword(const string& password) {
    string result = password;
    // Simple XOR encryption with a fixed key
    for (size_t i = 0; i < result.length(); i++) {
        result[i] = result[i] ^ 42; // XOR with 42
    }
    return result;
}

// Function to load users from file
void loadUsers() {
    ifstream file(usersFile);
    if (!file.is_open()) {
        // Create default admin user if file doesn't exist
        users.push_back(User("admin", encryptPassword("admin123"), true));
        return;
    }
    
    string username, passwordHash;
    int isAdmin;
    
    while (file >> username >> passwordHash >> isAdmin) {
        users.push_back(User(username, passwordHash, isAdmin != 0));
    }
    
    file.close();
    
    // Add default admin if no users exist
    if (users.empty()) {
        users.push_back(User("admin", encryptPassword("admin123"), true));
    }
}

// Function to save users to file
void saveUsers() {
    ofstream file(usersFile);
    if (!file.is_open()) {
        cerr << "Error opening users file for writing.\n";
        return;
    }
    
    for (const auto& user : users) {
        file << user.username << " " 
             << user.passwordHash << " " 
             << (user.isAdmin ? 1 : 0) << endl;
    }
    
    file.close();
}

// Function to get a password without showing it on screen
string getPassword() {
    string password;
    char ch;
    
    cout << "Password: ";
    while ((ch = getch()) != '\r' && ch != '\n') {
        if (ch == '\b') { // Handle backspace
            if (!password.empty()) {
                password.pop_back();
                cout << "\b \b"; // Erase character from screen
            }
        } else {
            password.push_back(ch);
            cout << '*'; // Show * instead of the actual character
        }
    }
    cout << endl;
    
    return password;
}

// Function to handle login
bool login() {
    string username;
    string password;
    int attempts = 0;
    const int maxAttempts = 3;
    
    while (attempts < maxAttempts) {
        cout << "Enter username: ";
        getline(cin, username);
        
        password = getPassword();
        
        string encryptedPassword = encryptPassword(password);
        
        for (auto& user : users) {
            if (user.username == username && user.passwordHash == encryptedPassword) {
                currentUser = &user;
                cout << "Login successful. Welcome, " << username << "!" << endl;
                cout << "User role: " << (user.isAdmin ? "Administrator" : "Regular User") << endl;
                cout << "Press any key to continue...";
                getch();
                return true;
            }
        }
        
        attempts++;
        cout << "Invalid username or password. Attempts remaining: " << (maxAttempts - attempts) << endl;
        
        if (attempts < maxAttempts) {
            cout << "Press any key to try again...";
            getch();
            cout << endl;
        }
    }
    
    cout << "Too many failed attempts. Exiting program." << endl;
    return false;
}

// Function to register a new user (admin only)
void registerUser() {
    if (!currentUser || !currentUser->isAdmin) {
        cout << "Only administrators can register new users." << endl;
        cout << "Press any key to continue...";
        getch();
        return;
    }
    
    string username, password;
    int isAdmin;
    
    cout << "=== USER REGISTRATION ===" << endl;
    cout << "Enter new username: ";
    getline(cin, username);
    
    // Check if username already exists
    for (const auto& user : users) {
        if (user.username == username) {
            cout << "Username already exists. Registration failed." << endl;
            cout << "Press any key to continue...";
            getch();
            return;
        }
    }
    
    cout << "Enter password: ";
    password = getPassword();
    
    cout << "Is this an admin account? (1 for yes, 0 for no): ";
    isAdmin = getIntegerInput();
    
    // Create new user
    users.push_back(User(username, encryptPassword(password), isAdmin != 0));
    saveUsers();
    
    cout << "User registered successfully." << endl;
    cout << "Press any key to continue...";
    getch();
}

// Function to change password
void changePassword() {
    if (!currentUser) {
        cout << "You must be logged in to change your password." << endl;
        return;
    }
    
    string oldPassword, newPassword;
    
    cout << "=== CHANGE PASSWORD ===" << endl;
    cout << "Enter your current password: ";
    oldPassword = getPassword();
    
    if (encryptPassword(oldPassword) != currentUser->passwordHash) {
        cout << "Incorrect password. Operation canceled." << endl;
        cout << "Press any key to continue...";
        getch();
        return;
    }
    
    cout << "Enter new password: ";
    newPassword = getPassword();
    
    currentUser->passwordHash = encryptPassword(newPassword);
    saveUsers();
    
    cout << "Password changed successfully." << endl;
    cout << "Press any key to continue...";
    getch();
}

// Clear screen function for cross-platform compatibility
void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// Function to get current date
void getCurrentDate(int& year, int& month, int& day) {
    time_t now = time(0);
    tm* ltm = localtime(&now);
    
    year = 1900 + ltm->tm_year;
    month = 1 + ltm->tm_mon;
    day = ltm->tm_mday;
}

// Function to validate date
bool isValidDate(int year, int month, int day) {
    if (year < 2000 || year > 2100) return false;
    if (month < 1 || month > 12) return false;
    
    int daysInMonth[] = {0, 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    
    // Adjust for leap years
    if (year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))
        daysInMonth[2] = 29;
        
    if (day < 1 || day > daysInMonth[month]) return false;
    
    return true;
}

// Function to get integer input with validation
int getIntegerInput() {
    int value;
    while (!(cin >> value)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid input. Please enter a number: ";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    return value;
}

// Function to pause the program until a key is pressed
void pauseScreen() {
    cout << "Press any key to continue...\n";
    getch();
}

// Function to read transactions from a file
vector<Transaction> readFromFile(const string& filename) {
    vector<Transaction> transactions;
    ifstream file(filename);
    
    if (!file.is_open()) {
        return transactions;
    }
    
    string line;
    Transaction* currentTransaction = nullptr;
    
    while (getline(file, line)) {
        if (line.empty()) continue;
        
        // Check if this is a date line
        if (line.find("Year:") != string::npos) {
            int year, month, date;
            string username = "unknown";
            
            // Extract date info
            sscanf(line.c_str(), "Year:%d\tMonth:%d\tDate: %d", &year, &month, &date);
            
            // Check if username is included
            size_t userPos = line.find("User: ");
            if (userPos != string::npos) {
                username = line.substr(userPos + 6);
            }
            
            transactions.push_back(Transaction(year, month, date, username));
            currentTransaction = &transactions.back();
        }
        // Check if this is an item line
        else if (currentTransaction != nullptr && 
                 (line.find("Name:") != string::npos || line.find("Sold - Name:") != string::npos)) {
            bool isSold = line.find("Sold - ") != string::npos;
            string name;
            int price, quantity;
            
            // Extract data from line
            size_t nameStart = line.find("Name: ") + 6;
            size_t nameEnd = line.find("\tPrice:");
            name = line.substr(nameStart, nameEnd - nameStart);
            
            size_t priceStart = line.find("Price: ") + 7;
            size_t priceEnd = line.find("\tQuantity:");
            price = stoi(line.substr(priceStart, priceEnd - priceStart));
            
            size_t quantityStart = line.find("Quantity: ") + 10;
            quantity = stoi(line.substr(quantityStart));
            
            currentTransaction->items.push_back(Item(name, price, quantity, isSold));
        }
    }
    
    file.close();
    return transactions;
}

// Function to save transactions to a file
void saveToFile(const vector<Transaction>& transactions, const string& filename) {
    ofstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Error opening file for writing.\n";
        return;
    }
    
    for (const auto& transaction : transactions) {
        file << "\nYear:" << transaction.year 
             << "\tMonth:" << transaction.month 
             << "\tDate: " << transaction.date
             << "\tUser: " << transaction.username << "\n";
        
        for (const auto& item : transaction.items) {
            if (item.isSold) {
                file << "Sold - Name: " << item.name 
                     << "\tPrice: " << item.price 
                     << "\tQuantity: " << item.quantity << "\n";
            } else {
                file << "Name: " << item.name 
                     << "\tPrice: " << item.price 
                     << "\tQuantity: " << item.quantity << "\n";
            }
        }
    }
    
    file.close();
}

// Function to add items
void addItems(vector<Transaction>& transactions) {
    if (!currentUser) {
        cout << "You must be logged in to add items." << endl;
        return;
    }
    
    int year, month, date;
    
    // Get current date as default
    getCurrentDate(year, month, date);
    
    cout << "Enter date (Leave 0 to use current date: " 
          << month << "/" << date << "/" << year << ")\n";
    
    cout << "Year (0 for current): ";
    int inputYear = getIntegerInput();
    if (inputYear != 0) year = inputYear;
    
    cout << "Month (0 for current): ";
    int inputMonth = getIntegerInput();
    if (inputMonth != 0) month = inputMonth;
    
    cout << "Date (0 for current): ";
    int inputDate = getIntegerInput();
    if (inputDate != 0) date = inputDate;
    
    // Validate date
    if (!isValidDate(year, month, date)) {
        cout << "Invalid date. Using current date instead.\n";
        getCurrentDate(year, month, date);
    }
    
    cout << "How many items do you want to add: ";
    int num = getIntegerInput();
    
    if (num <= 0) {
        cout << "No items to add.\n";
        return;
    }
    
    // Create a new transaction
    Transaction newTransaction(year, month, date, currentUser->username);
    
    for (int i = 0; i < num; i++) {
        cout << "\nItem " << (i + 1) << ":\n";
        
        cout << "Name: ";
        string name;
        getline(cin, name);
        
        cout << "Price: ";
        int price = getIntegerInput();
        
        cout << "Quantity: ";
        int quantity = getIntegerInput();
        
        newTransaction.items.push_back(Item(name, price, quantity, false));
    }
    
    transactions.push_back(newTransaction);
    cout << "\n" << num << " items added successfully.\n";
}

// Function to view items
void viewItems(const vector<Transaction>& transactions) {
    clearScreen();
    
    if (!currentUser) {
        cout << "You must be logged in to view items." << endl;
        pauseScreen();
        return;
    }
    
    if (transactions.empty()) {
        cout << "No items in history.\n";
        pauseScreen();
        return;
    }
    
    cout << "============================= HISTORY =============================\n";
    
    // Calculate total inventory value and sold value
    int totalInventoryValue = 0;
    int totalSoldValue = 0;
    int totalInventoryItems = 0;
    int totalSoldItems = 0;
    
    for (const auto& transaction : transactions) {
        cout << "Date: " << transaction.month << "/" 
              << transaction.date << "/" << transaction.year
              << " (User: " << transaction.username << ")\n";
        cout << "----------------------------------------------------------------\n";
        cout << left << setw(25) << "Name" 
              << setw(10) << "Price" 
              << setw(10) << "Quantity" 
              << setw(15) << "Total Value" 
              << "Type\n";
        cout << "----------------------------------------------------------------\n";
        
        for (const auto& item : transaction.items) {
            int totalValue = item.price * item.quantity;
            
            cout << left << setw(25) << item.name 
                  << setw(10) << item.price 
                  << setw(10) << item.quantity 
                  << setw(15) << totalValue;
            
            if (item.isSold) {
                cout << "SOLD\n";
                totalSoldValue += totalValue;
                totalSoldItems += item.quantity;
            } else {
                cout << "ADDED\n";
                totalInventoryValue += totalValue;
                totalInventoryItems += item.quantity;
            }
        }
        cout << "----------------------------------------------------------------\n\n";
    }
    
    // Show summary
    cout << "========================= SUMMARY =========================\n";
    cout << "Total inventory items: " << totalInventoryItems << "\n";
    cout << "Total inventory value: $" << totalInventoryValue << "\n";
    cout << "Total sold items: " << totalSoldItems << "\n";
    cout << "Total sales value: $" << totalSoldValue << "\n";
    cout << "Profit/Loss: $" << (totalSoldValue - totalInventoryValue) << "\n";
    cout << "==========================================================\n";
    
    pauseScreen();
}

// Function to search items
void searchItems(const vector<Transaction>& transactions) {
    clearScreen();
    
    if (!currentUser) {
        cout << "You must be logged in to search items." << endl;
        pauseScreen();
        return;
    }
    
    if (transactions.empty()) {
        cout << "No items in history.\n";
        pauseScreen();
        return;
    }
    
    cout << "Search by name (case-insensitive): ";
    string searchTerm;
    getline(cin, searchTerm);
    
    // Convert search term to lowercase for case-insensitive comparison
    transform(searchTerm.begin(), searchTerm.end(), searchTerm.begin(), 
               [](unsigned char c) { return tolower(c); });
    
    cout << "\n================== SEARCH RESULTS ==================\n";
    
    bool found = false;
    for (const auto& transaction : transactions) {
        for (const auto& item : transaction.items) {
            // Convert item name to lowercase for comparison
            string itemNameLower = item.name;
            transform(itemNameLower.begin(), itemNameLower.end(), itemNameLower.begin(), 
                       [](unsigned char c) { return tolower(c); });
            
            if (itemNameLower.find(searchTerm) != string::npos) {
                if (!found) {
                    cout << left << setw(25) << "Name" 
                          << setw(10) << "Price" 
                          << setw(10) << "Quantity" 
                          << setw(15) << "Date" 
                          << setw(10) << "User"
                          << "Type\n";
                    cout << "-------------------------------------------------------\n";
                    found = true;
                }
                
                cout << left << setw(25) << item.name 
                      << setw(10) << item.price 
                      << setw(10) << item.quantity 
                      << setw(15) << (to_string(transaction.month) + "/" + 
                                      to_string(transaction.date) + "/" + 
                                      to_string(transaction.year))
                      << setw(10) << transaction.username;
                
                cout << (item.isSold ? "SOLD\n" : "ADDED\n");
            }
        }
    }
    
    if (!found) {
        cout << "No items found matching '" << searchTerm << "'.\n";
    }
    
    pauseScreen();
}

// Function to sell items
void sellItems(vector<Transaction>& transactions) {
    if (!currentUser) {
        cout << "You must be logged in to record sales." << endl;
        pauseScreen();
        return;
    }
    
    int year, month, date;
    
    // Get current date as default
    getCurrentDate(year, month, date);
    
    cout << "Enter date (Leave 0 to use current date: " 
          << month << "/" << date << "/" << year << ")\n";
    
    cout << "Year (0 for current): ";
    int inputYear = getIntegerInput();
    if (inputYear != 0) year = inputYear;
    
    cout << "Month (0 for current): ";
    int inputMonth = getIntegerInput();
    if (inputMonth != 0) month = inputMonth;
    
    cout << "Date (0 for current): ";
    int inputDate = getIntegerInput();
    if (inputDate != 0) date = inputDate;
    
    // Validate date
    if (!isValidDate(year, month, date)) {
        cout << "Invalid date. Using current date instead.\n";
        getCurrentDate(year, month, date);
    }
    
    cout << "How many items did you sell: ";
    int num = getIntegerInput();
    
    if (num <= 0) {
        cout << "No items to add.\n";
        return;
    }
    
    // Create a new transaction
    Transaction newTransaction(year, month, date, currentUser->username);
    
    for (int i = 0; i < num; i++) {
        cout << "\nItem " << (i + 1) << ":\n";
        
        cout << "Name: ";
        string name;
        getline(cin, name);
        
        cout << "Price: ";
        int price = getIntegerInput();
        
        cout << "Quantity: ";
        int quantity = getIntegerInput();
        
        newTransaction.items.push_back(Item(name, price, quantity, true));
    }
    
    transactions.push_back(newTransaction);
    cout << "\n" << num << " sold items recorded successfully.\n";
}

// Function to display user information (admin only)
void manageUsers() {
    if (!currentUser || !currentUser->isAdmin) {
        cout << "Only administrators can manage users." << endl;
        pauseScreen();
        return;
    }
    
    clearScreen();
    cout << "=================== USER MANAGEMENT ===================\n";
    cout << "1. View All Users\n";
    cout << "2. Register New User\n";
    cout << "3. Delete User\n";
    cout << "4. Return to Main Menu\n";
    cout << "======================================================\n";
    cout << "Enter choice: ";
    
    int choice = getIntegerInput();
    
    switch (choice) {
        case 1: {
            clearScreen();
            cout << "=========== REGISTERED USERS ===========\n";
            cout << left << setw(20) << "Username" << "Role\n";
            cout << "---------------------------------------\n";
            
            for (const auto& user : users) {
                cout << left << setw(20) << user.username 
                      << (user.isAdmin ? "Administrator" : "Regular User") << "\n";
            }
            pauseScreen();
            break;
        }
        case 2:
            registerUser();
            break;
        case 3: {
            string username;
            cout << "Enter username to delete: ";
            getline(cin, username);
            
            // Can't delete yourself
            if (username == currentUser->username) {
                cout << "You cannot delete your own account." << endl;
                pauseScreen();
                break;
            }
            
            auto it = find_if(users.begin(), users.end(), 
                               [&username](const User& user) { return user.username == username; });
            
            if (it != users.end()) {
                users.erase(it);
                saveUsers();
                cout << "User '" << username << "' deleted successfully." << endl;
            } else {
                cout << "User not found." << endl;
            }
            
            pauseScreen();
            break;
        }
        case 4:
            return;
        default:
            cout << "Invalid choice. Please try again.\n";
            pauseScreen();
    }
}

// Main program
int main() {
    const string filename = "inventory_history.txt";
    
    // Load users from file
    loadUsers();
    
    clearScreen();
    cout << "=============================================\n";
    cout << "         INVENTORY MANAGEMENT SYSTEM        \n";
    cout << "=============================================\n";
    
    // Login
    if (!login()) {
        return 1;
    }
    
    vector<Transaction> transactions = readFromFile(filename);
    
    while (true) {
        clearScreen();
        cout << "=============================================\n";
        cout << "         INVENTORY MANAGEMENT SYSTEM        \n";
        cout << "=============================================\n";
        cout << "Logged in as: " << currentUser->username 
              << " (" << (currentUser->isAdmin ? "Admin" : "User") << ")\n";
        cout << "=============================================\n";
        cout << "1. Add Item(s)\n";
        cout << "2. View All Items\n";
        cout << "3. Sell Item(s)\n";
        cout << "4. Search Items\n";
        cout << "5. Change Password\n";
        
        if (currentUser->isAdmin) {
            cout << "6. User Management\n";
            cout << "7. Exit\n";
        } else {
            cout << "6. Exit\n";
        }
        
        cout << "=============================================\n";
        cout << "Enter choice: ";
        
        int choice = getIntegerInput();
        
        switch (choice) {
            case 1:
                addItems(transactions);
                saveToFile(transactions, filename);
                break;
                
            case 2:
                viewItems(transactions);
                break;
                
            case 3:
                sellItems(transactions);
                saveToFile(transactions, filename);
                break;
                
            case 4:
                searchItems(transactions);
                break;
                
            case 5:
                changePassword();
                break;
                
            case 6:
                if (currentUser->isAdmin) {
                    manageUsers();
                } else {
                    cout << "Exiting program. Goodbye!\n";
                    return 0;
                }
                break;
                
            case 7:
                if (currentUser->isAdmin) {
                    cout << "Exiting program. Goodbye!\n";
                    return 0;
                }
                // Fall through if not admin
                
            default:
                cout << "Invalid choice. Please try again.\n";
                pauseScreen();
        }
    }
    
    return 0;
}