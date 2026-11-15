#include <iostream>
#include <string>
#include <exception>
#include <iomanip>

using namespace std;

// Base Custom Exception
class BankException : public exception {
protected:
    string message;
public:
    explicit BankException(const string& msg) : message(msg) {}
    const char* what() const noexcept override {
        return message.c_str();
    }
};

// Derived Exception 1: Insufficient Funds
class InsufficientFundsException : public BankException {
public:
    InsufficientFundsException(double balance, double amount)
        : BankException("Insufficient Funds! Available: " + to_string(balance) + ", Requested: " + to_string(amount)) {}
};

// Derived Exception 2: Invalid Amount
class InvalidAmountException : public BankException {
public:
    explicit InvalidAmountException(double amount)
        : BankException("Invalid Transaction Amount (" + to_string(amount) + "). Amount must be positive.") {}
};

class BankAccount {
private:
    string accountHolder;
    double balance;

public:
    BankAccount(const string& holder, double initialBalance) : accountHolder(holder) {
        if (initialBalance < 0) {
            throw InvalidAmountException(initialBalance);
        }
        balance = initialBalance;
    }

    // Provides Strong Exception Safety Guarantee: state remains unchanged if exception is thrown
    void deposit(double amount) {
        if (amount <= 0) {
            throw InvalidAmountException(amount);
        }
        balance += amount;
        cout << "Deposited: " << fixed << setprecision(2) << amount << " | New Balance: " << balance << "\n";
    }

    void withdraw(double amount) {
        if (amount <= 0) {
            throw InvalidAmountException(amount);
        }
        if (amount > balance) {
            throw InsufficientFundsException(balance, amount);
        }
        balance -= amount;
        cout << "Withdrawn: " << fixed << setprecision(2) << amount << " | New Balance: " << balance << "\n";
    }

    void transfer(BankAccount& target, double amount) {
        // Strong exception safety: withdraw throws before target is modified
        withdraw(amount);
        try {
            target.deposit(amount);
            cout << "Transfer to " << target.accountHolder << " successful.\n";
        } catch (...) {
            // Rollback if deposit fails
            balance += amount;
            throw;
        }
    }

    double getBalance() const { return balance; }
};

int main() {
    try {
        string name;
        double initialBal;
        cout << "Enter Account Holder Name and Initial Balance: ";
        cin >> name >> initialBal;

        BankAccount acc1(name, initialBal);
        BankAccount acc2("Reserve_Account", 1000.0);

        int op;
        double amt;
        cout << "\n1. Deposit\n2. Withdraw\n3. Transfer to Reserve_Account\nEnter operation: ";
        cin >> op;
        cout << "Enter amount: ";
        cin >> amt;

        if (op == 1) acc1.deposit(amt);
        else if (op == 2) acc1.withdraw(amt);
        else if (op == 3) acc1.transfer(acc2, amt);
        else cout << "Invalid operation.\n";

        cout << "Final Balance (" << name << "): " << fixed << setprecision(2) << acc1.getBalance() << "\n";
    }
    catch (const InsufficientFundsException& e) {
        cerr << "[ERROR - InsufficientFundsException] " << e.what() << "\n";
    }
    catch (const InvalidAmountException& e) {
        cerr << "[ERROR - InvalidAmountException] " << e.what() << "\n";
    }
    catch (const BankException& e) {
        cerr << "[ERROR - BankException] " << e.what() << "\n";
    }

    return 0;
}