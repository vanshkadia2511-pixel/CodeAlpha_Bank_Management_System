/*
    =====================================================
    ACCOUNT.H
    Represents a BANK ACCOUNT: an account number, a balance,
    and the Customer who owns it. Knows how to deposit,
    withdraw, transfer, and how to save/load itself from disk.
    =====================================================
*/
#ifndef ACCOUNT_H
#define ACCOUNT_H

#include "Utils.h"
#include "Customer.h"
#include "Transaction.h"
#include <algorithm>

class Account
{
private:
    long long accountNumber;
    double    balance;
    Customer  customer;

public:
    Account() : accountNumber(0), balance(0) {}

    Account(long long accNo, double bal, const Customer &cust)
        : accountNumber(accNo), balance(bal), customer(cust) {}

    // ---------------- Getters ----------------
    long long getAccountNumber() const { return accountNumber; }
    double    getBalance()       const { return balance; }
    Customer& getCustomer()             { return customer; } // non-const: allows editing
    const Customer& getCustomer() const { return customer; }

    // ---------------- Core banking operations ----------------

    // Adds money to the account. Returns false (with a message) if invalid.
    bool deposit(double amount, string &errorMsg)
    {
        if (amount <= 0)
        {
            errorMsg = "Deposit amount must be positive.";
            return false;
        }
        balance += amount;
        return true;
    }

    // Removes money from the account, respecting the minimum balance rule.
    bool withdraw(double amount, string &errorMsg)
    {
        if (amount <= 0)
        {
            errorMsg = "Withdrawal amount must be positive.";
            return false;
        }
        if (balance - amount < MINIMUM_BALANCE)
        {
            errorMsg = "Insufficient balance. Minimum balance of Rs." +
                       to_string((int)MINIMUM_BALANCE) + " must be maintained.";
            return false;
        }
        balance -= amount;
        return true;
    }

    // Transfers money from this account to another Account object.
    // Handles both sides: balances, saving both files, and logging
    // a transaction on both accounts.
    bool transferTo(Account &receiver, double amount, string &errorMsg)
    {
        if (receiver.getAccountNumber() == accountNumber)
        {
            errorMsg = "You cannot transfer money to your own account.";
            return false;
        }
        if (amount <= 0)
        {
            errorMsg = "Transfer amount must be positive.";
            return false;
        }
        if (balance - amount < MINIMUM_BALANCE)
        {
            errorMsg = "Insufficient balance. Minimum balance of Rs." +
                       to_string((int)MINIMUM_BALANCE) + " must be maintained.";
            return false;
        }

        balance -= amount;
        receiver.balance += amount;

        if (!saveToFile() || !receiver.saveToFile())
        {
            // Roll back if saving failed
            balance += amount;
            receiver.balance -= amount;
            errorMsg = "Transfer failed due to a file error.";
            return false;
        }

        addTransaction("Transfer Sent to " + to_string(receiver.getAccountNumber()), amount);
        receiver.addTransaction("Transfer Received from " + to_string(accountNumber), amount);
        return true;
    }

    // ---------------- File storage ----------------

    // Saves this account + its owner's details to users/<accountNumber>.txt
    bool saveToFile() const
    {
        string path = USERS_DIR + to_string(accountNumber) + ".txt";
        ofstream fout(path);
        if (!fout.is_open()) return false;

        fout << accountNumber            << "\n"
             << customer.getCustomerID() << "\n"
             << customer.getFullName()   << "\n"
             << customer.getUsername()   << "\n"
             << customer.getPassword()   << "\n"
             << customer.getEmail()      << "\n"
             << customer.getPhone()      << "\n"
             << customer.getAddress()    << "\n"
             << customer.getDOB()        << "\n"
             << fixed << setprecision(2) << balance << "\n";

        fout.close();
        return true;
    }

    // Loads an account (and its Customer) from users/<accountNumber>.txt
    static bool loadFromFile(long long accNo, Account &account)
    {
        string path = USERS_DIR + to_string(accNo) + ".txt";
        ifstream fin(path);
        if (!fin.is_open()) return false;

        string accNoStr, custID, name, uname, pass, mail, ph, addr, dobStr, balStr;

        getline(fin, accNoStr);
        getline(fin, custID);
        getline(fin, name);
        getline(fin, uname);
        getline(fin, pass);
        getline(fin, mail);
        getline(fin, ph);
        getline(fin, addr);
        getline(fin, dobStr);
        getline(fin, balStr);
        fin.close();

        Customer cust(custID, name, uname, pass, mail, ph, addr, dobStr);
        account = Account(stoll(accNoStr), stod(balStr), cust);
        return true;
    }

    // ---------------- Transaction history ----------------

    // Creates a Transaction "receipt" for this account and appends it to file
    bool addTransaction(const string &type, double amount) const
    {
        Transaction t = Transaction::createNow(type, amount, balance);
        string path = TRANS_DIR + to_string(accountNumber) + "_history.txt";
        ofstream fout(path, ios::app);
        if (!fout.is_open()) return false;
        fout << t.toFileLine() << "\n";
        fout.close();
        return true;
    }

    // Returns every transaction ever recorded for this account
    vector<Transaction> getAllTransactions() const
    {
        vector<Transaction> result;
        string path = TRANS_DIR + to_string(accountNumber) + "_history.txt";
        ifstream fin(path);
        if (!fin.is_open()) return result;

        string line;
        while (getline(fin, line))
        {
            if (!trim(line).empty())
                result.push_back(Transaction::fromFileLine(line));
        }
        fin.close();
        return result;
    }

    // Returns only the most recent N transactions
    vector<Transaction> getRecentTransactions(int count) const
    {
        vector<Transaction> all = getAllTransactions();
        vector<Transaction> recent;
        int start = max(0, (int)all.size() - count);
        for (int i = start; i < (int)all.size(); i++)
            recent.push_back(all[i]);
        return recent;
    }

    // ---------------- Display ----------------

    void displayFullProfile() const
    {
        cout << "\n===================== PROFILE DETAILS =====================\n";
        cout << left << setw(20) << "Account Number" << ": " << accountNumber << "\n";
        customer.displayProfile();
        cout << left << setw(20) << "Balance" << ": Rs." << fixed << setprecision(2) << balance << "\n";
    }

    void displayBalance() const
    {
        cout << "\n  Current Balance: Rs." << fixed << setprecision(2) << balance << "\n";
    }
};

#endif // ACCOUNT_H