#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

void clearScreen() {
    system("cls || clear");
}

void printHeader(const string& title) {
    clearScreen();
    cout << "===========================================\n";
    cout << "|| \t\t" << title << " \t\t  ||\n";
    cout << "===========================================\n";
}

void printMenu() {
    clearScreen();
    cout << "===========================================\n";
    cout << "|| \t\t MAIN MENU \t\t  ||\n";
    cout << "===========================================\n";
    cout << "|| 1. Add Item                           ||\n";
    cout << "|| 2. View Items                         ||\n";
    cout << "|| 3. Sell Item                          ||\n";
    cout << "|| 4. Clear History                      ||\n";
    cout << "|| 5. Exit                               ||\n";
    cout << "===========================================\n";
    cout << "Enter choice: ";
}

void printFooter() {
    cout << "===========================================\n";
}

int main() {
    int choice;
    do {
        printMenu();
        cin >> choice;
        cin.ignore();  // Clear the newline character

        if (choice == 1) {
            fstream history("history.bin", ios::in | ios::out | ios::app);
            if (!history) {
                cout << "Error opening file" << endl;
                return 1;
            }
            
            int num, price, quantity, month, year, date;
            string name;
            
            do {
                printHeader("Adding Items");
                
                cout << "Year: ";
                cin >> year;
                cout << "Month: ";
                cin >> month;
                cout << "Date: ";
                cin >> date;
                if ((month < 1 || month > 12) || (date < 1 || date > 31)) {
                    cout << "Invalid Month and Date format. Please reenter.\n";
                }
            } while ((month < 1 || month > 12) || (date < 1 || date > 31));
            
            cout << "How many items do you want to add: ";
            cin >> num;
            cin.ignore();
            
            history << "\nYear: " << year << "\tMonth: " << month << "\tDate: " << date << "\n";
            
            for (int i = 0; i < num; i++) {
                cout << "\n\nName: ";
                getline(cin, name);
                
                cout << "Price: ";
                cin >> price;
                cin.ignore();
                
                cout << "Quantity: ";
                cin >> quantity;
                cin.ignore();
                
                history << "Name: " << name << "\tPrice: " << price << "\tQuantity: " << quantity << "\n";
            }
            history.close();
            printFooter();
        } 
        else if (choice == 2) {
            char viewAgain;
            do {
                fstream history("history.bin", ios::in);
                if (!history) {
                    cout << "Error opening file" << endl;
                    return 1;
                }
                
                history.clear();
                history.seekg(0, ios::beg);
                
                string line;
                printHeader("HISTORY");
                while (getline(history, line)) {
                    cout << line << endl;
                }
                
                cout << "Press Enter to continue...";
                cin.get();
                history.close();

                cout << "Do you want to view the history again? (y/n): ";
                cin >> viewAgain;
                cin.ignore();  // Clear the newline character
            } while (viewAgain == 'y' || viewAgain == 'Y');
            printFooter();
        } 
        else if (choice == 3) {
            fstream history("history.bin", ios::in | ios::out | ios::app);
            if (!history) {
                cout << "Error opening file" << endl;
                return 1;
            }
            
            int num, price, quantity, month, year, date;
            string name;
            
            printHeader("Selling Items");
            
            cout << "Year: ";
            cin >> year;
            cout << "Month: ";
            cin >> month;
            cout << "Date: ";
            cin >> date;
            cout << "How many items did you sell: ";
            cin >> num;
            cin.ignore();
            
            history << "\nYear: " << year << "\tMonth: " << month << "\tDate: " << date << "\n";
            
            for (int i = 0; i < num; i++) {
                cout << "\n\nName: ";
                getline(cin, name);
                
                cout << "Price: ";
                cin >> price;
                cin.ignore();
                
                cout << "Quantity: ";
                cin >> quantity;
                cin.ignore();
                
                history << "Sold - Name: " << name << "\tPrice: " << price << "\tQuantity: " << quantity << "\n";
            }
            history.close();
            printFooter();
        }
        else if (choice == 4) {
            ofstream history("history.bin", ios::trunc);
            if (!history) {
                cout << "Error clearing history" << endl;
            } else {
                cout << "History cleared successfully." << endl;
            }
            history.close();
            printFooter();
        }
    } while (choice != 5);
    cout << "Exiting.....";
    
    return 0;
}
