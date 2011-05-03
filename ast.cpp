#include <iostream>
#include <vector>
#include <llvm/ExecutionEngine/JIT.h>
#include "ast.hpp"
#include "parser.hpp"

using namespace std;

void CodeGenContext::generateCode(NBlock& root)
{
    cout << "Generating code..." << endl;
    
    vector<const Type*> argTypes;
    FunctionType *ftype = FunctionType::get(Type::getInt32Ty(getGlobalContext()), argTypes, false);
    mainFunction = Function::Create(ftype, GlobalValue::InternalLinkage, "main", module);
    BasicBlock *bblock = BasicBlock::Create(getGlobalContext(), "entry", mainFunction, 0);

    pushBlock(bblock);
    Value * returnedValue = root.codeGen(*this);
    ReturnInst::Create(getGlobalContext(), returnedValue, bblock);
    popBlock();
    
    // Print bytecode
    cout << "Code is generated." << endl;
    PassManager pm;
    pm.add(createPrintModulePass(&outs()));
    pm.run(*module);
}

GenericValue CodeGenContext::runCode()
{
    cout << "Running code..." << endl;
    string error;
    ExecutionEngine *ee = EngineBuilder(module).setErrorStr(&error).setEngineKind(EngineKind::JIT).create();
    if(!error.empty()) {
        cout << "Can't run the code. Reason: " << error << endl;
        return GenericValue();
    } else {
        vector<GenericValue> noargs;
        GenericValue v = ee->runFunction(mainFunction, noargs);
        cout << "Code was run. Return value: " << v.UIntPairVal.first << v.UIntPairVal.second << endl;
        return v;
    }
}

Value * NInteger::codeGen(CodeGenContext& context)
{
    cout << "Creating integer: " << value << endl;
    return ConstantInt::get(Type::getInt32Ty(getGlobalContext()), value, true);
}

Value * NDouble::codeGen(CodeGenContext& context)
{
    cout << "Creating double: " << value << endl;
    return ConstantFP::get(Type::getDoubleTy(getGlobalContext()), value);
}

Value * NBinaryOperator::codeGen(CodeGenContext& context)
{
    cout << "Creating binary operation " << op << endl;
    Instruction::BinaryOps instr;
    switch (op) {
        case TPLUS:
            instr = Instruction::Add;
            break;
        case TMINUS:
            instr = Instruction::Sub;
            break;
        case TMUL:
            instr = Instruction::Mul;
            break;
        case TDIV:
            instr = Instruction::SDiv;
            break;
        default:
            return NULL;
    }
    return BinaryOperator::Create(instr, lhs.codeGen(context),
        rhs.codeGen(context), "", context.currentBlock());
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
    return last;
}

Value * NExpressionStatement::codeGen(CodeGenContext& context)
{
    cout << "Generating code for expression" << endl;
    return expression.codeGen(context);
}
