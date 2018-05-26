/* 046267 Computer Architecture - Spring 2017 - HW #3 */
/* Implementation (skeleton)  for the dataflow statistics calculator */

#include "dflow_calc.h"

//a class which includes all the relevant parameters for each command in the progress
class cmdParams{
private:
	int id;
	InstInfo cmd_inf;
	int src1DepInst;
	int src2DepInst;
	int latency;

public:
	cmdParams(int id_, const InstInfo &cmd_inf, int src1DepInst_, int src2DepInst_, int latency_);
	~cmdParams();
	int get_id() {return id;}
	InstInfo& get_cmd_inf() {return cmd_inf;}
	int get_src1DepInst() {return src1DepInst;}
	int get_src2DepInst() {return src2DepInst;}
	int get_latency() {return latency;}
};

cmdParams::cmdParams(int id_, const InstInfo &cmd_inf_, int src1DepInst_, int src2DepInst_, int latency_){
	id=id_;
	cmd_inf.dstIdx=cmd_inf_.dstIdx;
	cmd_inf.opcode=cmd_inf_.opcode;
	cmd_inf.src1Idx=cmd_inf_.src1Idx;
	cmd_inf.src2Idx=cmd_inf_.src2Idx;
	src1DepInst=src1DepInst_;
	src2DepInst=src2DepInst_;
	latency=latency_;
}

ProgCtx analyzeProg(const unsigned int opsLatency[],  InstInfo progTrace[], unsigned int numOfInsts) {
    return PROG_CTX_NULL;
}

void freeProgCtx(ProgCtx ctx) {
}

int getInstDepth(ProgCtx ctx, unsigned int theInst) {
    return -1;
}

int getInstDeps(ProgCtx ctx, unsigned int theInst, int *src1DepInst, int *src2DepInst) {
    return -1;
}

int getProgDepth(ProgCtx ctx) {
    return 0;
}


