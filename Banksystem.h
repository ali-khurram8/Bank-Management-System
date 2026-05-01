#ifndef BANKSYSTEM_H
#define BANKSYSTEM_H

#include "Accounts.h"
#include "Entities.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <limits>

struct LoginSession {
    std::string username;
    std::string role;            // "admin" | "employee" | "customer"
    std::string accountNumber;   // populated only for customers
    bool authenticated = false;
};

class BankSystem {
private:
    std::vector<Account*> accounts;
    std::vector<Employee>  employees;
    std::vector<User>      users;
    LoginSession session;

    // Persistence
    void saveAccounts() const;
    void loadAccounts();
    void saveTransactions() const;
    void loadTransactions();
    void saveEmployees() const;
    void loadEmployees();
    void saveUsers() const;
    void loadUsers();

    // Validation helpers
    bool accountExists(const std::string& accNum) const;
    bool validateAmount(double amount) const;

    // Credential helpers
    bool credentialExists(const std::string& username) const;
    void appendCredential(const std::string& username,
                          const std::string& password,
                          const std::string& role);

public:
    BankSystem();
    ~BankSystem();

    bool login();
    const LoginSession& getSession() const { return session; }

    // Admin / Employee operations
    void createAccount();
    Account* findAccount(const std::string& accNum);
    void depositAmount();
    void withdrawAmount();
    void viewAccount();
    void showTransactions();
    void addInterestToSavings();

    void addEmployee();
    void viewEmployees();

    void addUser();
    void viewUsers();

    // Admin-only operations
    void addCredential();
    void removeCredential();
    void viewCredentials();

    // Customer-only operations (act on session.accountNumber)
    void customerViewAccount();
    void customerShowTransactions();
    void customerDeposit();
    void customerWithdraw();
};

#endif
