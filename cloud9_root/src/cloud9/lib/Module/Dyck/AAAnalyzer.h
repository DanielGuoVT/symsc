/*
 * AAAnalyzer.h
 *
 *  Created on: Jun 17, 2014
 *      Author: sjguo
 */

#ifndef AAANALYZER_H_
#define AAANALYZER_H_

#define DEREF_LABEL -1

#include "DyckGraph.h"
#include "FunctionWrapper.h"
#include <map>
#include <tr1/unordered_map>

#include "llvm/Function.h"
#include "llvm/Operator.h"

using namespace std;

namespace klee {

typedef struct FunctionTypeNode {
	FunctionType * type;
	FunctionTypeNode * root;
	set<Function *> compatibleFuncs;
} FunctionTypeNode;

class AAAnalyzer {
private:
	Module* module;
	AliasAnalysis* aa;
	DyckGraph* dgraph;

private:
	map<Type*, FunctionTypeNode*> functionTyNodeMap;
	set<FunctionTypeNode *> tyroots;

private:
	map<Function*, FunctionWrapper *> wrapped_functions_map;
	set<FunctionWrapper*> wrapped_functions;
	set<Instruction*> unhandled_call_insts; // canary will change all invoke to call, TODO invoke

private:
	bool recordCGInfo;

public:
	AAAnalyzer(Module* m, AliasAnalysis* a, DyckGraph* d, bool CG = false);
	~AAAnalyzer();

	void start_intra_procedure_analysis();
	void end_intra_procedure_analysis();

	void start_inter_procedure_analysis();
	void end_inter_procedure_analysis();

	bool intra_procedure_analysis();
	bool inter_procedure_analysis();

	void printCallGraph(const string& mIdentifier);
	void printFunctionPointersInformation(const string& mIdentifier);

	void getUnhandledCallInstructions(set<Instruction*>* ret);

private:
	void handle_inst(Instruction *inst, FunctionWrapper * parent);
	void handle_instrinsic(Instruction *inst);
	void handle_invoke_call_inst(Value * ret, Value* cv, vector<Value*>* args, FunctionWrapper* parent);
	void handle_lib_invoke_call_inst(Value * ret, Function* cv, vector<Value*>* args, FunctionWrapper* parent);

private:
	bool handle_functions(FunctionWrapper* caller);
	void handle_common_function_call(Call* c, FunctionWrapper* caller, FunctionWrapper* callee);

private:
	int isCompatible(FunctionType * t1, FunctionType * t2);
	set<Function*>* getCompatibleFunctions(FunctionType * fty);
	void initFunctionGroups();
	void destroyFunctionGroups();

private:
	DyckVertex* addField(DyckVertex* val, long fieldIndex, DyckVertex* field);
	DyckVertex* addPtrTo(DyckVertex* address, DyckVertex* val);
	void makeAlias(DyckVertex* x, DyckVertex* y);

	DyckVertex* handle_gep(GEPOperator* gep);
	DyckVertex* wrapValue(Value * v);
};

}
#endif /* AAANALYZER_H_ */
