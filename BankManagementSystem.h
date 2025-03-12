#pragma once

#include "CustomException.h"

#include <iostream>
#include <iomanip>

#include <cppconn/driver.h>
#include <cppconn/connection.h>
#include <cppconn/prepared_statement.h>
#include <cppconn/resultset.h>
#include <cppconn/resultset_metadata.h>
#include <cppconn/exception.h>

#include <vector>

using namespace std;

struct ConnectionDetails {
	string hostName;
	string userName;
	string password;
	vector<string> tableNames;
	string databaseName;
	void setDetails(string hostName, string userName, string password, string databaseName, vector<string> tableNames) {
		this->hostName = hostName;
		this->userName = userName;
		this->password = password;
		this->databaseName = databaseName;
		this->tableNames = tableNames;
	}
};


class Bank {
private:
	//Create account data type
	struct Account {
		string name;
		double balance;
		Account(string,  double);
	};

	//Properties
	sql::Connection* conn;
	ConnectionDetails details;

	void insertRecord(Account);
	double getBalance(long int);
public:
	//Methods
	Bank();
	void openAccount();
	void updateAccountDetails();
	void deposit();
	void withdraw();
	void balanceEnquiry();
	void showAllAccounts();
	void closeAccount();
};

Bank::Account::Account(string name, double balance) {
	this->name = name;
	this->balance = balance;
}

Bank::Bank() {

	//Connection details
	vector<string> tableNames = { "account", "transection" };
	details.setDetails("localhost", "root", "1234", "bank_management", tableNames);

	//Create connection
	sql::Driver* driver = get_driver_instance();
	conn = driver->connect(details.hostName, details.userName, details.password);

	if (conn==nullptr) {
		throw sql::SQLException("Unable to estabhished the connection to mysql server.\n");
	}

	//Prepare sql statement
	string query = "CREATE DATABASE IF NOT EXISTS " + details.databaseName;
	sql::PreparedStatement* pstmt = conn->prepareStatement(query);
	pstmt->executeUpdate();
	delete pstmt;

	//Connect to the specified mysql database
	conn->setSchema(details.databaseName);

	//Create account table
	query = "CREATE TABLE IF NOT EXISTS " + details.tableNames[0] + " (account_no BIGINT AUTO_INCREMENT, holder_name VARCHAR(255), balance DOUBLE, status enum(\"active\",\"inactive\",\"closed\") NOT NULL, opening_date DATE, closing_date DATE, CONSTRAINT pk_acc PRIMARY KEY(account_no))";
	pstmt = conn->prepareStatement(query);
	pstmt->executeUpdate();
	delete pstmt;

	query = "ALTER TABLE " + details.tableNames[0] + " AUTO_INCREMENT=1000";
	pstmt = conn->prepareStatement(query);
	pstmt->executeUpdate();
	delete pstmt;

	//Create transecton table
	query = "CREATE TABLE IF NOT EXISTS " + details.tableNames[1] + " (id BIGINT AUTO_INCREMENT, amount DOUBLE, type ENUM ('credit','debit','enquiry'), account_no BIGINT, _date DATE, CONSTRAINT pk_trans PRIMARY KEY(id), CONSTRAINT fk_acc_trans FOREIGN KEY(account_no) REFERENCES " + details.tableNames[0] + "(account_no))";
	pstmt = conn->prepareStatement(query);
	pstmt->executeUpdate();
	delete pstmt;

	query = "ALTER TABLE "+	details.tableNames[1]+" AUTO_INCREMENT=1";
	pstmt = conn->prepareStatement(query);
	pstmt->executeUpdate();
	delete pstmt;
}

void Bank::insertRecord(Account account) {
	string query = "INSERT INTO " + details.tableNames[0] + " (holder_name, balance, status, opening_date) VALUES (?,?,?,?)";
	sql::PreparedStatement* pstmt = conn->prepareStatement(query);
	pstmt->setString(1, account.name);
	pstmt->setDouble(2, account.balance);
	pstmt->setString(3, "active");

	//Need to implement date system
	pstmt->setDateTime(4, "2025-03-10");

	pstmt->executeUpdate();
	delete pstmt;
}

void Bank::openAccount() {
	string name;
	double initBalance;

	cout << "\nEnter details to open new account";
	cout << "\nA/C holder name: ";
	getline(cin, name);
	cout << "Initial deposit amount: ";
	cin >> initBalance;

	if (initBalance < 0) {
		throw InvalidAmount("Initial deposit amount must be zero or more.\n");
	}

	Account newAccount(name, initBalance);
	insertRecord(newAccount);
}


void Bank::updateAccountDetails() {
	long int accountNo;
	int choice, status;

	cout << "\nEnter details of account which you want to update";
	cout << "\nAccount no: ";
	cin >> accountNo;

	if (accountNo < 1000) {
		throw InvalidAccountNo();
	}

	cout << "\nWhich data you want to update";
	cout << "\n1. Update name";
	cout << "\n2. Update status";
	cout << "\nEnter your choice ";
	cin >> choice;

	string query, value;

	switch (choice) {
	case 1:
		cout << "\nNew name: ";
		cin.ignore();
		getline(cin, value);
		query = "UPDATE " + details.tableNames[0] + " SET holder_name=? WHERE account_no = ?";
		break;
	case 2:
		cout << "\nSelect status\n1. active\n2.inactive\n3.closed\nEnter your status choice ";
		cin >> status;

		if (status == 1) {
			value = "active";
		}
		else if (status == 2) {
			value = "inactive";
		}
		else if (status == 3) {
			value = "closed";
		}
		else {
			query = "";
			cout << "\nInvalid choice, operation fail!\n";
		}
		query = "UPDATE " + details.tableNames[0] + " SET status=? WHERE account_no = ?";
		break;
	default:
		query = "";
		cout << "\nInvalid choice, operation fail!\n";
		break;
	}

	if (query != "") {
		sql::PreparedStatement* pstmt = conn->prepareStatement(query);
		pstmt->setString(1, value);
		pstmt->setInt64(2, accountNo);
		pstmt->executeUpdate();
		delete pstmt;
	}
}

void Bank::deposit() {
	long int accountNo;
	double amount;

	cout << "\nEnter deposit amount: ";
	cin >> amount;

	cout << "Enter receiver account no: ";
	cin >> accountNo;

	if (amount < 1) {
		throw InvalidAmount();
	}

	if (accountNo < 1000) {
		throw InvalidAccountNo();
	}

	string query;
	sql::PreparedStatement* pstmt;

	//Update balance in account table
	query = "UPDATE " + details.tableNames[0] + " SET balance=balance+? WHERE account_no=?";
	pstmt = conn->prepareStatement(query);
	pstmt->setDouble(1, amount);
	pstmt->setInt64(2, accountNo);

	int rowEffected = pstmt->executeUpdate();

	if (rowEffected == 0) {
		cout << "\nAccount not found!";
	}

	delete pstmt;

	//add transection 
	query = "INSERT INTO " + details.tableNames[1] + "(amount, type, account_no, _date) VALUES (?, ?, ?, ?)";
	pstmt = conn->prepareStatement(query);
	pstmt->setDouble(1, amount);
	pstmt->setString(2, "credit");
	pstmt->setInt64(3, accountNo);
	pstmt->setDateTime(4, "2025-03-10");
	pstmt->executeUpdate();

	delete pstmt;
}

void Bank::withdraw() {
	long int accountNo;
	double amount;

	cout << "\nEnter withdraw amount: ";
	cin >> amount;

	if (amount < 1) {
		throw InvalidAmount();
	}

	cout << "Enter your account no: ";
	cin >> accountNo;

	if (accountNo < 1000) {
		throw InvalidAccountNo();
	}

	double newBalance = getBalance(accountNo) - amount;

	if (newBalance < 0) {
		throw InsufficientBalance();
	}

	//Update balance from record in account table
	string query = "UPDATE " + details.tableNames[0] + " SET balance=? WHERE account_no=?";
	sql::PreparedStatement* pstmt = conn->prepareStatement(query);
	pstmt->setDouble(1, newBalance);
	pstmt->setInt64(2, accountNo);
	pstmt->executeUpdate();
	delete pstmt;

	//Add a transection record in transection table
	query = "INSERT INTO " + details.tableNames[1] + "(amount, type, account_no, _date) VALUES (?, ?, ?, ?)";
	pstmt = conn->prepareStatement(query);
	pstmt->setDouble(1, amount);
	pstmt->setString(2, "debit");
	pstmt->setInt64(3, accountNo);
	pstmt->setDateTime(4, "2025-03-10");
	pstmt->executeUpdate();
	delete pstmt;
}

double Bank::getBalance(int long accountNo) {

	//Get balance from the given account no
	string query = "SELECT balance FROM " + details.tableNames[0] + " WHERE account_no=?";
	sql::PreparedStatement* pstmt = conn->prepareStatement(query);
	pstmt->setInt64(1, accountNo);
	sql::ResultSet* res = pstmt->executeQuery();

	//Throw exception if no record found
	if (res->rowsCount() == 0) {
		throw AccountNotFound();
	}

	res->next();
	double balance = res->getDouble("balance");

	//Release memory
	delete pstmt;
	delete res;

	return balance;
}

void Bank::balanceEnquiry() {
	long int accountNo;
	double amount;

	cout << "\nEnter account: ";
	cin >> accountNo;

	if (accountNo < 1000) {
		throw InvalidAccountNo();
	}

	amount = getBalance(accountNo);
	cout << "Available balance: "<<amount<<endl;

	//Update transection table
	string query = "INSERT INTO " + details.tableNames[1] + " (amount, type, account_no, _date) VALUES (?,?,?,?)";
	sql::PreparedStatement* pstmt = conn->prepareStatement(query);
	pstmt->setDouble(1, amount);
	pstmt->setString(2, "enquiry");
	pstmt->setInt64(3, accountNo);
	pstmt->setDateTime(4, "2025-03-11");
	pstmt->executeUpdate();
	delete pstmt;
}

void Bank::closeAccount() {
	long int accountNo;

	cout << "\nEnter details of account which you want to close";
	cout << "\nAccount no: ";
	cin >> accountNo;

	if (accountNo < 1000) {
		throw InvalidAccountNo();
	}

	string query;
	sql::PreparedStatement* pstmt;

	//Delete associated records from transection table
	query = "DELETE FROM " + details.tableNames[1] + " WHERE account_no=?";
	pstmt = conn->prepareStatement(query);
	pstmt->setInt64(1, accountNo);
	pstmt->executeUpdate();
	delete pstmt;

	//Delete a account from account table
	query = "DELETE FROM " + details.tableNames[0] + " WHERE account_no=?";
	pstmt = conn->prepareStatement(query);
	pstmt->setInt64(1, accountNo);
	pstmt->executeUpdate();
	delete pstmt;
}

void Bank::showAllAccounts() {
	string query = "SELECT * FROM " + details.tableNames[0];
	sql::PreparedStatement* pstmt = conn->prepareStatement(query);
	sql::ResultSet* res = pstmt->executeQuery();

	sql::ResultSetMetaData* resMD = res->getMetaData();
	int count=resMD->getColumnCount();

	cout << "\n";

	for (int i = 1; i <= count; i++)
	{
		cout <<setw(20)<<resMD->getColumnName(i);
	}

	cout << "\n";

	for (int i = 1; i <= count*20; i++)
	{
		cout << "-";
	}

	cout << "\n";

	while(res->next()){
		for (int i = 1; i <= count; i++)
		{
			string name = resMD->getColumnTypeName(i);

			if (name == "BIGINT") {
				cout << setw(20) << res->getInt64(i);
			}
			else if (name == "VARCHAR") {
				cout << setw(20) << res->getString(i);
			}
			else if (name == "DOUBLE") {
				cout << setw(20) << res->getDouble(i);
			}
			else if (name == "ENUM") {
				cout << setw(20) << res->getString(i);
			}
			else if (name == "DATE") {
				cout << setw(20) << res->getString(i);
			}
		}
		cout << endl;
	}
}