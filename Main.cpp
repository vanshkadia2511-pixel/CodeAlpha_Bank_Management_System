/*
    =====================================================
    BANK MANAGEMENT SYSTEM (Class-Based / OOP Version)
    File layout:
      Utils.h       - portable file/folder helpers, input readers
      Transaction.h - Transaction class
      Customer.h    - Customer class
      Account.h     - Account class (uses Customer + Transaction)
      Bank.h        - Bank controller class (menus, registration, login)
      main.cpp      - entry point (this file)
    =====================================================
*/

#include "Bank.h"

int main()
{
    Bank bank;   // Create ONE Bank object that runs the whole program
    bank.run();
    return 0;
}