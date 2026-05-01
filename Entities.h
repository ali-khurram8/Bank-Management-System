#ifndef ENTITIES_H
#define ENTITIES_H

#include <string>

struct Transaction {
    std::string type;
    double amount;
    Transaction(std::string t = "", double a = 0.0);
};

class Employee {
public:
    std::string empID;
    std::string name;
    std::string position;
    Employee(std::string id = "", std::string n = "", std::string pos = "");
};

class User {
public:
    std::string userID;
    std::string name;
    std::string phone;
    User(std::string id = "", std::string n = "", std::string ph = "");
};

#endif
