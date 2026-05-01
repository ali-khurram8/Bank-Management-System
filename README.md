# Bank Management System

A console-based Bank Management System written in **C++**, built as a university project to demonstrate Object-Oriented Programming concepts including inheritance, polymorphism, virtual functions, file persistence, and **role-based access control with separation of duties**.

---

## Features

- **Three-Role Login System** — Each role has its own menu and a strictly limited set of permissions:
  - **Admin** — System management only: employees, credentials, audit views (cannot move money)
  - **Employee (Teller)** — All banking operations: open accounts, deposit, withdraw, calculate interest
  - **Customer** — Self-service portal: view *only their own* account and transact on it
- **Separation of Duties** — Admin manages access, employees handle money — no single role can both grant access and drain funds
- **Account Types** — Savings (with compound interest, default 3%) and Current (with overdraft, default 5000)
- **Per-Account Transaction History** — Every deposit, withdrawal, and interest accrual is recorded against the specific account
- **Auto-Generated Customer Logins** — When an employee opens an account, a customer login is created in the same form
- **Credential Management** — Admins can add, remove, and view login credentials (passwords hidden in audit view)
- **Input Validation** — Negative amounts blocked, duplicates rejected, malformed input handled gracefully
- **Full Persistence** — All data survives across sessions using pipe-delimited text files
- **Two UI Modes** — Plain console and a colored GUI console (ANSI colors, loading bar, centered output)

---

## Project Structure

```
Bank Management System/
│
├── Entities.h / Entities.cpp         # Transaction, Employee, User entities
├── Accounts.h / Accounts.cpp         # Account (base), SavingsAccount, CurrentAccount
├── Banksystem.h / Banksystem.cpp     # BankSystem core + LoginSession + persistence
│
├── main.cpp                          # Plain console entry point (3 role-based menus)
├── BankManagementSystemwithgui.cpp   # Colored GUI console entry point (3 role-based menus)
│
├── BankManagementSystem.cpp          # Legacy single-file version (reference only)
│
├── auth.txt                          # Auto-generated: usernames, passwords, roles
├── accounts.txt                      # Auto-generated: persisted accounts
├── transactions.txt                  # Auto-generated: persisted transaction history
├── employees.txt                     # Auto-generated: employee records
└── users.txt                         # Auto-generated: customer contact records
```

---

## Role Permissions (Separation of Duties)

The system follows the **separation of duties** security principle: the admin can manage *who* uses the system but cannot move money, while employees handle daily banking but cannot manage credentials. This prevents a single compromised account from being able to both grant access and drain funds.

| Operation | Admin | Employee | Customer |
|---|:-:|:-:|:-:|
| Create Account (with customer login) | ❌ | ✅ | ❌ |
| Deposit / Withdraw on any account | ❌ | ✅ | ❌ |
| Deposit / Withdraw on **own** account | — | — | ✅ |
| View any account / transactions (audit) | ✅ (read-only) | ✅ | ❌ |
| View **own** account / transactions | — | — | ✅ |
| Calculate Interest | ❌ | ✅ | ❌ |
| Add / View Employees | ✅ | ❌ | ❌ |
| Add / View Customer Contacts | ✅ | ✅ | ❌ |
| Add / Remove / View Credentials | ✅ | ❌ | ❌ |

---

## OOP Concepts Demonstrated

| Concept | Where |
|---|---|
| **Inheritance** | `SavingsAccount` and `CurrentAccount` extend `Account` |
| **Polymorphism** | `withdraw()`, `displayAccountInfo()`, `calculateInterest()` overridden per type |
| **Virtual Functions** | Base `Account` declares pure-virtual `getType()` / `getTypeParam()` for persistence |
| **Encapsulation** | Private members + public getters; `BankSystem` hides persistence details |
| **Composition** | Each `Account` owns a `std::vector<Transaction>` |
| **Dynamic Memory** | Account pointers stored in `std::vector`, freed in destructor |
| **File I/O** | All data persists across sessions using pipe-delimited text files |
| **Access Control** | `LoginSession` struct + role-aware menus enforce permissions |

---

## How to Compile & Run

> Requires a C++11 (or later) compiler such as `g++`.

### Plain Console Version

```bash
g++ main.cpp Banksystem.cpp Accounts.cpp Entities.cpp -o bank
./bank
```

### Colored GUI Version

```bash
g++ BankManagementSystemwithgui.cpp Banksystem.cpp Accounts.cpp Entities.cpp -o bank_gui
./bank_gui
```

> **Note:** Do **not** compile `main.cpp` and `BankManagementSystemwithgui.cpp` together — both define `main()`.

---

## Default Credentials

`auth.txt` is auto-created on first run with two default accounts:

| Username | Password | Role |
|---|---|---|
| `admin` | `1234` | admin |
| `teller` | `0000` | employee |

Customer credentials are not seeded by default — they are created when an **employee** opens a new account.

---

## Typical Workflow

1. **First run** — log in as `teller` / `0000`
2. **Employee** opens a new account using menu option **1** (Create Account). The same form asks for a customer username and password — these are stored in `auth.txt` automatically.
3. **Logout**, then run the program again.
4. **Customer** logs in using the credentials from step 2 — they see only their own account, balance, and transaction history.
5. Optional: log in as `admin` / `1234` to add more employees, audit credentials, or view (read-only) any account.

---

## Data Files

All data files are auto-created in the **working directory** on first run. They use `|` as a delimiter to safely handle names with spaces.

| File | Format |
|---|---|
| `auth.txt` | `username\|password\|role` (role: `admin`, `employee`, or `customer:ACC_NUM`) |
| `accounts.txt` | `type\|number\|holder\|balance\|interestRate-or-overdraft` |
| `transactions.txt` | `accountNumber\|type\|amount` |
| `employees.txt` | `id\|name\|position` |
| `users.txt` | `id\|name\|phone` |

---

## Screenshots

### Login
```
======= BANK MANAGEMENT SYSTEM =======
Username: admin
Password: ****
Login successful. Welcome, admin (admin)!
```

### Admin Menu — system management only, no banking ops
```
-------- ADMIN MENU --------
 1.  Add Employee (with login)
 2.  View Employees
 3.  View Account (audit, read-only)
 4.  View Transactions (audit, read-only)
 5.  Add Customer Contact (User)
 6.  View Customer Contacts
 7.  Add Login Credential
 8.  Remove Login Credential
 9.  View Login Credentials
10.  Logout & Exit
```

### Employee Menu — all banking operations
```
-------- EMPLOYEE MENU --------
 1.  Create Account (with customer login)
 2.  Deposit
 3.  Withdraw
 4.  View Account
 5.  View Transactions
 6.  Calculate Interest (Savings)
 7.  Add Customer Contact
 8.  View Customer Contacts
 9.  Logout & Exit
```

### Customer Portal — self-service on own account only
```
-------- CUSTOMER PORTAL --------
 1.  View My Account
 2.  View My Transactions
 3.  Deposit
 4.  Withdraw
 5.  Logout & Exit
```

### Customer Viewing Their Own Account
```
Account Number : ACC001
Account Holder : John Doe
Balance        : 5105
```

### Credential Audit (admin)
```
--- Login Credentials ---
Username: admin   | Role: admin
Username: teller  | Role: employee
Username: john    | Role: customer:ACC001
```

---

## Requirements

- C++11 or later
- Any standard terminal (Linux, macOS, Windows Command Prompt / PowerShell)
- No external libraries required

---

## Author

**Ali Khurram**
University Project — Object-Oriented Programming in C++
