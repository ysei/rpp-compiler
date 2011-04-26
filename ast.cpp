#include <iostream>
#include <vector>
#include "ast.hpp"
#include "parser.hpp"

using namespace std;

void CodeGenContext::generateCode(NBlock& root)
{
    cout << "Generating code..." << endl;
    
    vector<const Type*> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getVoidTy(getGlobalContext()), argTypes, false);
    mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

    pushBlock(bblock);
    root.codeGen(*this);
    ReturnInst::Create(getGlobalContext(), bblock);
    popBlock();
    
    // Print bytecode
    cout << "Code is generated." << endl;
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
}

GenericValue CodeGenContext::runCode()
{
    cout << "Running code...\n";
    ExecutionEngine *ee = EngineBuilder(module).create();
    vector<GenericValue> noargs;
    GenericValue v = ee->runFunction(mainFunction, noargs);
    cout << "Code was run.\n";
}

Value * NInteger::codeGen(CodeGenContext& context)
{
    cout << "Creating integer: " << value << endl;
    return ConstantInt::get(Type::getInt64Ty(getGlobalContext()), value, true);
}

Value * NDouble::codeGen(CodeGenContext& context)
{
    cout << "Creating double: " << value << endl;
    return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
}

Value * NBlock::codeGen(CodeGenContext& context)
{
    StatementList::const_iterator it;
    Value *last = NULL;
    for (it = statements.begin(); it != statements.end(); it++) {
        cout << "Generating code for " << "some statement" << endl;
        last = (**it).codeGen(context);
    }
    cout << "Creating block" << endl;
}
