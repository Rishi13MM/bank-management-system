# Bank Management System

This is a C++ program for a **Bank Management System** that allows users to interact with a database and perform various banking operations. The system supports opening accounts, depositing and withdrawing money, checking balances, updating account details, and closing accounts.

## Features

- **Open New Account**: Create a new account with an initial deposit.
- **Deposit Money**: Add money to an existing account.
- **Withdraw Money**: Withdraw money from an account if the balance is sufficient.
- **Balance Enquiry**: View the current balance of an account.
- **Show All Accounts**: Display all bank accounts stored in the database.
- **Update Account Details**: Modify account name or status (active, inactive, closed).
- **Close Account**: Delete an account from the database.

## Technologies Used

- **C++**: Used to implement the system logic.
- **MySQL**: Database for storing account and transaction data.
- **Exception Handling**: Custom exceptions such as `InvalidAmount`, `InvalidAccountNo`, and `InsufficientBalance` are used to handle errors.

## File Structure

- **`BankManagementSystem.cpp`**: Main code file that interacts with the user and performs banking operations.
- **`BankManagementSystem.h`**: Header file defining the `Bank` class and database connection details.
- **`CustomException.h`**: Contains custom exception classes for handling specific errors.

## Prerequisites

Before running the program, ensure the following are installed:

- **C++ Compiler**: Such as GCC or Visual Studio.
- **MySQL Database**: MySQL must be installed.
- **MySQL Connector for C++**: Ensure the MySQL C++ connector is set up for database connectivity.

## Conclusion

The **Bank Management System** is a simple application designed to manage bank accounts and transactions. It utilizes MySQL for data storage and handles errors gracefully. Thank you for reviewing this project!