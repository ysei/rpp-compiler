#ifndef _AST_H__
#define _AST_H__

#include <map>
#include <stack>
#include <vector>
#include <iostream>

#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>

class llvm::Module;

struct CodeGenBlock
{
    llvm::BasicBlock * block;
    std::map<std::string, llvm::Value *> locals;
};

class CodeGenContext
{
public:
    CodeGenContext();

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

    llvm::Module * module();

    void printAssembly();

private:
    std::stack<CodeGenBlock *> blocks;
    llvm::Module * m_module;
};

class ASTNode
{
public:
    virtual ~ASTNode() {}
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
    IntegerNode(const IntegerNode& other) : value(other.value) {}
    explicit IntegerNode(int val) : value(val) {}

    virtual llvm::Value * codeGen(CodeGenContext& context);

private:
    int value;
};

class FloatNode : public ExpressionNode
{
public:
    FloatNode(const FloatNode& other) : value(other.value) {}
    explicit FloatNode(float val) : value(val) {}
    virtual llvm::Value * codeGen(CodeGenContext& context);

private:
    float value;
};

class IdentifierNode : public ExpressionNode
{
public:
    IdentifierNode(const IdentifierNode& other) : name(other.name) {}
    virtual ~IdentifierNode() {}
    explicit IdentifierNode(const std::string& name) : name(name) {}
    virtual llvm::Value * codeGen(CodeGenContext& context);

    std::string idName() const {
        return name;
    }

private:
    std::string name;
};

class BinaryOpExpression : public ExpressionNode
{
public:
    BinaryOpExpression(ExpressionNode * leftExpression, ExpressionNode * rightExpression, const std::string& op)
        : leftExpression(leftExpression), rightExpression(rightExpression), op(op) {}

    virtual llvm::Value * codeGen(CodeGenContext& context);

private:
    ExpressionNode * leftExpression;
    ExpressionNode * rightExpression;
    const std::string op;
};

class MethodCallExpression : public ExpressionNode
{
public:
    MethodCallExpression(IdentifierNode & id, std::vector<ExpressionNode *> & arguments)
        : id(id), arguments(arguments)
    {
    }

    virtual llvm::Value * codeGen(CodeGenContext& context);

private:
    IdentifierNode id;
    std::vector<ExpressionNode *> arguments;
};

class BlockStatement : public StatementNode
{
public:
    BlockStatement(std::vector<ASTNode *>& statements) : statements(statements) {}

    virtual llvm::Value * codeGen(CodeGenContext& context);

private:
    std::vector<ASTNode *> statements;
};

class VariableDeclaration : public StatementNode
{
public:
    VariableDeclaration(IdentifierNode* type, IdentifierNode * id) : type(type), id(id) {}
    VariableDeclaration(IdentifierNode* type, IdentifierNode * id, ExpressionNode * assingmentExpr)
        : type(type), id(id), assingmentExpr(assingmentExpr) {}

    IdentifierNode * varType() const {
        return type;
    }

    virtual llvm::Value * codeGen(CodeGenContext& context);

private:
    IdentifierNode * type;
    IdentifierNode * id;
    ExpressionNode * assingmentExpr;
};

class MethodDeclaration : public StatementNode
{
public:
    MethodDeclaration(IdentifierNode * name, IdentifierNode * returnType,
        std::vector<VariableDeclaration *> & arguments, BlockStatement * block)
        : name(name), returnType(returnType), arguments(arguments), block(block) {}

    virtual llvm::Value * codeGen(CodeGenContext& context);

private:
    IdentifierNode * name;
    IdentifierNode * returnType;
    std::vector<VariableDeclaration *> arguments;
    BlockStatement * block;
};

class ReturnStatement : public StatementNode
{
public:
    ReturnStatement(ExpressionNode * expresion) : expression(expresion) {}

    virtual llvm::Value * codeGen(CodeGenContext &context);

private:
    ExpressionNode * expression;
};

class AssignmentExpression : public ExpressionNode
{
public:
    AssignmentExpression(IdentifierNode * id, ExpressionNode * rightExpression) : id(id), rightExpression(rightExpression) {}

    virtual llvm::Value * codeGen(CodeGenContext &context);

private:
    IdentifierNode * id;
    ExpressionNode * rightExpression;
};

#endif
