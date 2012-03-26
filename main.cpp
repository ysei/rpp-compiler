#include <llvm/Support/TargetSelect.h>
#include <llvm/Value.h>
#include <llvm/ExecutionEngine/GenericValue.h>
#include <llvm/LLVMContext.h>
#include <llvm/Constants.h>
#include <llvm/Module.h>
#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>
#include <llvm/support/IRBuilder.h>
#include <llvm/Assembly/PrintModulePass.h>
#include <llvm/PassManager.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/ExecutionEngine/JIT.h>

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include "rppLexer.h"
#include "rppParser.h"
#include <antlr3basetree.h>
#include <antlr3basetreeadaptor.h>
#include <antlr3commontreenodestream.h>
#include <antlr3commontoken.h>
#include <antlr3string.h>

#include <map>
#include <stack>
#include <vector>


/*
def func(int x, int y) : int
    return x + y
end

def main
    k = func(10, 2)
    puts k

    p = 23 + 43 * (10 + k)
    puts k
end
*/

struct CodeGenBlock
{
	llvm::BasicBlock * block;
	std::map<std::string, llvm::Value *> locals;	
};

class CodeGenContext
{
public:
	CodeGenContext()
	{
		module = new llvm::Module("main", llvm::getGlobalContext());
	}
	
	std::map<std::string, llvm::Value *>& locals()
	{
		return blocks.top()->locals;
	}
	
	llvm::BasicBlock * currentBlock()
	{
		return blocks.top()->block;
	}
	
	void pushBlock(llvm::BasicBlock * block)
	{
		blocks.push(new CodeGenBlock());
		blocks.top()->block = block;
	}
	
	void popBlock()
	{
		CodeGenBlock * top = blocks.top();
		blocks.pop();
		delete top;
	}
	
private:
	std::stack<CodeGenBlock *> blocks;
	llvm::Module * module;
};

class ASTNode
{
public:
	virtual ~ASTNode() {};
	virtual llvm::Value * codeGen(CodeGenContext& context) = 0;
};

class ExpressionNode : public ASTNode
{
};

class StatementNode : public ASTNode
{	
};

class IntegerNode : public ExpressionNode
{
public:
	IntegerNode(int val) : value(val)
	{
	}
	
	virtual llvm::Value * codeGen(CodeGenContext& context);
	
private:
	int value;
};

class FloatNode : public ExpressionNode
{
public:
	FloatNode(float val) : value(val) {}
	virtual llvm::Value * codeGen(CodeGenContext& context);
	
private:
	float value;
};

class IdentifierNode : public ExpressionNode
{
public:
	IdentifierNode(const std::string& name) : name(name) {}
	virtual llvm::Value * codeGen(CodeGenContext& context);
	
private:
	std::string name;
};

class BinaryOpExpression : public ExpressionNode
{
public:
	BinaryOpExpression(ExpressionNode& leftExpression, ExpressionNode& rightExpression, const std::string op)
		: leftExpression(leftExpression), rightExpression(rightExpression), op(op) {}
		
	virtual llvm::Value * codeGen(CodeGenContext& context);
	
private:
	ExpressionNode & leftExpression;
	ExpressionNode & rightExpression;
	const std::string op;
};

class MethodCallExpression : public ExpressionNode
{
public:
	MethodCallExpression(IdentifierNode & id, std::vector<ExpressionNode *> arguments)
		: id(id), arguments(arguments)
	{
	}

	virtual llvm::Value * codeGen(CodeGenContext& context);

private:
	IdentifierNode id;
	std::vector<ExpressionNode *> arguments;
};

inline pANTLR3_COMMON_TOKEN getToken(pANTLR3_BASE_TREE node)
{
    return node->getToken(node);
}

inline unsigned char * getTokenText(pANTLR3_COMMON_TOKEN token)
{
    return token->getText(token)->chars;
}

inline unsigned char * getNodeText(pANTLR3_BASE_TREE node)
{
    return getTokenText(getToken(node));
}

inline unsigned int getTokenType(pANTLR3_COMMON_TOKEN token)
{
    return token->type;
}

inline unsigned int getTokenType(pANTLR3_BASE_TREE node)
{
    return getTokenType(getToken(node));
}

inline pANTLR3_BASE_TREE getNodeChild(pANTLR3_BASE_TREE node, unsigned int index)
{
    return (pANTLR3_BASE_TREE) node->getChild(node, index);
}

inline unsigned int getNodeChildCount(pANTLR3_BASE_TREE node)
{
    return node->getChildCount(node);
}

class RppCodeGen
{
public:
    RppCodeGen()
    {
        module = new llvm::Module("main", llvm::getGlobalContext());
        builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
    }

    virtual ~RppCodeGen()
    {
        delete builder;
        delete module;
    }

    llvm::Value * codeGenForFunc(const std::string& funcName, const std::vector<llvm::Type *> arguments, llvm::Type * returnType, pANTLR3_BASE_TREE block)
    {
        llvm::FunctionType *ftype = llvm::FunctionType::get(returnType, makeArrayRef(arguments), false);
        llvm::Function * function = llvm::Function::Create(ftype, llvm::GlobalValue::InternalLinkage, funcName.c_str(), module);
        llvm::BasicBlock *bblock = llvm::BasicBlock::Create(llvm::getGlobalContext(), "entry", function, 0);
        llvm::IRBuilder<> *builder = new llvm::IRBuilder<>(llvm::getGlobalContext());
        builder->SetInsertPoint(bblock);


        printByteCode();
        return NULL;
    }

    void printByteCode()
    {
        std::cout << "Code is generated." << std::endl;
        llvm::PassManager pm;
        pm.add(llvm::createPrintModulePass(&llvm::outs()));
        pm.run(*module);
    }

    void generateFunc(pANTLR3_BASE_TREE node)
    {
        pANTLR3_BASE_TREE nameNode = getNodeChild(node, 0);
        unsigned char * funcName = getNodeText(nameNode);

        printf("Generating function: %s\n", funcName);

        std::vector<llvm::Type *> argTypes;

        populateFuncArgsList(node, argTypes);

        llvm::Type * returnType = getReturnType(node);

        codeGenForFunc(std::string((const char *)funcName), argTypes, returnType, getNodeChild(node, getNodeChildCount(node) - 1));
    }

    void generateCode(pANTLR3_BASE_TREE node)
    {
        pANTLR3_COMMON_TOKEN tok = node->getToken(node);
        switch(tok->type) {
        case FUNC_DEF:
            generateFunc(node);
            break;
        }
    }

protected:
    llvm::Type * getType(const unsigned char * type) {
        std::string typeString((const char *) type);
        if(typeString == std::string("int")) {
            return llvm::Type::getInt32Ty(llvm::getGlobalContext());
        } else if(typeString == std::string("float")) {
            return llvm::Type::getFloatTy(llvm::getGlobalContext());
        } else if(typeString == std::string("void")) {
            return llvm::Type::getVoidTy(llvm::getGlobalContext());
        }

        return NULL;
    }

    void populateFuncArgsList(pANTLR3_BASE_TREE node, std::vector<llvm::Type *>& argTypes)
    {
        if(getNodeChildCount(node) == 1)
            return;

        pANTLR3_BASE_TREE possibleFuncParams = getNodeChild(node, 1);

        if(getTokenType(possibleFuncParams) == FUNC_PARAMS) {
            for(int i = 0; i < getNodeChildCount(possibleFuncParams); i++) {
                pANTLR3_BASE_TREE param = getNodeChild(possibleFuncParams, i);

                pANTLR3_BASE_TREE p = getNodeChild(param, 0);
                unsigned char * text = getNodeText(p);

                argTypes.push_back(getType(text));
                std::cout << getType(text) << std::endl;
            }
        }
    }

    llvm::Type * getReturnType(pANTLR3_BASE_TREE node)
    {
        if(getNodeChildCount(node) == 1) {
            return llvm::Type::getVoidTy(llvm::getGlobalContext());
        }

        pANTLR3_BASE_TREE possibleReturn;
        if(getNodeChildCount(node) == 2) {
            possibleReturn = getNodeChild(node, 1);
        } else {
            possibleReturn = getNodeChild(node, 2);
        }

        if(getTokenType(possibleReturn) == FUNC_RETURN_TYPE) {
            pANTLR3_BASE_TREE typeNode = getNodeChild(possibleReturn, 0);

            return getType(getNodeText(typeNode));
        }

        return llvm::Type::getVoidTy(llvm::getGlobalContext());
    }


private:
    llvm::Module *module;
    llvm::IRBuilder<> *builder;
};

int main(int /* argc */, char **  /* argv[] */)
{
    llvm::InitializeNativeTarget();

    pANTLR3_UINT8 fileName = (pANTLR3_UINT8) "example.rpp";
    pANTLR3_INPUT_STREAM input = antlr3FileStreamNew(fileName, ANTLR3_ENC_8BIT);
    pRppLexer lexer = RppLexerNew(input);
    pANTLR3_COMMON_TOKEN_STREAM tokenStream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    pRppParser parser = RppParserNew(tokenStream);

    RppParser_prog_return prog;
    prog = parser->prog(parser);

    if (parser->pParser->rec->state->errorCount > 0)
    {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", parser->pParser->rec->state->errorCount);
    } else {
        RppCodeGen codeGen;
        codeGen.generateCode(prog.tree);
        // generateCode(prog.tree);
    }

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->free(input);

    return 0;
}
