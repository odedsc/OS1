/* 046267 Computer Architecture - Spring 2017 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"

/* globals */

SIM_coreState coreState;
bool STALL; // the MIPS needs Stall one cycle
bool READ; // true if there is an instruction which needs for MEM stage more then one clock cycle
bool branch_flag; // true if branch is the current cmd
int32_t branch_add; // the address the mips needs to jump to
int32_t regFile_CCBefore[SIM_REGFILE_SIZE];
bool LOAD_STALL;

typedef struct {
    int32_t data;  // the ALU result
    int32_t pc; 	  // The pc of cmd
    int32_t address;  // address that needs to be kept for cmds like branch
} StageData;

StageData Stages[SIM_PIPELINE_DEPTH]; // stores the relevant data for each stage

/*implementation of the 5 stages in pipeline MIPS processor
  */

void SIM_FETCH() {
	if (!STALL && !READ) {
		SIM_MemInstRead(coreState.pc, &(coreState.pipeStageState[0]).cmd);
		(coreState.pipeStageState[0]).src1Val = 0, (coreState.pipeStageState[0]).src2Val = 0;
		coreState.pc = coreState.pc + 4;
		Stages[0].pc=coreState.pc;
	}

}

void SIM_DECODE(){
	if (!STALL && !READ){
		(coreState.pipeStageState[1]).cmd=(coreState.pipeStageState[0]).cmd;
		Stages[1]=Stages[0];
	}

	if (split_regfile){
		(coreState.pipeStageState[1]).src1Val = coreState.regFile[((coreState.pipeStageState[1]).cmd).src1];
	}
	else{
		(coreState.pipeStageState[1]).src1Val = regFile_CCBefore[((coreState.pipeStageState[1]).cmd).src1];
	}

	if (((coreState.pipeStageState[1]).cmd).isSrc2Imm){
		(coreState.pipeStageState[1]).src2Val=((coreState.pipeStageState[1]).cmd).src2;
	}
	else{
		(coreState.pipeStageState[1]).src2Val=coreState.regFile[((coreState.pipeStageState[1]).cmd).src2];
	}
}


void EmptyCertainStage(PipeStageState* stage)
{
	stage->src1Val = 0;
	stage->src2Val = 0;
	(&(stage->cmd))->opcode = CMD_NOP;
	(&(stage->cmd))->src1 = 0;
	(&(stage->cmd))->src2 = 0;
	(&(stage->cmd))->isSrc2Imm = 0;
	(&(stage->cmd))->dst = 0;
}


void SIM_EXECUTE()
{
	if (READ)
	{
		return;
	}
	if(STALL)
	{
		EmptyCertainStage(&(coreState.pipeStageState[2]));
		LOAD_STALL = false;
	}
	else
	{
		PipeStageState ID = coreState.pipeStageState[1] ;
		PipeStageState EXE = coreState.pipeStageState[2] ;
		EXE = ID;
		int32_t dstVal = coreState.regFile[(ID.cmd).dst];
		switch ((ID.cmd).opcode)
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
		coreState.pipeStageState[2] = EXE ;
		Stages[2].pc=Stages[1].pc;
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
			if ((Stages[3]).data != 0) {
				branch_add = Stages[3].address;
				branch_flag = true;
			}
		} break;
		case (CMD_BREQ):
		{
			if ((Stages[3]).data != 0) {
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
		EmptyCertainStage(&(coreState.pipeStageState[4]));
	}
	else
	{
		PipeStageState MEM = coreState.pipeStageState[3] ;
		PipeStageState WB = coreState.pipeStageState[4] ;
//		SIM_cmd MEMcmd = MEM.cmd ;
		WB = MEM; //Move the state to the following stage.
		Stages[4] = Stages[3];
		switch ((MEM.cmd).opcode)
		{
			case (CMD_NOP):
			{
				EmptyCertainStage(&(WB));
			} break;
			case (CMD_LOAD):
			case (CMD_SUB):
			case (CMD_ADD):
			{
				if ((MEM.cmd).dst != 0)
				{
					coreState.regFile[(MEM.cmd).dst] = Stages[4].data;
				}
			} break;
			case (CMD_HALT):{
			} break;
			default : {}
		}

		if (LOAD_STALL)
		{
			coreState.pipeStageState[1].src1Val = coreState.pipeStageState[3].src1Val;
			coreState.pipeStageState[1].src2Val = coreState.pipeStageState[3].src2Val;
			LOAD_STALL = false;
		}
		coreState.pipeStageState[4] = WB ;

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
		coreState.regFile[i] = 0;
	}

	for (int i=0; i< SIM_PIPELINE_DEPTH; i++) {
		EmptyCertainStage(&(coreState.pipeStageState[i]));
		Stages[i].address = 0;
		Stages[i].pc = 0;
		Stages[i].data = 0;
	}



	coreState.pc = 0;
	READ = false;
	STALL = false;
	branch_flag = false;
	branch_add = 0;

	SIM_FETCH();

	return 0;
}

bool HAZARD_CHECK(){
	SIM_cmd check_cmd=coreState.pipeStageState[1].cmd;
	for (int i=2; i<SIM_PIPELINE_DEPTH; i++){
		if (split_regfile==true && i==SIM_PIPELINE_DEPTH-2) break;
		SIM_cmd curr_cmd=(coreState.pipeStageState[i]).cmd;
		if ((curr_cmd.opcode == CMD_ADD)
				|| (curr_cmd.opcode == CMD_SUB)
				|| (curr_cmd.opcode == CMD_LOAD)
				|| (curr_cmd.opcode == CMD_SUBI)
				|| (curr_cmd.opcode == CMD_ADDI)){
			if ((curr_cmd.dst==check_cmd.src1 && check_cmd.src1!=0)
					|| (curr_cmd.dst==check_cmd.src2 && check_cmd.src2!=0 && check_cmd.isSrc2Imm)){
				if (!((check_cmd.opcode==CMD_BR) || (check_cmd.opcode==CMD_NOP) || (check_cmd.opcode==CMD_HALT))){
					return true;
				}
			}
			else if(curr_cmd.dst==check_cmd.dst){
				if ((check_cmd.opcode==CMD_BR) || (check_cmd.opcode==CMD_BREQ) || (check_cmd.opcode==CMD_BRNEQ)){
					return true;
				}
			}
		}

	}
	return false;
}

void Forwarding(){
	//trying to figure out which kind of forwarding is needed forwarding from EXE stage if EX/MEM.RegisterRd = ID/EX.RegisterRs &&  EX/MEM.RegisterRd = ID/EX.RegisterRt then forward

		if (coreState.pipeStageState[1].cmd.src1 == coreState.pipeStageState[2].cmd.dst
				|| coreState.pipeStageState[1].cmd.src2 == coreState.pipeStageState[2].cmd.dst )
		{
			coreState.pipeStageState[1].src1Val = coreState.pipeStageState[2].src1Val;
			coreState.pipeStageState[1].src2Val = coreState.pipeStageState[2].src2Val;
		}
	//forwarding from MEM (MEM/WB.RegWrite = 1) and MEM/WB.RegisterRd = ID/EX.RegisterRs and (EX/MEM.RegisterRd != ID/EX.RegisterRs or EX/MEM.RegWrite = 0)
		if (coreState.pipeStageState[3].cmd.dst == coreState.pipeStageState[2].cmd.src1 &&
				coreState.pipeStageState[3].cmd.dst != coreState.pipeStageState[1].cmd.src1 &&
				(coreState.pipeStageState[3].cmd.dst == coreState.pipeStageState[2].cmd.src2 &&
						coreState.pipeStageState[3].cmd.dst != coreState.pipeStageState[1].cmd.src2))
		{
			coreState.pipeStageState[1].src1Val = coreState.pipeStageState[3].src1Val;
			coreState.pipeStageState[1].src2Val = coreState.pipeStageState[3].src2Val;
		}
	//cases when can't forward and a stall is needed forwarding from WB to EXE
	if (coreState.pipeStageState[MEMORY].cmd.opcode == CMD_LOAD)
	{
		if ((coreState.pipeStageState[DECODE].cmd.dst == coreState.pipeStageState[MEMORY].cmd.src1) ||
				(coreState.pipeStageState[DECODE].cmd.dst == coreState.pipeStageState[MEMORY].cmd.src2))
		{
			LOAD_STALL = true;
		}
	}
	return;
}

/*! SIM_CoreClkTick: Update the core simulator's state given one clock cycle.
  This function is expected to update the core pipeline given a clock cycle event.
*/
void SIM_CoreClkTick() {
	//Checks about special events in this cycle:
	if (branch_flag==1) {
		coreState.pc = branch_add;
		//Flush
		EmptyCertainStage(&(coreState.pipeStageState[0])); //Empty IF
		EmptyCertainStage(&(coreState.pipeStageState[1])); //Empty ID
		EmptyCertainStage(&(coreState.pipeStageState[2])); //Empty EXE
		branch_flag = false;
	}

	if (HAZARD_CHECK()) {
		if (forwarding) {
			Forwarding();
		}
		else STALL = true;
	} else {
		STALL = false; //Don't stall the pipe because of Data Hazard
	}

	for (int i=0; i< SIM_REGFILE_SIZE; i++) {
		regFile_CCBefore[i] = coreState.regFile[i];
	}

	//Do the process of each stage:
	SIM_WB();
	SIM_MEMORY();
	SIM_EXECUTE();
	SIM_DECODE();
	SIM_FETCH();
	LOAD_STALL = false;

}

/*! SIM_CoreGetState: Return the current core (pipeline) internal state
    curState: The returned current pipeline state
    The function will return the state of the pipe at the end of a cycle
*/
void SIM_CoreGetState(SIM_coreState *curState) {
	curState->pc=coreState.pc-4;

	for (int i=0; i< SIM_PIPELINE_DEPTH; i++) {
		curState->pipeStageState[i] = coreState.pipeStageState[i] ;
	}

	if (split_regfile) {
		for (int i=0; i< SIM_REGFILE_SIZE; i++) {
			curState->regFile[i] = coreState.regFile[i];
		}
	}
	else{
		for (int i=0; i< SIM_REGFILE_SIZE; i++) {
			curState->regFile[i] = regFile_CCBefore[i];
		}
	}
}




