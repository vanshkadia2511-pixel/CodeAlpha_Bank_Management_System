/*
    =====================================================
    CUSTOMER.H
    Represents the PERSONAL details of a bank customer:
    identity, login credentials and contact information.
    Also owns the static validation rules for these fields.
    =====================================================
*/
#ifndef CUSTOMER_H
#define CUSTOMER_H

#include "Utils.h"
#include <cctype>

class Customer
{
private:
    string customerID;
    string fullName;
    string username;
    string password;
    string email;
    string phone;
    string address;
    string dob;

public:
    Customer() = default;

    Customer(string cid, string name, string uname, string pass,
             string mail, string ph, string addr, string dateOfBirth)
        : customerID(cid), fullName(name), username(uname), password(pass),
          email(mail), phone(ph), address(addr), dob(dateOfBirth) {}

    // ---------------- Getters ----------------
    string getCustomerID() const { return customerID; }
    string getFullName()   const { return fullName; }
    string getUsername()   const { return username; }
    string getPassword()   const { return password; }
    string getEmail()      const { return email; }
    string getPhone()      const { return phone; }
    string getAddress()    const { return address; }
    string getDOB()         const { return dob; }

    // ---------------- Setters (used for profile updates) ----------------
    void setCustomerID(const string &v) { customerID = v; }
    void setFullName(const string &v)   { fullName = v; }
    void setUsername(const string &v)   { username = v; }
    void setPassword(const string &v)   { password = v; }
    void setEmail(const string &v)      { email = v; }
    void setPhone(const string &v)      { phone = v; }
    void setAddress(const string &v)    { address = v; }
    void setDOB(const string &v)         { dob = v; }

    // ---------------- Display ----------------
    // Prints personal details only (never the password)
    void displayProfile() const
    {
        cout << left << setw(20) << "Customer ID"   << ": " << customerID << "\n";
        cout << left << setw(20) << "Full Name"     << ": " << fullName   << "\n";
        cout << left << setw(20) << "Username"      << ": " << username   << "\n";
        cout << left << setw(20) << "Email"         << ": " << email      << "\n";
        cout << left << setw(20) << "Phone"         << ": " << phone      << "\n";
        cout << left << setw(20) << "Address"       << ": " << address    << "\n";
        cout << left << setw(20) << "Date of Birth" << ": " << dob        << "\n";
    }

    // ---------------- Validation rules ----------------
    static bool validateUsername(const string &username)
    {
        if (username.length() < 5) return false;
        for (char c : username)
        {
            if (isspace((unsigned char)c)) return false;
            if (!isalnum((unsigned char)c) && c != '_') return false;
        }
        return true;
    }

    static bool validatePassword(const string &password)
    {
        if (password.length() < 8) return false;
        bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
        for (char c : password)
        {
            if (isupper((unsigned char)c)) hasUpper = true;
            else if (islower((unsigned char)c)) hasLower = true;
            else if (isdigit((unsigned char)c)) hasDigit = true;
            else if (!isspace((unsigned char)c)) hasSpecial = true;
        }
        return hasUpper && hasLower && hasDigit && hasSpecial;
    }

    static bool validatePhone(const string &phone)
    {
        if (phone.length() != 10) return false;
        for (char c : phone)
            if (!isdigit((unsigned char)c)) return false;
        return true;
    }

    static bool validateEmail(const string &email)
    {
        if (email.find(' ') != string::npos) return false;
        size_t atPos = email.find('@');
        if (atPos == string::npos || atPos == 0) return false;
        if (email.find('@', atPos + 1) != string::npos) return false;
        size_t dotPos = email.find('.', atPos);
        if (dotPos == string::npos) return false;
        if (dotPos == atPos + 1) return false;
        if (dotPos == email.length() - 1) return false;
        return true;
    }

    static bool validateDOB(const string &dob)
    {
        if (dob.length() != 10) return false;
        if (dob[2] != '-' || dob[5] != '-') return false;
        for (size_t i = 0; i < dob.length(); i++)
        {
            if (i == 2 || i == 5) continue;
            if (!isdigit((unsigned char)dob[i])) return false;
        }
        int day = stoi(dob.substr(0, 2));
        int month = stoi(dob.substr(3, 2));
        int year = stoi(dob.substr(6, 4));
        if (month < 1 || month > 12) return false;
        if (day < 1 || day > 31) return false;
        if (year < 1900 || year > 2100) return false;
        return true;
    }
};

#endif // CUSTOMER_H