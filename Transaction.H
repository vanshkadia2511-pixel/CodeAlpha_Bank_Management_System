/*
    =====================================================
    TRANSACTION.H
    Represents ONE transaction record (a single line in a
    customer's history file). Knows how to turn itself into
    text and how to rebuild itself from text.
    =====================================================
*/
#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "Utils.h"

class Transaction
{
private:
    string transactionID;
    string date;
    string time;
    string type;
    double amount;
    double balanceAfter;

public:
    Transaction() : amount(0), balanceAfter(0) {}

    Transaction(string id, string d, string t, string ty, double amt, double bal)
        : transactionID(id), date(d), time(t), type(ty), amount(amt), balanceAfter(bal) {}

    // Builds a brand-new transaction using the current date/time
    static Transaction createNow(const string &type, double amount, double balanceAfter)
    {
        static int counter = 0;
        counter++;
        ostringstream idStream;
        idStream << "TXN" << ::time(nullptr) << setfill('0') << setw(4) << counter;

        return Transaction(idStream.str(), currentDate(), currentTime(), type, amount, balanceAfter);
    }

    // Converts this transaction into one "|" separated line for file storage
    string toFileLine() const
    {
        ostringstream oss;
        oss << transactionID << "|" << date << "|" << time << "|" << type << "|"
            << fixed << setprecision(2) << amount << "|"
            << fixed << setprecision(2) << balanceAfter;
        return oss.str();
    }

    // Rebuilds a Transaction object from a stored "|" separated line
    static Transaction fromFileLine(const string &line)
    {
        vector<string> f = splitLine(line, '|');
        if (f.size() < 6) return Transaction();
        return Transaction(f[0], f[1], f[2], f[3], stod(f[4]), stod(f[5]));
    }

    // Getters
    string getID() const { return transactionID; }
    string getDate() const { return date; }
    string getTime() const { return time; }
    string getType() const { return type; }
    double getAmount() const { return amount; }
    double getBalanceAfter() const { return balanceAfter; }

    // Prints a header + separator line for a transaction table
    static void printTableHeader()
    {
        cout << "\n" << left
             << setw(18) << "Transaction ID" << " "
             << setw(12) << "Date" << " "
             << setw(10) << "Time" << " "
             << setw(24) << "Type" << " "
             << setw(12) << "Amount" << " "
             << setw(12) << "Balance" << "\n";
        cout << string(96, '-') << "\n";
    }

    // Prints this single transaction as one aligned row
    void printRow() const
    {
        cout << left
             << setw(18) << transactionID << " "
             << setw(12) << date << " "
             << setw(10) << time << " "
             << setw(24) << type << " "
             << setw(12) << fixed << setprecision(2) << amount << " "
             << setw(12) << fixed << setprecision(2) << balanceAfter << "\n";
    }

    // Prints a whole list of transactions as a neat table
    static void printTable(const vector<Transaction> &list)
    {
        printTableHeader();
        for (const auto &t : list) t.printRow();
        cout << string(96, '-') << "\n";
    }
};

#endif // TRANSACTION_H