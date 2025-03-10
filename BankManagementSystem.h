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
	private:
		string name;
		long int accountNo;
		double balance;
	public:
		Account(string, long int, double);
	};

	//Properties
	sql::Connection* conn;
	ConnectionDetails details;
public:
	//Methods
	Bank();
};

Bank::Account::Account(string name, long int accountNo, double balance) {
	this->accountNo = accountNo;
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
	query = "CREATE TABLE IF NOT EXISTS " + details.tableNames[0] + " (account_no BIGINT, holder_name VARCHAR(255), balance DOUBLE, status enum(\"active\",\"inactive\",\"closed\") NOT NULL, opening_date DATE, closing_date DATE, CONSTRAINT pk_acc PRIMARY KEY(account_no))";
	pstmt = conn->prepareStatement(query);
	pstmt->executeUpdate();
	delete pstmt;

	//Create transecton table
	query = "CREATE TABLE IF NOT EXISTS " + details.tableNames[1] + " (id BIGINT, amount DOUBLE, type ENUM ('credit','debit','enquiry'), account_no BIGINT, _date DATE, CONSTRAINT pk_trans PRIMARY KEY(id), CONSTRAINT fk_acc_trans FOREIGN KEY(account_no) REFERENCES " + details.tableNames[0] + "(account_no))";
	pstmt = conn->prepareStatement(query);
	pstmt->executeUpdate();
	delete pstmt;
}