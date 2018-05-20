/*
 * bank.cpp
 *
 *  Created on: May 19, 2018
 *      Author: os
 */

#include "bank.h"

Bank::Bank(){
	rd_count_=0;
	balance_=0;
	file=fopen("log.txt", "w");
	pthread_mutex_init(&write_lock_, NULL);
	pthread_mutex_init(&read_lock_, NULL);
}

Bank::~Bank(){

	fclose(file);
	pthread_mutex_destroy(&write_lock_);
	pthread_mutex_destroy(&read_lock_);

	while (accounts_vec_.size()){
		Account* tmp=accounts_vec_.back();
		accounts_vec_.pop_back();
		delete tmp;
	}

}

void Bank::read_lock(){
	pthread_mutex_lock(&read_lock_);
	rd_count_++;
	if (rd_count_ == 1){
		pthread_mutex_lock(&write_lock_);
	}
	pthread_mutex_unlock(&read_lock_);
}

void Bank::write_lock(){
	pthread_mutex_lock(&write_lock_);
}

void Bank::read_unlock(){
	pthread_mutex_lock(&read_lock_);
	rd_count_--;
	if (rd_count_ == 0){
		pthread_mutex_unlock(&write_lock_);
	}
	pthread_mutex_unlock(&read_lock_);
}


void Bank::write_unlock(){
	pthread_mutex_unlock(&write_lock_);
}

Account* Bank::account_exist(int account){
	for (vector<Account*>::iterator i = accounts_vec_.begin(); i != accounts_vec_.end(); ++i)
	{
		if ((*i)->get_id()==account)
			return *i;
	}
	return NULL;
}

void Bank::add_account(Account* pAccount){
	for (vector<Account*>::iterator i = accounts_vec_.begin(); i != accounts_vec_.end(); ++i)
	{
		if ((*i)->get_id() > pAccount->get_id())
		{
			accounts_vec_.insert(i, pAccount);
			return;
		}
	}
	accounts_vec_.push_back(pAccount);
}

void Bank::remove_account(Account* pAccount){
	for (vector<Account*>::iterator i = accounts_vec_.begin(); i != accounts_vec_.end(); ++i)
	{
		if (*i == pAccount)
		{
			accounts_vec_.erase(i);
			return;
		}
	}
}

void Bank::commission(){
	float commission_val = ((rand() % 3) + 2) / 100.0;
	for (vector<Account*>::iterator i = accounts_vec_.begin(); i != accounts_vec_.end(); ++i)
	{
		Account& account = **i;
		account.lock();
		int commission = static_cast<int>(account.balance_inquiry() * commission_val + 0.5);

		if(!account.isVIP_){
			account.withdraw(commission);
			balance_ += commission;
		}
		else{
			account.deposit(commission);
			balance_-= commission;
		}
		account.unlock();

		int commission_val_precents = static_cast<int>(commission_val*100);
		if(!account.isVIP_)
		{
			fprintf(file, "‫ ‪Bank: commissions of %d were charged, the bank gained %d $ from account %d\n", commission_val_precents, commission, account.get_id());
		}
		/*else
		{
			fprintf(file, "‫ ‪Bank: commissions of %d % were charged, the bank gained %d $ from account %d\n", static_cast<int>(commission_val*100), -1*commission, account.get_id);
		}*/
	}
}

void Bank::print(){
	write_lock();

	printf("\033[2J");
	printf("\033[1;1H");

	printf("Current Bank Status\n");
	for (vector<Account*>::iterator i = accounts_vec_.begin(); i != accounts_vec_.end(); ++i)
	{
		Account& account = **i;
		printf("Account %d: Balance - %d $ , Account Password - %d\n",
				account.get_id(), account.balance_inquiry(), account.get_password());
	}
	printf("The Bank Has %d $\n", balance_);

	write_unlock();

}
