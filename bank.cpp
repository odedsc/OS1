/*
File name: bank.cpp
Module's role: implementation of all the functions taking care of checks an all the of bank actions 
Functions: 
Bank();
Bank();
void read_lock();
void write_lock();
void read_unlock();
void write_unlock();
Account* account_exist(int account);
void add_account(Account*);
void remove_account(Account*);
void commission();
void print();
*/

 
#include "bank.h"

//******************************
// Function name: Bank
// Description: constructs a new bank
// Parameters: None
// Returns: None
//******************************

Bank::Bank(){
	rd_count_=0;
	balance_=0;
	file=fopen("log.txt", "w");
	if (pthread_mutex_init(&write_lock_, NULL)){
		perror("mutex initialization failed\n");
	}
	if (pthread_mutex_init(&read_lock_, NULL)){
		perror("mutex initialization failed\n");
	}
}

//******************************
// Function name: ~Bank
// Description: destroy a bank
// Parameters: None
// Returns: None
//******************************
Bank::~Bank(){

	fclose(file);
	if (pthread_mutex_destroy(&write_lock_)){
		perror("mutex destruction failed\n");
	}
	if(pthread_mutex_destroy(&read_lock_)){
		perror("mutex destruction failed\n");
	}

	while (accounts_vec_.size()){
		Account* tmp=accounts_vec_.back();
		accounts_vec_.pop_back();
		delete tmp;
	}

}

//******************************
// Function name: read_lock
// Description: lock down a mutex used for reading
// Parameters: None
// Returns: None
//******************************
void Bank::read_lock(){
	pthread_mutex_lock(&read_lock_);
	rd_count_++;
	if (rd_count_ == 1){
		pthread_mutex_lock(&write_lock_);
	}
	pthread_mutex_unlock(&read_lock_);
}

//******************************
// Function name: write_lock
// Description: lock down a mutex used for writing
// Parameters: None
// Returns: None
//******************************
void Bank::write_lock(){
	pthread_mutex_lock(&write_lock_);
}

//******************************
// Function name: read_unlock
// Description: unlock a mutex used for reading
// Parameters: None
// Returns: None
//******************************
void Bank::read_unlock(){
	pthread_mutex_lock(&read_lock_);
	rd_count_--;
	if (rd_count_ == 0){
		pthread_mutex_unlock(&write_lock_);
	}
	pthread_mutex_unlock(&read_lock_);
}

//******************************
// Function name: write_unlock
// Description: unlock a mutex used for writing
// Parameters: None
// Returns: None
//******************************
void Bank::write_unlock(){
	pthread_mutex_unlock(&write_lock_);
}

//******************************
// Function name: account_exist
// Description: checks if an account with a certain id exists
// Parameters: number of the account
// Returns: a pointer to the account (if exsits)
//******************************
Account* Bank::account_exist(int account){
	for (vector<Account*>::iterator i = accounts_vec_.begin(); i != accounts_vec_.end(); ++i)
	{
		if ((*i)->get_id()==account)
			return *i;
	}
	return NULL;
}

//******************************
// Function name: add_account
// Description: add an account to the bank's list of accounts
// Parameters: pointer to the account
// Returns: None
//******************************
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

//******************************
// Function name: remove_account
// Description: remove an account to the bank's list of accounts
// Parameters: pointer to the account
// Returns: None
//******************************
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

//******************************
// Function name: commission
// Description: charge a random comission (between 2 to 4) from all non VIP accounts and pays same comission to VIP accounts
// Parameters: None
// Returns: None
//******************************
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
			fprintf(file, "Bank: commissions of %d %% were charged, the bank gained %d $ from account %d\n", commission_val_precents, commission, account.get_id());
		}
		else
		{
			fprintf(file, "Bank: commissions of %d %% were charged, the bank gained -%d $ from account %d\n", commission_val_precents, commission, account.get_id());
		}
	}
}

//******************************
// Function name: print
// Description: prints the current status of the main bank account and of all the listed accounts in the bank
// Parameters: None
// Returns: None
//******************************
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
