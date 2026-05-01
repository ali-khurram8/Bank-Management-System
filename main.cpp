// Plain console entry point.
// Compile: g++ main.cpp Banksystem.cpp Accounts.cpp Entities.cpp -o bank
#include "Banksystem.h"
#include <iostream>
#include <limits>
using namespace std;

static int readChoice() {
    int choice;
    if (!(cin >> choice)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return -1;
    }
    return choice;
}

static void pauseHere() {
    cout << "Press Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// Admin has no banking powers (separation of duties).
// Read-only views are kept for audit purposes.
static void adminMenu(BankSystem& sys) {
    int choice = 0;
    do {
        cout << "\n-------- ADMIN MENU --------\n";
        cout << " 1.  Add Employee (with login)\n";
        cout << " 2.  View Employees\n";
        cout << " 3.  View Account (audit, read-only)\n";
        cout << " 4.  View Transactions (audit, read-only)\n";
        cout << " 5.  Add Customer Contact (User)\n";
        cout << " 6.  View Customer Contacts\n";
        cout << " 7.  Add Login Credential\n";
        cout << " 8.  Remove Login Credential\n";
        cout << " 9.  View Login Credentials\n";
        cout << "10.  Logout & Exit\n";
        cout << "Enter choice: ";
        choice = readChoice();

        switch (choice) {
            case 1:  sys.addEmployee();      break;
            case 2:  sys.viewEmployees();    break;
            case 3:  sys.viewAccount();      break;
            case 4:  sys.showTransactions(); break;
            case 5:  sys.addUser();          break;
            case 6:  sys.viewUsers();        break;
            case 7:  sys.addCredential();    break;
            case 8:  sys.removeCredential(); break;
            case 9:  sys.viewCredentials();  break;
            case 10: cout << "Goodbye!\n";   break;
            default: cout << "Invalid choice.\n";
        }
        if (choice != 10) pauseHere();
    } while (choice != 10);
}

static void employeeMenu(BankSystem& sys) {
    int choice = 0;
    do {
        cout << "\n-------- EMPLOYEE MENU --------\n";
        cout << " 1.  Create Account (with customer login)\n";
        cout << " 2.  Deposit\n";
        cout << " 3.  Withdraw\n";
        cout << " 4.  View Account\n";
        cout << " 5.  View Transactions\n";
        cout << " 6.  Calculate Interest (Savings)\n";
        cout << " 7.  Add Customer Contact\n";
        cout << " 8.  View Customer Contacts\n";
        cout << " 9.  Logout & Exit\n";
        cout << "Enter choice: ";
        choice = readChoice();

        switch (choice) {
            case 1: sys.createAccount();        break;
            case 2: sys.depositAmount();         break;
            case 3: sys.withdrawAmount();        break;
            case 4: sys.viewAccount();           break;
            case 5: sys.showTransactions();      break;
            case 6: sys.addInterestToSavings();  break;
            case 7: sys.addUser();               break;
            case 8: sys.viewUsers();             break;
            case 9: cout << "Goodbye!\n";        break;
            default: cout << "Invalid choice.\n";
        }
        if (choice != 9) pauseHere();
    } while (choice != 9);
}

static void customerMenu(BankSystem& sys) {
    int choice = 0;
    do {
        cout << "\n-------- CUSTOMER PORTAL --------\n";
        cout << " 1.  View My Account\n";
        cout << " 2.  View My Transactions\n";
        cout << " 3.  Deposit\n";
        cout << " 4.  Withdraw\n";
        cout << " 5.  Logout & Exit\n";
        cout << "Enter choice: ";
        choice = readChoice();

        switch (choice) {
            case 1: sys.customerViewAccount();      break;
            case 2: sys.customerShowTransactions(); break;
            case 3: sys.customerDeposit();          break;
            case 4: sys.customerWithdraw();         break;
            case 5: cout << "Goodbye!\n";           break;
            default: cout << "Invalid choice.\n";
        }
        if (choice != 5) pauseHere();
    } while (choice != 5);
}

int main() {
    BankSystem system;
    if (!system.login()) return 1;

    const string& role = system.getSession().role;
    if      (role == "admin")    adminMenu(system);
    else if (role == "employee") employeeMenu(system);
    else if (role == "customer") customerMenu(system);
    else {
        cout << "Unknown role: " << role << ". Exiting.\n";
        return 1;
    }
    return 0;
}
