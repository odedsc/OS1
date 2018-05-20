/*
 * ATM.h
 *
 *  Created on: May 19, 2018
 *      Author: os
 */

#ifndef ATM_H_
#define ATM_H_

#include "bank.h"
#include "account.h"

class ATM{
private:
	int id_;
	pthread_mutex_t mutex_;

public:
	ATM(int id);
	~ATM();

	void open_account(int id, int password, int balance);
	void turn_VIP(int account, int password);
	void deposit(int account, int password, int amount);
	void withdraw(int account, int password, int amount);
	void balance_inquiry(int account, int password);
	void transfer(int account, int password, int target_account, int amount);

};



#endif /* ATM_H_ */