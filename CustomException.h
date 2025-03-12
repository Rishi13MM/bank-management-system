#pragma once
#include <iostream>
#include <cstring>

using namespace std;

class InsufficientBalance :public out_of_range {
private:
	string msg;
public:
	InsufficientBalance(const string& msg) :out_of_range("") {
		this->msg = msg;
	}

	InsufficientBalance():out_of_range("") {
		msg = "Insufficient balance!";
	}
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};

class InvalidAccountNo :public out_of_range {
private:
	string msg;
public:
	InvalidAccountNo():out_of_range("") {
		msg = "Valid account no. should start from 1000.";
	}

	InvalidAccountNo(const string& msg) :out_of_range("") {
		this->msg = msg;
	}

	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};


class InvalidAmount :public out_of_range {
private:
	string msg;

public:
	InvalidAmount(const string& msg): out_of_range("") {
		this->msg = msg;
	}

	InvalidAmount():out_of_range("") {
		msg = "Deposit or Withdraw amount must be more than zero.";
	}
	
	virtual const char* what() const noexcept override {
		return msg.c_str();
	}
};

class AccountNotFound :public runtime_error {
private:
	string msg;
public:
	AccountNotFound(): runtime_error("Account not found!") {}
	AccountNotFound(const string& msg) : runtime_error(msg) {}
};