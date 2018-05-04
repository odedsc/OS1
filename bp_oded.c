/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"

# define max_size_BTB 32
# define max_size_State_Machine 256
# define max_size_target 30

typedef enum { SNT=0, WNT=1, WT=2, ST=3 } prediction;
typedef enum { NONE=0, LSB=1, MID=2 } share_type;


typedef struct {
	int tag;
	unsigned int target;
	unsigned int history;
} BTB_line;

BTB_line BTB [max_size_BTB];
prediction Local_State_Machine [max_size_BTB][max_size_State_Machine];
unsigned int global_history_index;
prediction Global_State_Machine [max_size_BTB]; //Global State Machine
SIM_stats current_stats;

unsigned int BTB_size;
unsigned int history_size;
unsigned int history_size_2_powered;
unsigned int State_Machine_size;
unsigned int tag_size;
unsigned int tag_size_2_powered;
share_type shared;
bool is_global_hist;
bool is_global_table;

unsigned int 2_power (unsigned int size)
{
	unsigned int after_power = 1;
	for (i = 0; i<size; i++)
	{
		after_power = after_power*2;
	}
	return after_power;
}

unsigned int compute_total_size (unsigned int State_Machine_size) {

	unsigned int total_State_Machine_size;
	if (is_global_table)
	{
		total_State_Machine_size = 2*State_Machine_size;
	}
	else
	{
		total_State_Machine_size = (2*State_Machine_size) * BTB_size;
	}

	unsigned int total_history_size;
	if (is_global_hist)
	{
		total_history_size = history_size;
	} else
	{
		total_history_size = history_size * BTB_size;
	}

	unsigned int line_size = max_size_target + tag_size;

	unsigned int total_size = BTB_size*(line_size) + total_history_size + total_State_Machine_size;
	return total_size;

}

int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize,
             bool isGlobalHist, bool isGlobalTable, int Shared)
{
	for (int i=0; i< btbSize; i++)
	{
		BTB[i].tag = -1; //Meaning that the entry is empty
		BTB[i].history = 0;
	}
	if (isGlobalHist)
	{
		global_history_index = 0;
	}

	// init global / local state machines
	unsigned int State_Machine_size = 2_power(historySize);
	if (isGlobalTable)
	{
		for (int i=0; i< State_Machine_size; i++)
		{
			Global_State_Machine[i] = WNT;
		}
	}
	else
	{
		for (int j=0; j< btbSize; j++) {
			for (int i=0; i < State_Machine_size; i++)
			{
				Local_State_Machine[j][i] = WNT;
			}
		}
	}

	BTB_size = btbSize;
	history_size = historySize;
	history_size_2_powered = compute_2_power(history_size);
	tag_size = tagSize;
	tag_size_2_powered = compute_2_power(tag_size);
	is_global_hist = isGlobalHist;
	is_global_table = isGlobalTable;
	shared = Shared;

	current_stats.size = compute_total_size(State_Machine_size);

	return 0 ;
}

bool BP_predict(uint32_t pc, uint32_t *dst){
	return false;
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	return;
}

void BP_GetStats(SIM_stats *curStats) {
	return;
}