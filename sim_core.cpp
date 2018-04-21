/* 046267 Computer Architecture - Spring 2017 - HW #1 */
/* This file should hold your implementation of the CPU pipeline core simulator */

#include "sim_api.h"

/* globals */

SIM_coreState coreState;
bool STALL; // the MIPS needs Stall
bool READ; // true if there is an instruction which needs for MEM stage more then one clock cycle

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

/*! SIM_CoreReset: Reset the processor core simulator machine to start new simulation
Use this API to initialize the processor core simulator's data structures.
The simulator machine must complete this call with these requirements met:
- PC = 0  (entry point for a program is at address 0)
- All the register file is cleared (all registers hold 0)
- The value of IF is the instuction in address 0x0
\returns 0 on success. <0 in case of initialization failure.
*/
int SIM_CoreReset(void) {
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

