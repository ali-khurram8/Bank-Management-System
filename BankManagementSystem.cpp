// LEGACY single-file version (kept for reference).
// All issues have been fixed in the split version:
//   main.cpp + Banksystem.cpp + Accounts.cpp + Entities.cpp
// Use that version for any further development.

#include <iostream>
#include <fstream>
#include <string>
#include <limits>
using namespace std;

class Account {
protected:
    string accountNumber;
    string accountHolderName;
    double balance;

public:
    Account(string accNum = "", string name = "", double initialBalance = 0.0)
        : accountNumber(accNum), accountHolderName(name), balance(initialBalance) {}

    virtual void deposit(double amount) {
        balance += amount;
        cout << "Deposit Successful. New Balance: " << balance << endl;
    }

    virtual void withdraw(double amount) {
        if (balance >= amount) {
            balance -= amount;
            cout << "Withdrawal Successful. New Balance: " << balance << endl;
        } else {
            cout << "Insufficient Balance." << endl;
        }
    }

    virtual void displayAccountInfo() const {
        cout << "Account Number: " << accountNumber << endl;
        cout << "Account Holder: " << accountHolderName << endl;
        cout << "Balance: " << balance << endl;
    }

    string getAccountNumber() const { return accountNumber; }
    double getBalance() const { return balance; }
    virtual void calculateInterest() {}
};

class SavingsAccount : public Account {
private:
    double interestRate;

public:
    SavingsAccount(string accNum = "", string name = "", double initialBalance = 0.0, double rate = 0.03)
        : Account(accNum, name, initialBalance), interestRate(rate) {}

    void calculateInterest() override {
        double interest = balance * interestRate;
        balance += interest;
        cout << "Interest added: " << interest << endl;
        cout << "New Balance: " << balance << endl;
    }
};

class CurrentAccount : public Account {
private:
    double overdraftLimit;

public:
    CurrentAccount(string accNum = "", string name = "", double initialBalance = 0.0, double overdraft = 5000.0)
        : Account(accNum, name, initialBalance), overdraftLimit(overdraft) {}

    void withdraw(double amount) override {
        if (balance + overdraftLimit >= amount) {
            balance -= amount;
            cout << "Withdrawal Successful. New Balance: " << balance << endl;
        } else {
            cout << "Withdrawal exceeds overdraft limit." << endl;
        }
    }

    void displayAccountInfo() const override {
        Account::displayAccountInfo();
        cout << "Overdraft Limit: " << overdraftLimit << endl;
    }
};

class Transaction {
public:
    string type;
    double amount;
    Transaction(string t = "", double a = 0.0) : type(t), amount(a) {}
};

class Employee {
public:
    string empID;
    string name;
    string position;
    Employee(string id = "", string n = "", string pos = "") : empID(id), name(n), position(pos) {}
};

class User {
public:
    string userID;
    string name;
    string phone;
    User(string id = "", string n = "", string ph = "") : userID(id), name(n), phone(ph) {}
};

class BankSystem {
private:
    Account* accounts[100]{};
    int accountCount;

    Transaction transactions[1000]{};
    int transactionCount;

    Employee employees[100]{};
    int employeeCount;

    User users[100]{};
    int userCount;

public:
    BankSystem() : accountCount(0), transactionCount(0), employeeCount(0), userCount(0) {
        loadEmployees();
        loadUsers();
    }

    ~BankSystem() {
        for (int i = 0; i < accountCount; i++) delete accounts[i];
    }

    void createAccount() {
        string type, accNum, name;
        double initialBalance;
        cout << "Enter Account Type (savings / current): ";
        cin >> type;
        cout << "Enter Account Number: ";
        cin >> accNum;
        cout << "Enter Account Holder Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Initial Balance: ";
        cin >> initialBalance;

        if (type == "savings") {
            accounts[accountCount++] = new SavingsAccount(accNum, name, initialBalance);
            cout << "Savings Account Created Successfully!" << endl;
        } else if (type == "current") {
            accounts[accountCount++] = new CurrentAccount(accNum, name, initialBalance);
            cout << "Current Account Created Successfully!" << endl;
        } else {
            cout << "Invalid Account Type!" << endl;
        }
    }

    Account* findAccount(const string& accNum) {
        for (int i = 0; i < accountCount; i++) {
            if (accounts[i]->getAccountNumber() == accNum) return accounts[i];
        }
        return nullptr;
    }

    void depositAmount() {
        string accNum; double amount;
        cout << "Enter Account Number: ";
        cin >> accNum;
        Account* acc = findAccount(accNum);
        if (acc) {
            cout << "Enter Amount to Deposit: ";
            cin >> amount;
            acc->deposit(amount);
            transactions[transactionCount++] = Transaction("Deposit", amount);
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void withdrawAmount() {
        string accNum; double amount;
        cout << "Enter Account Number: ";
        cin >> accNum;
        Account* acc = findAccount(accNum);
        if (acc) {
            cout << "Enter Amount to Withdraw: ";
            cin >> amount;
            acc->withdraw(amount);
            transactions[transactionCount++] = Transaction("Withdraw", amount);
        } else {
            cout << "Account not found!" << endl;
        }
    }

    void viewAccount() {
        string accNum;
        cout << "Enter Account Number: ";
        cin >> accNum;
        Account* acc = findAccount(accNum);
        if (acc) acc->displayAccountInfo();
        else cout << "Account not found!" << endl;
    }

    void showTransactions() {
        cout << "Transaction History: " << endl;
        for (int i = 0; i < transactionCount; i++) {
            cout << transactions[i].type << ": " << transactions[i].amount << endl;
        }
    }

    void addInterestToSavings() {
        string accNum;
        cout << "Enter Account Number: ";
        cin >> accNum;
        Account* acc = findAccount(accNum);
        if (acc) acc->calculateInterest();
        else cout << "Account not found!" << endl;
    }

    void addEmployee() {
        string id, name, position;
        cout << "Enter Employee ID: ";
        cin >> id;
        cout << "Enter Employee Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Employee Position: ";
        getline(cin, position);

        employees[employeeCount++] = Employee(id, name, position);

        ofstream outFile("employees.txt", ios::app);
        if (outFile.is_open()) {
            outFile << id << ' ' << name << ' ' << position << '\n';
            outFile.close();
        }
        cout << "Employee Added and Saved!" << endl;
    }

    void viewEmployees() {
        cout << "--- Employee List ---" << endl;
        for (int i = 0; i < employeeCount; i++) {
            cout << "ID: " << employees[i].empID << endl;
            cout << "Name: " << employees[i].name << endl;
            cout << "Position: " << employees[i].position << endl;
        }
    }

    void loadEmployees() {
        ifstream inFile("employees.txt");
        if (inFile.is_open()) {
            while (inFile >> employees[employeeCount].empID) {
                inFile >> employees[employeeCount].name;
                getline(inFile, employees[employeeCount].position);
                employeeCount++;
                if (employeeCount >= 100) break;
            }
            inFile.close();
        }
    }

    void addUser() {
        string id, name, phone;
        cout << "Enter User ID: ";
        cin >> id;
        cout << "Enter User Name: ";
        cin.ignore();
        getline(cin, name);
        cout << "Enter Phone Number: ";
        getline(cin, phone);

        users[userCount++] = User(id, name, phone);

        ofstream outFile("users.txt", ios::app);
        if (outFile.is_open()) {
            outFile << id << ' ' << name << ' ' << phone << '\n';
            outFile.close();
        }
        cout << "User Added and Saved!" << endl;
    }

    void viewUsers() {
        cout << "--- User List ---" << endl;
        for (int i = 0; i < userCount; i++) {
            cout << "ID: " << users[i].userID << endl;
            cout << "Name: " << users[i].name << endl;
            cout << "Phone: " << users[i].phone << endl;
        }
    }

    void loadUsers() {
        ifstream inFile("users.txt");
        if (inFile.is_open()) {
            while (inFile >> users[userCount].userID) {
                inFile >> users[userCount].name;
                getline(inFile, users[userCount].phone);
                userCount++;
                if (userCount >= 100) break;
            }
            inFile.close();
        }
    }
};

int main() {
    BankSystem system;
    int choice;

    do {
        cout << "\n-------- BANK MANAGEMENT SYSTEM --------" << endl;
        cout << "1. Create Account" << endl;
        cout << "2. Deposit Money" << endl;
        cout << "3. Withdraw Money" << endl;
        cout << "4. View Account Info" << endl;
        cout << "5. View Transaction History" << endl;
        cout << "6. Calculate Interest (Savings Account)" << endl;
        cout << "7. Add Employee" << endl;
        cout << "8. View Employees" << endl;
        cout << "9. Add User" << endl;
        cout << "10. View Users" << endl;
        cout << "11. Exit" << endl;
        cout << "Enter your choice: ";
        cin >> choice;

        switch (choice) {
            case 1: system.createAccount(); break;
            case 2: system.depositAmount(); break;
            case 3: system.withdrawAmount(); break;
            case 4: system.viewAccount(); break;
            case 5: system.showTransactions(); break;
            case 6: system.addInterestToSavings(); break;
            case 7: system.addEmployee(); break;
            case 8: system.viewEmployees(); break;
            case 9: system.addUser(); break;
            case 10: system.viewUsers(); break;
            case 11:
                cout << "Exiting the system. Thank you!" << endl;
                break;
            default:
                cout << "Invalid choice. Try again." << endl;
        }

        cout << "Press Enter to continue...";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();

    } while (choice != 11);

    return 0;
}
