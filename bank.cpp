#include "bank.h"

const char* COMMISSION_MASSAGE = "Bank: commissions of %d % were charged, the bank gained %d $ from account %d\n";

Bank::Bank()
{
	account_list       (),
	account_iterator   (),
	write_mutex        (),
	read_mutex         (),
	Bank_mutex         (),
	read_counter       (0),
	balance            (0),
	plog_file          (fopen("log.txt", "w")),
	ATM_array          (NULL),
}

void Bank::Init(int num_of_ATM)
{
	ATM_array = new ATM*[num_of_ATM];
}

Bank::~Bank()
{
	fclose(plog_file);

	pthread_mutex_destroy(&write_mutex);
	pthread_mutex_destroy(&read_mutex);
	pthread_mutex_destroy(&Bank_mutex);

	while (account_list.size()>0)
	{
		Account* pAccout = account_list.back();
		account_list.pop_back();
		delete pAccout;
	}
}

void Bank::Read_Lock()
{
	pthread_mutex_lock(&read_mutex);
	if (read_counter++ == 1)
	{
		pthread_mutex_lock(&write_mutex);
	}
	pthread_mutex_unlock(&read_mutex);
}

void Bank::Read_Unlock()
{
	pthread_mutex_lock(&read_mutex);
	if (read_counter-- == 0)
	{
		pthread_mutex_unlock(&write_mutex);
	}
	pthread_mutex_unlock(&read_mutex);}

void Bank::Write_Lock()
{
	pthread_mutex_lock(&write_mutex);
}

void Bank::Write_Unlock()
{
	pthread_mutex_unlock(&write_mutex);
}

Account* Bank::pAccount(int account_id)
{
	for (account_iterator i = account_list.begin(); i != account_list.end(); ++i)
	{
		if ((*i)->ID() == account_id)
		{
			return *i;
		}
	}
	return NULL;
}

void Bank::Add_Account(Account* pAccount)
{
	for (account_iterator i = account_list.begin(); i != account_list.end(); i++)
	{
		if ((*i)->ID() > pAccount->ID())
		{
			account_list.insert(i, pAccount);
			return;
		}
	}
	account_list.push_back(pAccount);
}

void Bank::Delete_Account(Account* pAccount)
{
	for (account_iterator i = account_list.begin(); i != account_list.end(); ++i)
	{
		if (*i == pAccount)
		{
			account_list.erase(i);
			return;
		}
	}
}

void Bank::Commission()
{
	float commission_ratio = ((rand() % 3) + 2) / 100.0;
	for (account_iterator i = account_list.begin(); i != account_list.end(); ++i)
	{
		if (isVIP_ == false)
		{
			Account& account = **i;

			account.Lock();
			int commission = static_cast<int>(account.Get_Balance() * commission_ratio + 0.5);

			account.Withdraw(commission);
			balance += commission;

			int account_id = account.ID();
			account.Unlock();

			fprintf(plog_file, COMMISSION_MASSAGE, static_cast<int>(commission_ratio*100), commission, account_id);
		}

	}
}

void Bank::Log()
{
	Write_Lock();

	printf("\033[2J");
	printf("\033[1;1H");

	printf("Current Bank Status\n");
	for (account_iterator i = account_list.begin(); i != account_list.end(); ++i)
	{
		Account& account = **i;
		printf("Account %d: Balance - %d $ , Account Password - %d\n",
				account.ID(), account.Get_Balance(), account.Password());
	}
	printf("The Bank Has %d $\n", balance);

	Write_Unlock();
}
