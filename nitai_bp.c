/* 046267 Computer Architecture - Spring 2016 - HW #2 */
/* This file should hold your implementation of the predictor simulator */

#include "bp_api.h"
#include <stdio.h>

#define BTB_max_size 32
#define States_max_size 256
#define target_size 30

typedef struct {
	int tag;
	int target;
	int LocalHistory;
} BTB_entry;

BTB_entry BTB[BTB_max_size];

/*history states machine*/
typedef enum {SNT=0, WNT=1, WT=2, ST=3} State;

/*local states machine*/
State Local_States[BTB_max_size][States_max_size];

/*global states machine*/
State Global_States[States_max_size];

/*global variables*/

unsigned btb_Size;
unsigned history_Size;
unsigned tag_Size;
bool isGlobHist;
bool isGlobTable;

typedef enum {NOT_SHARE=0, SHARE_LSB=1, SHARE_MID=2} shared;

shared shared_type;

SIM_stats curr_stats;

int global_BHR;

int BP_init(unsigned btbSize, unsigned historySize, unsigned tagSize,
             bool isGlobalHist, bool isGlobalTable, int Shared){

	//save all the given parameters for the other functions
	btb_Size=btbSize;
	history_Size=historySize;
	tag_Size=tagSize;
	isGlobHist=isGlobalHist;
	isGlobTable=isGlobalTable;
	shared_type=Shared;

	//initialize all entries in BTB
	for (int i=0; i< btbSize; i++){
		BTB[i].tag=-1;
		BTB[i].target=-1;
		BTB[i].LocalHistory=0;
	}

	//initialize global states and local states tables
	for (int i=0; i<States_max_size; i++){
		Global_States[i]=WNT;
	}

	for (int i=0; i<States_max_size; i++){
		for (int j=0; j<BTB_max_size; j++){
			Local_States[j][i]=WNT;
		}
	}

	//find the memory size needed according to the table and the history kind
	if (isGlobalTable && isGlobalHist){
		curr_stats.size=btbSize*(target_size+tagSize)+2*(1 << historySize)+ historySize;
	}
	else if (!isGlobalTable && !isGlobalHist){
		curr_stats.size=btbSize*(target_size+tagSize+2*(1 << historySize)+ historySize);
	}
	else if (isGlobalTable && !isGlobalHist){
		curr_stats.size=btbSize*(target_size+tagSize+historySize) +2*(1 << historySize);
	}
	else if (!isGlobalTable && isGlobalHist){
		curr_stats.size=btbSize*(target_size+tagSize+2*(1 << historySize)) + historySize;
	}

	global_BHR=0;

	return 0;
}

/*find the history value in the history table by different parameters*/
int calc_hist(uint32_t pc, int BTB_id){
	if (!isGlobHist){
		if (isGlobTable){
			if (shared_type==SHARE_LSB){
				int pc_toXor=(pc/4)%(1 << history_Size);
				int history_toXor=BTB[BTB_id].LocalHistory;
				return pc_toXor ^ history_toXor;
			}
			else if (shared_type==SHARE_MID){
				int pc_toXor=(((pc/4)%(1 << history_Size))) << 14;
				int history_toXor=BTB[BTB_id].LocalHistory;
				return pc_toXor ^ history_toXor;
			}
		}
		else{
			return BTB[BTB_id].LocalHistory;
		}
	}
	else{
		if (isGlobTable){
			if (shared_type==SHARE_LSB){
				int pc_toXor=(pc/4)%(1 << history_Size);
				return global_BHR ^ pc_toXor;
			}
			else if (shared_type==SHARE_MID){
				int pc_toXor=(((pc/4)%(1 << history_Size))) << 14;
				return global_BHR ^ pc_toXor;
			}
		}
		else{
			return global_BHR;
		}
	}
}


bool BP_predict(uint32_t pc, uint32_t *dst){
	int BTB_id=(pc/4)%(btb_Size);
	if (BTB[BTB_id].tag!=-1 && BTB[BTB_id].target!=-1){
		if (BTB[BTB_id].tag==((pc/4)%(1 << tag_Size))){ // check tag match
			int hist=calc_hist(pc, BTB_id);
			if (isGlobTable){
				if (Global_States[hist]==WT || Global_States[hist]==ST){ // if the history value is ST or WT, predict branch
					*dst=BTB[BTB_id].target;
					return true;
				}
			}
			else if (!isGlobTable){
				if (Local_States[BTB_id][hist]==WT || Local_States[BTB_id][hist]==ST){ // if the history value is ST or WT, predict branch
					*dst=BTB[BTB_id].target;
					return true;
				}
			}
		}
	}
	//if there isn't a branch prediction
	*dst=pc+4;
	return false;
}

void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst){
	return;
}

void BP_GetStats(SIM_stats *curStats) {
	return;
}
