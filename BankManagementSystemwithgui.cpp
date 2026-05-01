// GUI (colored console) entry point.
// Compile: g++ BankManagementSystemwithgui.cpp Banksystem.cpp Accounts.cpp Entities.cpp -o bank_gui
// Do NOT compile together with main.cpp (both define main()).

#include "Banksystem.h"
#include <iostream>
#include <sstream>
#include <limits>

#ifdef _WIN32
  #include <windows.h>
#else
  #include <unistd.h>
  #include <sys/ioctl.h>
#endif

using namespace std;

// ── Console helpers ──────────────────────────────────────────────────────────

static int consoleWidth() {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    if (GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi))
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return 80;
#else
    struct winsize w{};
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) == 0 && w.ws_col > 0)
        return w.ws_col;
    return 80;
#endif
}

static void printCentered(const string& text) {
    int padding = (consoleWidth() - (int)text.length()) / 2;
    if (padding < 0) padding = 0;
    cout << string(padding, ' ') << text << "\n";
}

static void setColor(int color) {
#ifdef _WIN32
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
#else
    switch (color) {
        case 10: cout << "\033[32m"; break; // green
        case 11: cout << "\033[36m"; break; // cyan
        case 12: cout << "\033[31m"; break; // red
        case 14: cout << "\033[33m"; break; // yellow
        default: cout << "\033[0m";  break; // reset
    }
#endif
}

static void resetColor() { setColor(7); }

static void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

static void sleepMs(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
}

static void showLoadingBar() {
    clearScreen();
    setColor(11);
    printCentered("Loading Bank Management System...");

    const int barWidth = 50;
    int pad = (consoleWidth() - barWidth - 2) / 2;
    if (pad < 0) pad = 0;

    cout << string(pad, ' ') << "[";
    setColor(10);
    for (int i = 0; i < barWidth; ++i) {
        cout << '#' << flush;
        sleepMs(20);
    }
    resetColor();
    cout << "]\n";
    sleepMs(300);
}

static void printMenuItem(const string& item) {
    setColor(14);
    printCentered(item);
    resetColor();
}

static void printHeader(const string& title) {
    setColor(11);
    printCentered(title);
    resetColor();
}

static void printError(const string& msg) {
    setColor(12);
    cout << msg << "\n";
    resetColor();
}

static void printSuccess(const string& msg) {
    setColor(10);
    cout << msg << "\n";
    resetColor();
}

// ── Menu helpers ─────────────────────────────────────────────────────────────

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
    cout << "\nPress Enter to continue...";
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

// ── Role-specific menus ──────────────────────────────────────────────────────

// Admin has no banking powers (separation of duties).
// Read-only views are kept for audit purposes.
static void adminMenu(BankSystem& sys) {
    int choice = 0;
    do {
        clearScreen();
        printHeader("======== ADMIN MENU ========");
        printMenuItem(" 1.  Add Employee (with login)");
        printMenuItem(" 2.  View Employees");
        printMenuItem(" 3.  View Account (audit, read-only)");
        printMenuItem(" 4.  View Transactions (audit, read-only)");
        printMenuItem(" 5.  Add Customer Contact (User)");
        printMenuItem(" 6.  View Customer Contacts");
        printMenuItem(" 7.  Add Login Credential");
        printMenuItem(" 8.  Remove Login Credential");
        printMenuItem(" 9.  View Login Credentials");
        printMenuItem("10.  Logout & Exit");
        setColor(10);
        printCentered("Enter your choice: ");
        resetColor();
        choice = readChoice();
        cout << "\n";

        switch (choice) {
            case 1:  sys.addEmployee();         break;
            case 2:  sys.viewEmployees();       break;
            case 3:  sys.viewAccount();         break;
            case 4:  sys.showTransactions();    break;
            case 5:  sys.addUser();             break;
            case 6:  sys.viewUsers();           break;
            case 7:  sys.addCredential();       break;
            case 8:  sys.removeCredential();    break;
            case 9:  sys.viewCredentials();     break;
            case 10: printSuccess("Goodbye!");  break;
            default: printError("Invalid choice.");
        }
        if (choice != 10) pauseHere();
    } while (choice != 10);
}

static void employeeMenu(BankSystem& sys) {
    int choice = 0;
    do {
        clearScreen();
        printHeader("======== EMPLOYEE MENU ========");
        printMenuItem(" 1.  Create Account (with customer login)");
        printMenuItem(" 2.  Deposit");
        printMenuItem(" 3.  Withdraw");
        printMenuItem(" 4.  View Account");
        printMenuItem(" 5.  View Transactions");
        printMenuItem(" 6.  Calculate Interest (Savings)");
        printMenuItem(" 7.  Add Customer Contact");
        printMenuItem(" 8.  View Customer Contacts");
        printMenuItem(" 9.  Logout & Exit");
        setColor(10);
        printCentered("Enter your choice: ");
        resetColor();
        choice = readChoice();
        cout << "\n";

        switch (choice) {
            case 1: sys.createAccount();        break;
            case 2: sys.depositAmount();         break;
            case 3: sys.withdrawAmount();        break;
            case 4: sys.viewAccount();           break;
            case 5: sys.showTransactions();      break;
            case 6: sys.addInterestToSavings();  break;
            case 7: sys.addUser();               break;
            case 8: sys.viewUsers();             break;
            case 9: printSuccess("Goodbye!");    break;
            default: printError("Invalid choice.");
        }
        if (choice != 9) pauseHere();
    } while (choice != 9);
}

static void customerMenu(BankSystem& sys) {
    int choice = 0;
    do {
        clearScreen();
        printHeader("======== CUSTOMER PORTAL ========");
        printMenuItem(" 1.  View My Account");
        printMenuItem(" 2.  View My Transactions");
        printMenuItem(" 3.  Deposit");
        printMenuItem(" 4.  Withdraw");
        printMenuItem(" 5.  Logout & Exit");
        setColor(10);
        printCentered("Enter your choice: ");
        resetColor();
        choice = readChoice();
        cout << "\n";

        switch (choice) {
            case 1: sys.customerViewAccount();      break;
            case 2: sys.customerShowTransactions(); break;
            case 3: sys.customerDeposit();          break;
            case 4: sys.customerWithdraw();         break;
            case 5: printSuccess("Goodbye!");       break;
            default: printError("Invalid choice.");
        }
        if (choice != 5) pauseHere();
    } while (choice != 5);
}

// ── Main ─────────────────────────────────────────────────────────────────────

int main() {
    showLoadingBar();

    BankSystem system;
    if (!system.login()) {
        printError("Access denied.");
        return 1;
    }

    const string& role = system.getSession().role;
    if      (role == "admin")    adminMenu(system);
    else if (role == "employee") employeeMenu(system);
    else if (role == "customer") customerMenu(system);
    else {
        printError("Unknown role: " + role);
        return 1;
    }
    return 0;
}
