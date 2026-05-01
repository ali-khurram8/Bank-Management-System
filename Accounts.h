#ifndef ACCOUNTS_H
#define ACCOUNTS_H

#include <string>
#include <vector>
#include "Entities.h"

class Account {
protected:
    std::string accountNumber;
    std::string accountHolderName;
    double balance;
    std::vector<Transaction> transactions;

public:
    Account(std::string accNum = "", std::string name = "", double initialBalance = 0.0);
    virtual ~Account() = default;

    virtual void deposit(double amount);
    virtual void withdraw(double amount);
    virtual void displayAccountInfo() const;
    virtual void calculateInterest();

    // Used for file persistence
    virtual std::string getType() const = 0;
    virtual double getTypeParam() const = 0;

    std::string getAccountNumber() const;
    std::string getHolderName() const;
    double getBalance() const;
    const std::vector<Transaction>& getTransactions() const;
    void loadTransaction(const std::string& type, double amount);
};

class SavingsAccount : public Account {
private:
    double interestRate;
public:
    SavingsAccount(std::string accNum = "", std::string name = "", double initialBalance = 0.0, double rate = 0.03);
    void calculateInterest() override;
    std::string getType() const override;
    double getTypeParam() const override;
};

class CurrentAccount : public Account {
private:
    double overdraftLimit;
public:
    CurrentAccount(std::string accNum = "", std::string name = "", double initialBalance = 0.0, double overdraft = 5000.0);
    void withdraw(double amount) override;
    void displayAccountInfo() const override;
    void calculateInterest() override;
    std::string getType() const override;
    double getTypeParam() const override;
};

#endif
