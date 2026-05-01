#include "Banksystem.h"
#include <iostream>
using namespace std;

// ── Lifecycle ────────────────────────────────────────────────────────────────

BankSystem::BankSystem() {
    loadAccounts();
    loadTransactions();   // must come after loadAccounts so pointers exist
    loadEmployees();
    loadUsers();
}

BankSystem::~BankSystem() {
    saveAccounts();
    saveTransactions();
    for (Account* acc : accounts) delete acc;
}

// ── Login ────────────────────────────────────────────────────────────────────
// auth.txt format: username|password|role
//   role: "admin" | "employee" | "customer:ACC_NUM"

bool BankSystem::login() {
    const string AUTH_FILE = "auth.txt";

    // Seed default credentials if file is absent
    {
        ifstream check(AUTH_FILE);
        if (!check.is_open()) {
            ofstream init(AUTH_FILE);
            init << "admin|1234|admin\n";
            init << "teller|0000|employee\n";
        }
    }

    int attempts = 3;
    while (attempts > 0) {
        cout << "\n======= BANK MANAGEMENT SYSTEM =======\n";
        cout << "Username: ";
        string username; cin >> username;
        cout << "Password: ";
        string password; cin >> password;

        ifstream file(AUTH_FILE);
        string line;
        while (getline(file, line)) {
            if (line.empty()) continue;
            istringstream ss(line);
            string u, p, role;
            getline(ss, u, '|');
            getline(ss, p, '|');
            getline(ss, role);

            if (u != username || p != password) continue;

            // Backward compatibility: old 2-field rows are treated as admin
            if (role.empty()) role = "admin";

            session.authenticated = true;
            session.username      = u;

            size_t colon = role.find(':');
            if (colon != string::npos) {
                session.role          = role.substr(0, colon);
                session.accountNumber = role.substr(colon + 1);
            } else {
                session.role          = role;
                session.accountNumber = "";
            }

            cout << "Login successful. Welcome, " << username
                 << " (" << session.role << ")!\n";
            return true;
        }

        --attempts;
        if (attempts > 0)
            cout << "Invalid credentials. " << attempts << " attempt(s) remaining.\n";
        else
            cout << "Too many failed attempts. Access denied.\n";
    }
    return false;
}

// ── Validation helpers ────────────────────────────────────────────────────────

bool BankSystem::accountExists(const string& accNum) const {
    for (const Account* acc : accounts)
        if (acc->getAccountNumber() == accNum) return true;
    return false;
}

bool BankSystem::validateAmount(double amount) const {
    if (amount <= 0) {
        cout << "Amount must be greater than zero.\n";
        return false;
    }
    return true;
}

// ── Credential helpers ────────────────────────────────────────────────────────

bool BankSystem::credentialExists(const string& username) const {
    ifstream in("auth.txt");
    if (!in.is_open()) return false;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string u;
        getline(ss, u, '|');
        if (u == username) return true;
    }
    return false;
}

void BankSystem::appendCredential(const string& username,
                                  const string& password,
                                  const string& role) {
    ofstream out("auth.txt", ios::app);
    out << username << "|" << password << "|" << role << "\n";
}

// ── Account operations ────────────────────────────────────────────────────────

void BankSystem::createAccount() {
    string type, accNum, name;
    double initialBalance;
    string custUser, custPass;

    cout << "Account Type (savings / current): ";
    cin >> type;
    if (type != "savings" && type != "current") {
        cout << "Invalid type. Must be 'savings' or 'current'.\n";
        return;
    }

    cout << "Account Number: ";
    cin >> accNum;
    if (accountExists(accNum)) {
        cout << "Account number already exists.\n";
        return;
    }

    cout << "Account Holder Name: ";
    cin.ignore();
    getline(cin, name);
    if (name.empty()) { cout << "Name cannot be empty.\n"; return; }

    cout << "Initial Balance: ";
    if (!(cin >> initialBalance) || initialBalance < 0) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cout << "Invalid balance. Must be zero or greater.\n";
        return;
    }

    cout << "Customer Username (for online banking): ";
    cin >> custUser;
    if (credentialExists(custUser)) {
        cout << "Username already taken.\n";
        return;
    }

    cout << "Customer Password: ";
    cin >> custPass;
    if (custPass.empty()) {
        cout << "Password cannot be empty.\n";
        return;
    }

    if (type == "savings")
        accounts.push_back(new SavingsAccount(accNum, name, initialBalance));
    else
        accounts.push_back(new CurrentAccount(accNum, name, initialBalance));

    saveAccounts();
    appendCredential(custUser, custPass, "customer:" + accNum);

    cout << (type == "savings" ? "Savings" : "Current")
         << " account created successfully.\n";
    cout << "Customer can now log in as: " << custUser << "\n";
}

Account* BankSystem::findAccount(const string& accNum) {
    for (Account* acc : accounts)
        if (acc->getAccountNumber() == accNum) return acc;
    return nullptr;
}

void BankSystem::depositAmount() {
    string accNum; double amount;
    cout << "Account Number: ";
    cin >> accNum;
    Account* acc = findAccount(accNum);
    if (!acc) { cout << "Account not found.\n"; return; }

    cout << "Amount to Deposit: ";
    if (!(cin >> amount) || !validateAmount(amount)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    acc->deposit(amount);
    saveAccounts();
    saveTransactions();
}

void BankSystem::withdrawAmount() {
    string accNum; double amount;
    cout << "Account Number: ";
    cin >> accNum;
    Account* acc = findAccount(accNum);
    if (!acc) { cout << "Account not found.\n"; return; }

    cout << "Amount to Withdraw: ";
    if (!(cin >> amount) || !validateAmount(amount)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    acc->withdraw(amount);
    saveAccounts();
    saveTransactions();
}

void BankSystem::viewAccount() {
    string accNum;
    cout << "Account Number: ";
    cin >> accNum;
    Account* acc = findAccount(accNum);
    if (acc) acc->displayAccountInfo();
    else cout << "Account not found.\n";
}

void BankSystem::showTransactions() {
    string accNum;
    cout << "Account Number: ";
    cin >> accNum;
    Account* acc = findAccount(accNum);
    if (!acc) { cout << "Account not found.\n"; return; }

    const auto& txns = acc->getTransactions();
    if (txns.empty()) { cout << "No transactions for this account.\n"; return; }

    cout << "--- Transaction History [" << accNum << "] ---\n";
    for (const Transaction& t : txns)
        cout << t.type << ": " << t.amount << "\n";
}

void BankSystem::addInterestToSavings() {
    string accNum;
    cout << "Account Number: ";
    cin >> accNum;
    Account* acc = findAccount(accNum);
    if (!acc) { cout << "Account not found.\n"; return; }
    acc->calculateInterest();
    saveAccounts();
    saveTransactions();
}

// ── Employee operations ───────────────────────────────────────────────────────

void BankSystem::addEmployee() {
    string id, name, position, empUser, empPass;
    cout << "Employee ID: ";
    cin >> id;
    for (const Employee& e : employees) {
        if (e.empID == id) { cout << "Employee ID already exists.\n"; return; }
    }

    cout << "Employee Name: ";
    cin.ignore();
    getline(cin, name);
    if (name.empty()) { cout << "Name cannot be empty.\n"; return; }

    cout << "Position: ";
    getline(cin, position);
    if (position.empty()) { cout << "Position cannot be empty.\n"; return; }

    cout << "Employee Login Username: ";
    cin >> empUser;
    if (credentialExists(empUser)) {
        cout << "Username already taken.\n";
        return;
    }

    cout << "Employee Login Password: ";
    cin >> empPass;
    if (empPass.empty()) { cout << "Password cannot be empty.\n"; return; }

    employees.push_back(Employee(id, name, position));
    saveEmployees();
    appendCredential(empUser, empPass, "employee");
    cout << "Employee added successfully. Login: " << empUser << "\n";
}

void BankSystem::viewEmployees() {
    if (employees.empty()) { cout << "No employees on record.\n"; return; }
    cout << "--- Employee List ---\n";
    for (const Employee& e : employees)
        cout << "ID: " << e.empID << " | Name: " << e.name
             << " | Position: " << e.position << "\n";
}

// ── User (auxiliary contact) operations ──────────────────────────────────────

void BankSystem::addUser() {
    string id, name, phone;
    cout << "User ID: ";
    cin >> id;
    for (const User& u : users) {
        if (u.userID == id) { cout << "User ID already exists.\n"; return; }
    }

    cout << "User Name: ";
    cin.ignore();
    getline(cin, name);
    if (name.empty()) { cout << "Name cannot be empty.\n"; return; }

    cout << "Phone Number: ";
    getline(cin, phone);
    if (phone.empty()) { cout << "Phone cannot be empty.\n"; return; }

    users.push_back(User(id, name, phone));
    saveUsers();
    cout << "User contact added successfully.\n";
}

void BankSystem::viewUsers() {
    if (users.empty()) { cout << "No users on record.\n"; return; }
    cout << "--- User Contact List ---\n";
    for (const User& u : users)
        cout << "ID: " << u.userID << " | Name: " << u.name
             << " | Phone: " << u.phone << "\n";
}

// ── Admin-only credential management ─────────────────────────────────────────

void BankSystem::addCredential() {
    string username, password, role;

    cout << "Username: "; cin >> username;
    if (credentialExists(username)) {
        cout << "Username already exists.\n"; return;
    }
    cout << "Password: "; cin >> password;
    if (password.empty()) { cout << "Password cannot be empty.\n"; return; }

    cout << "Role (admin / employee / customer): ";
    cin >> role;

    if (role == "customer") {
        string accNum;
        cout << "Linked Account Number: ";
        cin >> accNum;
        if (!accountExists(accNum)) {
            cout << "Account does not exist.\n"; return;
        }
        appendCredential(username, password, "customer:" + accNum);
    } else if (role == "admin" || role == "employee") {
        appendCredential(username, password, role);
    } else {
        cout << "Invalid role.\n"; return;
    }

    cout << "Credential added.\n";
}

void BankSystem::removeCredential() {
    string username;
    cout << "Username to remove: ";
    cin >> username;

    if (username == session.username) {
        cout << "You cannot remove your own credential.\n"; return;
    }

    ifstream in("auth.txt");
    if (!in.is_open()) { cout << "No credentials file.\n"; return; }

    vector<string> kept;
    string line;
    bool found = false;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string u;
        getline(ss, u, '|');
        if (u == username) { found = true; continue; }
        kept.push_back(line);
    }
    in.close();

    if (!found) { cout << "Username not found.\n"; return; }

    ofstream out("auth.txt");
    for (const string& l : kept) out << l << "\n";
    cout << "Credential removed.\n";
}

void BankSystem::viewCredentials() {
    ifstream in("auth.txt");
    if (!in.is_open()) { cout << "No credentials.\n"; return; }
    cout << "--- Login Credentials ---\n";
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string u, p, role;
        getline(ss, u, '|');
        getline(ss, p, '|');
        getline(ss, role);
        if (role.empty()) role = "admin";
        cout << "Username: " << u << " | Role: " << role << "\n";
    }
}

// ── Customer-only operations (act on session.accountNumber) ───────────────────

void BankSystem::customerViewAccount() {
    Account* acc = findAccount(session.accountNumber);
    if (!acc) { cout << "Your account is missing. Contact admin.\n"; return; }
    acc->displayAccountInfo();
}

void BankSystem::customerShowTransactions() {
    Account* acc = findAccount(session.accountNumber);
    if (!acc) { cout << "Your account is missing. Contact admin.\n"; return; }

    const auto& txns = acc->getTransactions();
    if (txns.empty()) { cout << "You have no transactions yet.\n"; return; }

    cout << "--- Your Transaction History ---\n";
    for (const Transaction& t : txns)
        cout << t.type << ": " << t.amount << "\n";
}

void BankSystem::customerDeposit() {
    Account* acc = findAccount(session.accountNumber);
    if (!acc) { cout << "Your account is missing. Contact admin.\n"; return; }

    double amount;
    cout << "Amount to Deposit: ";
    if (!(cin >> amount) || !validateAmount(amount)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    acc->deposit(amount);
    saveAccounts();
    saveTransactions();
}

void BankSystem::customerWithdraw() {
    Account* acc = findAccount(session.accountNumber);
    if (!acc) { cout << "Your account is missing. Contact admin.\n"; return; }

    double amount;
    cout << "Amount to Withdraw: ";
    if (!(cin >> amount) || !validateAmount(amount)) {
        cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        return;
    }
    acc->withdraw(amount);
    saveAccounts();
    saveTransactions();
}

// ── Persistence ───────────────────────────────────────────────────────────────
// All files use '|' as delimiter to safely handle names/values with spaces.

void BankSystem::saveAccounts() const {
    ofstream out("accounts.txt");
    for (const Account* acc : accounts) {
        out << acc->getType()          << "|"
            << acc->getAccountNumber() << "|"
            << acc->getHolderName()    << "|"
            << acc->getBalance()       << "|"
            << acc->getTypeParam()     << "\n";
    }
}

void BankSystem::loadAccounts() {
    ifstream in("accounts.txt");
    if (!in.is_open()) return;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string type, accNum, name, balStr, paramStr;
        getline(ss, type,    '|');
        getline(ss, accNum,  '|');
        getline(ss, name,    '|');
        getline(ss, balStr,  '|');
        getline(ss, paramStr);
        double balance = stod(balStr);
        double param   = stod(paramStr);
        if (type == "savings")
            accounts.push_back(new SavingsAccount(accNum, name, balance, param));
        else if (type == "current")
            accounts.push_back(new CurrentAccount(accNum, name, balance, param));
    }
}

void BankSystem::saveTransactions() const {
    ofstream out("transactions.txt");
    for (const Account* acc : accounts)
        for (const Transaction& t : acc->getTransactions())
            out << acc->getAccountNumber() << "|" << t.type << "|" << t.amount << "\n";
}

void BankSystem::loadTransactions() {
    ifstream in("transactions.txt");
    if (!in.is_open()) return;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string accNum, type, amtStr;
        getline(ss, accNum,  '|');
        getline(ss, type,    '|');
        getline(ss, amtStr);
        Account* acc = findAccount(accNum);
        if (acc) acc->loadTransaction(type, stod(amtStr));
    }
}

void BankSystem::saveEmployees() const {
    ofstream out("employees.txt");
    for (const Employee& e : employees)
        out << e.empID << "|" << e.name << "|" << e.position << "\n";
}

void BankSystem::loadEmployees() {
    ifstream in("employees.txt");
    if (!in.is_open()) return;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string id, name, pos;
        getline(ss, id,   '|');
        getline(ss, name, '|');
        getline(ss, pos);
        employees.push_back(Employee(id, name, pos));
    }
}

void BankSystem::saveUsers() const {
    ofstream out("users.txt");
    for (const User& u : users)
        out << u.userID << "|" << u.name << "|" << u.phone << "\n";
}

void BankSystem::loadUsers() {
    ifstream in("users.txt");
    if (!in.is_open()) return;
    string line;
    while (getline(in, line)) {
        if (line.empty()) continue;
        istringstream ss(line);
        string id, name, phone;
        getline(ss, id,    '|');
        getline(ss, name,  '|');
        getline(ss, phone);
        users.push_back(User(id, name, phone));
    }
}
