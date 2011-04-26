#include <iostream>
#include "ast.hpp"
#include "parser.hpp"

using namespace std;

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