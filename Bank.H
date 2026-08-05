/*
    =====================================================
    BANK.H
    The "controller" class. Manages the overall system: folder
    setup, generating account numbers, registering customers,
    logging in, and running the on-screen menus. It creates and
    coordinates Account/Customer/Transaction objects but does
    not store customer data itself (the files on disk are the
    real storage).
    =====================================================
*/
#ifndef BANK_H
#define BANK_H

#include "Utils.h"
#include "Customer.h"
#include "Account.h"
#include "Transaction.h"

class Bank
{
public:
    // ---------------- Setup ----------------
    void createFolders()
    {
        if (!isDirectory(DATA_DIR) && !createDirectory(DATA_DIR))
            cout << "  [Warning] Could not create data directory.\n";
        if (!isDirectory(USERS_DIR) && !createDirectory(USERS_DIR))
            cout << "  [Warning] Could not create users directory.\n";
        if (!isDirectory(TRANS_DIR) && !createDirectory(TRANS_DIR))
            cout << "  [Warning] Could not create transactions directory.\n";

        if (!pathExists(ACCOUNT_FILE))
        {
            ofstream fout(ACCOUNT_FILE);
            if (fout.is_open())
            {
                fout << STARTING_ACCOUNT_NUMBER;
                fout.close();
            }
        }
    }

    // ---------------- ID / lookup helpers ----------------

    long long generateAccountNumber()
    {
        long long nextAccount = STARTING_ACCOUNT_NUMBER;

        ifstream fin(ACCOUNT_FILE);
        if (fin.is_open()) { fin >> nextAccount; fin.close(); }

        ofstream fout(ACCOUNT_FILE);
        if (fout.is_open()) { fout << (nextAccount + 1); fout.close(); }

        return nextAccount;
    }

    bool accountExists(long long accountNumber)
    {
        string path = USERS_DIR + to_string(accountNumber) + ".txt";
        return pathExists(path);
    }

    bool usernameExists(const string &username)
    {
        if (!isDirectory(USERS_DIR)) return false;
        for (const auto &path : listFiles(USERS_DIR))
        {
            ifstream fin(path);
            if (!fin.is_open()) continue;
            string line;
            int lineNo = 0;
            while (getline(fin, line))
            {
                lineNo++;
                if (lineNo == 4) // username is always line 4
                {
                    if (trim(line) == username) { fin.close(); return true; }
                    break;
                }
            }
            fin.close();
        }
        return false;
    }

    bool findAccountByUsername(const string &username, long long &accountNumber)
    {
        if (!isDirectory(USERS_DIR)) return false;
        for (const auto &path : listFiles(USERS_DIR))
        {
            ifstream fin(path);
            if (!fin.is_open()) continue;
            vector<string> lines;
            string line;
            while (getline(fin, line)) lines.push_back(line);
            fin.close();

            if (lines.size() >= 4 && trim(lines[3]) == username)
            {
                accountNumber = stoll(lines[0]);
                return true;
            }
        }
        return false;
    }

    // ---------------- Registration ----------------

    void registerCustomer()
    {
        cout << "\n===================== CREATE NEW ACCOUNT =====================\n";

        string fullName = readNonEmptyLine("Enter Full Name: ");

        string username;
        while (true)
        {
            username = readNonEmptyLine("Enter Username (min 5 chars, no spaces): ");
            if (!Customer::validateUsername(username))
            {
                cout << "  [Error] Invalid username format. Try again.\n";
                continue;
            }
            if (usernameExists(username))
            {
                cout << "  [Error] Username already taken. Choose another.\n";
                continue;
            }
            break;
        }

        string password;
        while (true)
        {
            password = readNonEmptyLine("Enter Password (min 8 chars, upper/lower/digit/special): ");
            if (!Customer::validatePassword(password))
            {
                cout << "  [Error] Password does not meet requirements.\n";
                continue;
            }
            string confirm = readNonEmptyLine("Confirm Password: ");
            if (confirm != password)
            {
                cout << "  [Error] Passwords do not match.\n";
                continue;
            }
            break;
        }

        string email;
        while (true)
        {
            email = readNonEmptyLine("Enter Email: ");
            if (!Customer::validateEmail(email))
            {
                cout << "  [Error] Invalid email format.\n";
                continue;
            }
            break;
        }

        string phone;
        while (true)
        {
            phone = readNonEmptyLine("Enter Phone Number (10 digits): ");
            if (!Customer::validatePhone(phone))
            {
                cout << "  [Error] Phone number must be exactly 10 digits.\n";
                continue;
            }
            break;
        }

        string address = readNonEmptyLine("Enter Address: ");

        string dob;
        while (true)
        {
            dob = readNonEmptyLine("Enter Date of Birth (DD-MM-YYYY): ");
            if (!Customer::validateDOB(dob))
            {
                cout << "  [Error] Invalid date format. Use DD-MM-YYYY.\n";
                continue;
            }
            break;
        }

        double deposit;
        while (true)
        {
            deposit = readDoubleInput("Enter Initial Deposit (min Rs.500): ");
            if (deposit < MINIMUM_DEPOSIT)
            {
                cout << "  [Error] Initial deposit must be at least Rs.500.\n";
                continue;
            }
            break;
        }

        long long accountNumber = generateAccountNumber();
        string customerID = "CUST" + to_string(accountNumber);

        Customer customer(customerID, fullName, username, password, email, phone, address, dob);
        Account account(accountNumber, deposit, customer);

        if (!account.saveToFile())
        {
            cout << "  [Error] Failed to save account. Please try again later.\n";
            return;
        }

        account.addTransaction("Account Created", deposit);

        cout << "\n===================== ACCOUNT CREATED SUCCESSFULLY =====================\n";
        cout << "Your Account Number : " << accountNumber << "\n";
        cout << "Your Customer ID    : " << customerID << "\n";
        cout << "Please remember your Account Number/Username for future login.\n";
    }

    // ---------------- Login + Dashboard ----------------

    void login()
    {
        cout << "\n===================== LOGIN =====================\n";
        cout << "1. Login using Username\n";
        cout << "2. Login using Account Number\n";
        int mode = readIntInput("Enter your choice: ");

        long long accountNumber = -1;

        if (mode == 1)
        {
            string username = readNonEmptyLine("Enter Username: ");
            if (!findAccountByUsername(username, accountNumber))
            {
                cout << "  [Error] No account found with that username.\n";
                return;
            }
        }
        else if (mode == 2)
        {
            accountNumber = (long long)readDoubleInput("Enter Account Number: ");
            if (!accountExists(accountNumber))
            {
                cout << "  [Error] No account found with that account number.\n";
                return;
            }
        }
        else
        {
            cout << "  [Error] Invalid choice.\n";
            return;
        }

        Account account;
        if (!Account::loadFromFile(accountNumber, account))
        {
            cout << "  [Error] Unable to load account data.\n";
            return;
        }

        int attempts = 3;
        while (attempts > 0)
        {
            string password = readNonEmptyLine("Enter Password: ");
            if (password == account.getCustomer().getPassword())
            {
                cout << "\n  [Success] Login successful. Welcome, "
                     << account.getCustomer().getFullName() << "!\n";
                dashboard(account);
                return;
            }
            attempts--;
            if (attempts > 0)
                cout << "  [Error] Incorrect password. Attempts remaining: " << attempts << "\n";
            else
                cout << "  [Error] Too many failed attempts. Account access locked for this session.\n";
        }
    }

    void dashboard(Account &account)
    {
        bool loggedIn = true;
        while (loggedIn)
        {
            cout << "\n=============================\n";
            cout << "        DASHBOARD\n";
            cout << "=============================\n";
            cout << "1. View Profile\n";
            cout << "2. Check Balance\n";
            cout << "3. Deposit\n";
            cout << "4. Withdraw\n";
            cout << "5. Transfer Money\n";
            cout << "6. Mini Statement\n";
            cout << "7. Transaction History\n";
            cout << "8. Change Password\n";
            cout << "9. Update Profile\n";
            cout << "10. Logout\n";

            int choice = readIntInput("Enter your choice: ");

            switch (choice)
            {
                case 1: account.displayFullProfile(); break;
                case 2: account.displayBalance(); break;
                case 3: handleDeposit(account); break;
                case 4: handleWithdraw(account); break;
                case 5: handleTransfer(account); break;
                case 6:
                    cout << "\n===================== MINI STATEMENT (Last 5) =====================\n";
                    Transaction::printTable(account.getRecentTransactions(5));
                    break;
                case 7:
                    cout << "\n===================== FULL TRANSACTION HISTORY =====================\n";
                    Transaction::printTable(account.getAllTransactions());
                    break;
                case 8: handleChangePassword(account); break;
                case 9: handleUpdateProfile(account); break;
                case 10:
                    cout << "\n  Logging out... Returning to Main Menu.\n";
                    loggedIn = false;
                    break;
                default:
                    cout << "  [Error] Invalid choice. Please select 1-10.\n";
            }
        }
    }

private:
    // ---------------- Dashboard action handlers ----------------
    // These live in Bank because they combine user I/O (asking
    // questions) with calls into the Account class (the actual
    // banking logic). The Account class itself has no cin/cout.

    void handleDeposit(Account &account)
    {
        cout << "\n===================== DEPOSIT MONEY =====================\n";
        double amount = readDoubleInput("Enter amount to deposit: Rs.");

        string errorMsg;
        if (!account.deposit(amount, errorMsg))
        {
            cout << "  [Error] " << errorMsg << "\n";
            return;
        }

        if (!account.saveToFile())
        {
            cout << "  [Error] Failed to update account. Deposit cancelled.\n";
            return;
        }

        account.addTransaction("Deposit", amount);
        cout << "  [Success] Rs." << fixed << setprecision(2) << amount << " deposited.\n";
        cout << "  New Balance: Rs." << fixed << setprecision(2) << account.getBalance() << "\n";
    }

    void handleWithdraw(Account &account)
    {
        cout << "\n===================== WITHDRAW MONEY =====================\n";
        double amount = readDoubleInput("Enter amount to withdraw: Rs.");

        string errorMsg;
        if (!account.withdraw(amount, errorMsg))
        {
            cout << "  [Error] " << errorMsg << "\n";
            return;
        }

        if (!account.saveToFile())
        {
            cout << "  [Error] Failed to update account. Withdrawal cancelled.\n";
            return;
        }

        account.addTransaction("Withdraw", amount);
        cout << "  [Success] Rs." << fixed << setprecision(2) << amount << " withdrawn.\n";
        cout << "  New Balance: Rs." << fixed << setprecision(2) << account.getBalance() << "\n";
    }

    void handleTransfer(Account &account)
    {
        cout << "\n===================== TRANSFER MONEY =====================\n";
        long long receiverAccNo = (long long)readDoubleInput("Enter Receiver's Account Number: ");

        if (!accountExists(receiverAccNo))
        {
            cout << "  [Error] Receiver account does not exist.\n";
            return;
        }

        Account receiver;
        if (!Account::loadFromFile(receiverAccNo, receiver))
        {
            cout << "  [Error] Unable to load receiver account.\n";
            return;
        }

        double amount = readDoubleInput("Enter amount to transfer: Rs.");

        string errorMsg;
        if (!account.transferTo(receiver, amount, errorMsg))
        {
            cout << "  [Error] " << errorMsg << "\n";
            return;
        }

        cout << "  [Success] Rs." << fixed << setprecision(2) << amount
             << " transferred to account " << receiverAccNo << ".\n";
        cout << "  New Balance: Rs." << fixed << setprecision(2) << account.getBalance() << "\n";
    }

    void handleChangePassword(Account &account)
    {
        cout << "\n===================== CHANGE PASSWORD =====================\n";
        string oldPass = readNonEmptyLine("Enter Current Password: ");

        if (oldPass != account.getCustomer().getPassword())
        {
            cout << "  [Error] Current password is incorrect.\n";
            return;
        }

        string newPass;
        while (true)
        {
            newPass = readNonEmptyLine("Enter New Password (min 8 chars, upper/lower/digit/special): ");
            if (!Customer::validatePassword(newPass))
            {
                cout << "  [Error] Password does not meet requirements.\n";
                continue;
            }
            string confirm = readNonEmptyLine("Confirm New Password: ");
            if (confirm != newPass)
            {
                cout << "  [Error] Passwords do not match.\n";
                continue;
            }
            break;
        }

        account.getCustomer().setPassword(newPass);

        if (!account.saveToFile())
        {
            cout << "  [Error] Failed to save new password.\n";
            return;
        }

        cout << "  [Success] Password changed successfully.\n";
    }

    void handleUpdateProfile(Account &account)
    {
        bool updating = true;
        while (updating)
        {
            cout << "\n===================== UPDATE PROFILE =====================\n";
            cout << "1. Update Name\n";
            cout << "2. Update Phone\n";
            cout << "3. Update Email\n";
            cout << "4. Update Address\n";
            cout << "5. Back to Dashboard\n";
            int choice = readIntInput("Enter your choice: ");

            switch (choice)
            {
                case 1:
                    account.getCustomer().setFullName(readNonEmptyLine("Enter New Full Name: "));
                    if (account.saveToFile()) cout << "  [Success] Name updated.\n";
                    break;

                case 2:
                {
                    string phone;
                    while (true)
                    {
                        phone = readNonEmptyLine("Enter New Phone Number (10 digits): ");
                        if (!Customer::validatePhone(phone))
                        {
                            cout << "  [Error] Phone number must be exactly 10 digits.\n";
                            continue;
                        }
                        break;
                    }
                    account.getCustomer().setPhone(phone);
                    if (account.saveToFile()) cout << "  [Success] Phone updated.\n";
                    break;
                }

                case 3:
                {
                    string email;
                    while (true)
                    {
                        email = readNonEmptyLine("Enter New Email: ");
                        if (!Customer::validateEmail(email))
                        {
                            cout << "  [Error] Invalid email format.\n";
                            continue;
                        }
                        break;
                    }
                    account.getCustomer().setEmail(email);
                    if (account.saveToFile()) cout << "  [Success] Email updated.\n";
                    break;
                }

                case 4:
                    account.getCustomer().setAddress(readNonEmptyLine("Enter New Address: "));
                    if (account.saveToFile()) cout << "  [Success] Address updated.\n";
                    break;

                case 5:
                    updating = false;
                    break;

                default:
                    cout << "  [Error] Invalid choice. Please try again.\n";
            }
        }
    }

public:
    // ---------------- Top level program loop ----------------
    void run()
    {
        createFolders();

        bool running = true;
        while (running)
        {
            cout << "\n#############################################\n";
            cout << "#         BANK MANAGEMENT SYSTEM             #\n";
            cout << "#############################################\n";
            cout << "1. Create Account\n";
            cout << "2. Login\n";
            cout << "3. Exit\n";

            int choice = readIntInput("Enter your choice: ");

            switch (choice)
            {
                case 1: registerCustomer(); break;
                case 2: login(); break;
                case 3:
                    cout << "\nThank you for using the Bank Management System. Goodbye!\n";
                    running = false;
                    break;
                default:
                    cout << "  [Error] Invalid choice. Please select 1, 2 or 3.\n";
            }
        }
    }
};

#endif // BANK_H