/*
 * ATM.cpp
 *
 *  Created on: May 19, 2018
 *      Author: os
 */
#include "ATM.h"
#include <stdio.h>

ATM::ATM(int id){
	id_=id;
	pthread_mutex_init(&mutex_, NULL);
}

ATM::~ATM(){
	pthread_mutex_destroy(&mutex_);
}

void ATM::open_account(int id, int password, int balance){
	Account* new_acc=new Account(id, password, balance);

	bank.write_lock();

	/*check if already exists*/
	if (bank.account(id)!=NULL){
		bank.write_unlock();
		fprintf(bank.file, "Error %d: Your transaction failed - account with the same id exists", id_);
	}
	else{
		sleep(1);
		bank.add_account(new_acc);
		bank.write_unlock();
		fprintf(bank.file, "%d: New account id is %d with password %d and initial balance %d",id_, id, password, balance);
	}

}

void ATM::turn_VIP(int account, int password){
	bank.read_lock();
	Account* pAccount= bank.pAccount(account);
	if (pAccount==NULL || (pAccount!=NULL && pAccount->password_!=password)){
		fprintf(bank.file, "Error %d: your transaction failed - password for account %d is incorrect", id_, account);
		bank.read_unlock();
	}
	else{
		pAccount->lock();
		sleep(1);
		pAccount->turnVIP();
		pAccount->unlock();
		bank.read_unlock();
	}

}


