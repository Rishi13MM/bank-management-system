#pragma once
#include <iostream>
#include <conio.h>

#include "BankManagementSystem.h"

using namespace std;

void printChar(char, int,char='\n', char = '\n');

int main()
{
    Bank bank;
    int choice;
    string systemName = "BANK MANAGEMENT SYSTEM               ";
    string optionTitle = "Service Menu";

    while(1){
        system("cls");

        printChar('#', 50);
        cout << setw(50) << systemName;
        printChar('#', 50);

        cout << "\n" << optionTitle;
        printChar('_', optionTitle.length() * 1);
        cout << "\n1. Open new account\n2. Update account details\n3. Deposit your money\n4. Withdraw your money\n5. Balance enquiry\n6. Show all accounts\n7. Close account\n8. Exit";

        cout << "\n\nEnter your choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            cin.ignore();
            bank.openAccount();
            break;
        case 2:
            bank.updateAccountDetails();
            break;
        case 3:
            bank.deposit();
            break;
        case 4:
            bank.withdraw();
            break;
        case 5:
            bank.balanceEnquiry();
            break;
        case 6:
            bank.showAllAccounts();
            break;
        case 7:
            bank.closeAccount();
            break;
        case 8:
            exit(0);
        default:
            cout << "Invalid choice (1 - 7), try again.";
        }

        cout << "\nPress any key to continue... ";
        _getch();
    }
    return 0;
}

void printChar(char ch, int num,char start, char end)
{
    cout <<start;
    for (size_t i = 0; i < num; i++)
    {
        cout << ch;
    }
    cout << end;
}
