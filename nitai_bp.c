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


// Increase prediction by one (for instance, WT -> ST)
State increase_State(prediction curr)
{
	if (curr < ST)
	{
		return (curr+1);
	}
	else return curr;
}

// Decrease prediction by one (for instance, WT -> WNT)
State decrease_State(prediction curr)
{
	if (curr > SNT)
	{
		return (curr-1);
	}
	else return curr;
}

/*update the history value by checking whether we jumped or not*/
int update_history(int history, bool taken){
	if (taken){
		return (((history << 1) + 1 ) % ( 1 << history_Size));
	}
	else{
		return ((history << 1 ) % ( 1 << history_Size));
	}

}



void BP_update(uint32_t pc, uint32_t targetPc, bool taken, uint32_t pred_dst)
{

	//Update the BTB:
	int BTB_id = ((pc / 4) % btb_Size);
	BTB_entry curr_line = (BTB[BTB_id]);

	 // change the BTB entry if it's empty or doesn't match the tag
	bool same_Tag = false;
	int pc_tag = ((pc / 4) % (1 << tag_Size));
	if (curr_line.tag == pc_tag){
		same_Tag = true;
	}
	curr_line.target = targetPc; //update the BTB entry
	if ((curr_line.tag == -1) || (same_Tag==false)){
		//Add BTB entry:
		curr_line.tag = ((pc / 4) % (1 << tag_Size));

		//update the relevant state machine:
		//get the current history value:
		unsigned int hist_id=0;
		if (isGlobHist){
			hist_id=calc_hist(pc, BTB_id);
		}
		else{
			if (isGlobTable){
				if (shared_type==SHARE_LSB){
					hist_id=(pc/4)%(1 << history_Size);
				}
				else if (shared_type==SHARE_MID){
					hist_id=(((pc/4)%(1 << history_Size))) << 14;
				}
			}
		}
		//update the relevant state machine:
		if (isGlobTable)
		{
			//update the global states machine
			Global_States[hist_id] = (taken? increase_State(Global_States[hist_id]) : decrease_State(Global_States[hist_id]))
		}
		else
		{
			//initialize local states machine
			for (int i=0; i < States_max_size; i++)
			{
				Local_States[BTB_id][i] = WNT;
			}

			//Update the relevant state
			Local_States[BTB_id][hist_id] = (taken? increase_State(Local_States[BTB_id][hist_id]) : decrease_State(Local_States[BTB_id][hist_id]))
		}

		//update history value:
		if (isGlobHist)
		{
			global_BHR=update_history(global_BHR, taken);
		}
		else
		{
			curr_line.LocalHistory = taken ? 1 : 0;
		}

	}
	else
	{ //BTB entry already contains this branch:

		//calculate history value:
		unsigned int hist_id=calc_hist(pc, BTB_id);

		//update the relevant state machine:
		if (isGlobTable)
		{
			//update the global states machine
			Global_States[hist_id] = (taken? increase_State(Global_States[hist_id]) : decrease_State(Global_States[hist_id]))
		}
		else
		{
			//Update the specific state in the relevant state machine:
			Local_States[BTB_id][hist_id] = (taken? increase_State(Local_States[BTB_id][hist_id]) : decrease_State(Local_States[BTB_id][hist_id]))
		}

		//update the relevant history value:
		if (isGlobHist)
		{
			global_BHR=update_history(global_BHR,taken);
		}
		else
		{
			curr_line.LocalHistory = update_history(curr_line.LocalHistory, taken);
		}

	}

	/*curr_stats update*/
	curr_stats.br_num++;
	if (taken){
		if (targetPc!=pred_dst){
			curr_stats.flush_num++;
		}
	}
	else if (!taken){
		if ((pc+4)!=pred_dst){
			curr_stats.flush_num++;
		}
	}
	/*****************/
	return;
}
void BP_GetStats(SIM_stats *curStats) {
	*curStats=curr_stats;
	return;
}
