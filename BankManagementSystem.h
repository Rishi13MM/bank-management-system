#pragma once

#include <iostream>
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
public:
	//Methods
	Bank();
	void openAccount();
	void updateAccountDetails();
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
		//throw invalidAmount("Initial deposit amount must be greater than zero.\n");
		throw sql::SQLException("Initial deposit amount must be greater than zero.\n");
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
		//throw invalidAccountNo("Account no. must be 1000 or more and unique.\n");
		throw sql::SQLException("Invalid account no. it must be 1000 or more.\n");
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
