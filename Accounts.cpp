#include "Accounts.h"
#include <iostream>
using namespace std;

Account::Account(string accNum, string name, double initialBalance)
    : accountNumber(accNum), accountHolderName(name), balance(initialBalance) {}

void Account::deposit(double amount) {
    balance += amount;
    transactions.push_back(Transaction("Deposit", amount));
    cout << "Deposit successful. New balance: " << balance << endl;
}

void Account::withdraw(double amount) {
    if (balance >= amount) {
        balance -= amount;
        transactions.push_back(Transaction("Withdraw", amount));
        cout << "Withdrawal successful. New balance: " << balance << endl;
    } else {
        cout << "Insufficient balance." << endl;
    }
}

void Account::displayAccountInfo() const {
    cout << "Account Number : " << accountNumber << endl;
    cout << "Account Holder : " << accountHolderName << endl;
    cout << "Balance        : " << balance << endl;
}

void Account::calculateInterest() {
    cout << "Interest calculation is not applicable for this account type." << endl;
}

string Account::getAccountNumber() const { return accountNumber; }
string Account::getHolderName() const    { return accountHolderName; }
double Account::getBalance() const       { return balance; }

const vector<Transaction>& Account::getTransactions() const { return transactions; }

void Account::loadTransaction(const string& type, double amount) {
    transactions.push_back(Transaction(type, amount));
}

// ── SavingsAccount ──────────────────────────────────────────────────────────

SavingsAccount::SavingsAccount(string accNum, string name, double initialBalance, double rate)
    : Account(accNum, name, initialBalance), interestRate(rate) {}

void SavingsAccount::calculateInterest() {
    double interest = balance * interestRate;
    balance += interest;
    transactions.push_back(Transaction("Interest", interest));
    cout << "Interest added: " << interest << ". New balance: " << balance << endl;
}

string SavingsAccount::getType() const     { return "savings"; }
double SavingsAccount::getTypeParam() const { return interestRate; }

// ── CurrentAccount ───────────────────────────────────────────────────────────

CurrentAccount::CurrentAccount(string accNum, string name, double initialBalance, double overdraft)
    : Account(accNum, name, initialBalance), overdraftLimit(overdraft) {}

void CurrentAccount::withdraw(double amount) {
    if (balance + overdraftLimit >= amount) {
        balance -= amount;
        transactions.push_back(Transaction("Withdraw", amount));
        cout << "Withdrawal successful. New balance: " << balance << endl;
    } else {
        cout << "Withdrawal exceeds overdraft limit." << endl;
    }
}

void CurrentAccount::displayAccountInfo() const {
    Account::displayAccountInfo();
    cout << "Overdraft Limit: " << overdraftLimit << endl;
}

void CurrentAccount::calculateInterest() {
    cout << "Current accounts do not earn interest." << endl;
}

string CurrentAccount::getType() const     { return "current"; }
double CurrentAccount::getTypeParam() const { return overdraftLimit; }
