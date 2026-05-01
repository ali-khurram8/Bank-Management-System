#include "Entities.h"

Transaction::Transaction(std::string t, double a) : type(t), amount(a) {}
Employee::Employee(std::string id, std::string n, std::string pos) : empID(id), name(n), position(pos) {}
User::User(std::string id, std::string n, std::string ph) : userID(id), name(n), phone(ph) {}
