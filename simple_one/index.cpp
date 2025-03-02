#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

int main() {
    int choice;
    do {
        cout << "\n1. Add Item\n2. View Items\n3. Sell Item\n4. Clear History\n5. Exit\n";
        cout << "Enter choice: ";
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
            
            cout << "Year: ";
            cin >> year;
            cout << "Month: ";
            cin >> month;
            cout << "Date: ";
            cin >> date;
            cout << "How many items do you want to add: ";
            cin >> num;
            cin.ignore();
            
            history << "\nYear: " << year << "\tMonth: " << month << "\tDate: " << date << "\n";
            
            for (int i = 0; i < num; i++) {
                cout << "Name: ";
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
        } 
        else if (choice == 2) {
            fstream history("history.bin", ios::in);
            if (!history) {
                cout << "Error opening file" << endl;
                return 1;
            }
            
            history.clear();
            history.seekg(0, ios::beg);
            
            string line;
            system("cls || clear");
            cout << "            HISTORY\n";
            while (getline(history, line)) {
                cout << line << endl;
            }
            
            cout << "Press Enter to continue...";
            cin.get();
            history.close();
        } 
        else if (choice == 3) {
            fstream history("history.bin", ios::in | ios::out | ios::app);
            if (!history) {
                cout << "Error opening file" << endl;
                return 1;
            }
            
            int num, price, quantity, month, year, date;
            string name;
            
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
                cout << "Name: ";
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
        }
        else if (choice == 4) {
            ofstream history("history.bin", ios::trunc);
            if (!history) {
                cout << "Error clearing history" << endl;
            } else {
                cout << "History cleared successfully." << endl;
            }
            history.close();
        }
    } while (choice != 5);
    
    return 0;
}
