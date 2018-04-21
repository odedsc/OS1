/* 046267 Computer Architecture - Spring 2017 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"

/* globals */

SIM_coreState coreState;
bool STALL; // the MIPS needs Stall
bool READ; // true if there is an instruction which needs for MEM stage more then one clock cycle
bool branch_flag;
int32_t branch_add;

typedef struct {
    int32_t data;  // The main data of the cmd, such as ALU result for ADD, branch condition.
    int32_t pc; 	  // The pc of this specific command (+4)
    int32_t address;  // Addresses, such as the jump address for branch, and mem address for load, store
} StageData;

StageData Stages[SIM_PIPELINE_DEPTH]; // this array is used to store the MidData of each stage in the pipeline
		   /*implementation of the 5 stages in pipeline MIPS processor
		   */

void SIM_FETCH() {
	if (!STALL && !READ) {
		SIM_MemInstRead(coreState.pc, &(coreState.pipeStageState[0]).cmd);
		(coreState.pipeStageState[0]).src1Val = 0, (coreState.pipeStageState[0]).src2Val = 0;
		coreState.pc = coreState.pc + 4;
	}

}

void SIM_DECODE(){
	if (!STALL && !READ){
		(coreState.pipeStageState[1]).cmd=(coreState.pipeStageState[0]).cmd;
	}

	if (split_regfile){
		(coreState.pipeStageState[1]).src1Val = coreState.regFile[((coreState.pipeStageState[1]).cmd).src1] ;
	}

	if (((coreState.pipeStageState[1]).cmd).isSrc2Imm){
		(coreState.pipeStageState[1]).src2Val=((coreState.pipeStageState[1]).cmd).src2;
	}
	else{
		(coreState.pipeStageState[1]).src2Val=coreState.regFile[((coreState.pipeStageState[1]).cmd).src2];
}
	
void EmptyCertainStage (PipeStageState* stage)
{
	stage->src1Val = 0;
	stage->src2Val = 0;
	(&(stage->cmd))->opcode = 0;
	(&(stage->cmd))->src1 = 0;
	(&(stage->cmd))->src2 = 0;
	(&(stage->cmd))->isSrc2Imm = 0;
	(&(stage->cmd))->dst = 0;
}


void Execute()
{
	if (READ) //Stall the core if waiting for data memory reading
	{
		return;
	}
	if(STALL)
	{
		EmptyCertainStage(&(core->pipeStageState[2]));
		/*
		loadForwardStall = false;
		*/
	}
	else
	{
		PipeStageState ID = coreState ->pipeStageState[1] ;
		PipeStageState EXE = coreState ->pipeStageState[2] ;
		SIM_cmd IDcmd = ID.cmd ;
		EXE = ID; //Move the state to the following stage.
		int32_t dstVal = coreState ->regFile[(IDcmd).dst];
		switch (IDcmd.opcode)
		{
			case (CMD_NOP):
			{
				EmptyCertainStage(&(EXE));
			} break;
			case (CMD_ADD):
			{
				Stages[2].data = (ID).src1Val + (ID).src2Val;
			} break;
			case (CMD_SUB):
			{
				Stages[2].data = (ID).src1Val - (ID).src2Val;
			} break;
			case (CMD_ADDI):
			{
				Stages[2].data = (ID).src1Val + (ID).src2Val;
			} break;
			case (CMD_SUBI):
			{
				Stages[2].data = (ID).src1Val - (ID).src2Val;
			} break;
			case (CMD_LOAD):
			{
				Stages[2].address = (ID).src1Val + (ID).src2Val;
			} break;
			case (CMD_STORE):
			{
				Stages[2].address = dstVal + (ID).src2Val;
			} break;
			case (CMD_BR):
			{
				Stages[2].address =  Stages[1].pc + dstVal;
			} break;
			case (CMD_BRNEQ):
			{
				Stages[2].address =  Stages[1].pc + dstVal;
			} break;
			case (CMD_BREQ):
			{
				Stages[2].address =  Stages[1].pc + dstVal;
				Stages[2].data = (EXE).src1Val - (EXE).src2Val;
			} break;
			case (CMD_HALT):{
			} break;
			default : {}
		}
		core->pipeStageState[2] = EXE ;
	}
}
	
void SIM_MEMORY(){
	PipeStageState EXE = coreState.pipeStageState[2] ;
	PipeStageState MEM = coreState.pipeStageState[3] ;

	if (!READ){
		MEM = EXE;
		Stages[3]=Stages[2];
	}
	switch(MEM.cmd.opcode){
		case (CMD_NOP):
		{
			EmptyCertainStage(&(MEM));
		} break;
		case (CMD_ADD):
		{
		} break;
		case (CMD_SUB):
		{
		} break;
		case (CMD_ADDI):
		{
		} break;
		case (CMD_SUBI):
		{
		} break;
		case (CMD_LOAD):
		{
			if(SIM_MemDataRead(Stages[3].address, &((Stages[3]).data)) == -1) {
				READ=true;
			}
			else{
				READ=false;
			}
		} break;
		case (CMD_STORE):
		{
			SIM_MemDataWrite(Stages[3].address, (EXE).src1Val);
		} break;
		case (CMD_BR):
		{
			branch_add = Stages[3].address;
			branch_flag = true;
		} break;
		case (CMD_BRNEQ):
		{
			if ((stages[3]).data != 0) {
				branch_add = Stages[3].address;
				branch_flag = true;
			}
		} break;
		case (CMD_BREQ):
		{
			if ((stages[3]).data != 0) {
				branch_add = Stages[3].address;
				branch_flag = true;
			}
		} break;
		case (CMD_HALT):{
		} break;
		default : {}
	}
	coreState.pipeStageState[3] = MEM ;
}

	
void SIM_WB()
{
	if (!READ)
	{
		EmptyCertainStage(&(coreState->pipeStageState[4]));
	}
	else
	{
		PipeStageState MEM = coreState->pipeStageState[3] ;
		PipeStageState WB = coreState->pipeStageState[4] ;
		SIM_cmd MEMcmd = MEM.cmd ;
		WB = MEM; //Move the state to the following stage.
		stages[4] = stages[3];
		switch (MEMcmd.opcode)
		{
			case (CMD_NOP):
			{
				SIM_EmptyStage(&(WB));
			} break;
			case (CMD_LOAD):
			case (CMD_SUB):
			case (CMD_ADD):
			{
				if ((MEMcmd).dst != 0)
				{
					coreState->regFile[(MEMcmd).dst] = stages[4].data;
				}
			} break;
			case (CMD_HALT):{
			} break;
			default : {}
		}
		/*
		if (loadForwardStall)
		{
			coreState->pipeStageState[1].src1Val = coreState->pipeStageState[3].src1Val;
			coreState->pipeStageState[1].src2Val = coreState->pipeStageState[3].src2Val;
			loadForwardStall = false;
		}
		coreState->pipeStageState[4] = WB ;
		*/
	}
}
	
/*! SIM_CoreReset: Reset the processor core simulator machine to start new simulation
Use this API to initialize the processor core simulator's data structures.
The simulator machine must complete this call with these requirements met:
- PC = 0  (entry point for a program is at address 0)
- All the register file is cleared (all registers hold 0)
- The value of IF is the instuction in address 0x0
\returns 0 on success. <0 in case of initialization failure.
*/
int SIM_CoreReset(void) {

	for (int i=0; i< SIM_REGFILE_SIZE; i++) {
		core->regFile[i] = 0;
	}

	for (int i=0; i< SIM_PIPELINE_DEPTH; i++) {
		EmptyCertainStage(&(coreState.pipeStageState[i]));
		States[i].address = 0;
		States[i].pc = 0;
		States[i].data = 0;
	}



	coreState->pc = 0;
	READ = false;
	STALL = false;
	branch_flag = false;
	branch_add = 0;

	SIM_FETCH();

	return 0;
}

/*! SIM_CoreClkTick: Update the core simulator's state given one clock cycle.
This function is expected to update the core pipeline given a clock cycle event.
*/
void SIM_CoreClkTick() {

}

/*! SIM_CoreGetState: Return the current core (pipeline) internal state
curState: The returned current pipeline state
The function will return the state of the pipe at the end of a cycle
*/
void SIM_CoreGetState(SIM_coreState *curState) {
}

