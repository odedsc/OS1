/*
 * account.cpp
 *
 *  Created on: May 19, 2018
 *      Author: os
 */

#include "account.h"

Account::Account(int id, int password, int balance){
	id_ = id;
	password_ = password;
	balance_ = balance;
	isVIP_ = false;
	pthread_mutex_init(&mutex_, NULL);
}

Account::~Account(){
	pthread_mutex_destroy(&mutex_);
}

void Account::lock(){
	pthread_mutex_lock(&mutex_);
}

void Account::unlock(){
	pthread_mutex_unlock(&mutex_);
}

void Account::deposit(int amount){
	balance_+=amount;
}

void Account::withdraw(int amount){
	balance_-=amount;
}

void Account::transfer(Account& target, int amount){
	balance_ -= amount;
	target.balance_ += amount;
}


