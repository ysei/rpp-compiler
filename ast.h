#ifndef _AST_H__
#define _AST_H__

#include <map>
#include <stack>
#include <vector>
#include <iostream>

#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>

#include "astnodevisitor.h"

class llvm::Module;

struct CodeGenBlock
{
    llvm::BasicBlock * block;
    std::map<std::string, llvm::Value *> locals;
    std::map<std::string, llvm::Value *> arguments;
};

class CodeGenContext
{
public:
    CodeGenContext();

    std::map<std::string, llvm::Value *>& locals()
    {
        return blocks.top()->locals;
    }

    std::map<std::string, llvm::Value *>& arguments()
    {
        return blocks.top()->arguments;
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

    virtual void accept(ASTNodeVisitor * visitor) = 0;
};

class ExpressionNode : public ASTNode
{
public:
    enum Type {
        Int = 1,
        Float = 2,
        Invalid = 4
    };

    ExpressionNode() : m_type(ExpressionNode::Invalid) {}

    virtual ExpressionNode::Type type() const {
        return m_type;
    }

    std::string typeString() const {
        switch(m_type) {
        case Int:
            return "int";
        case Float:
            return "float";
        case Invalid:
            return "Invalid";
        }

        return "Unknown";
    }

    void setType(const ExpressionNode::Type type) {
        m_type = type;
    }

    void setType(const std::string& typeString) {
        m_type = expressionTypeFromString(typeString);
    }

protected:
    ExpressionNode::Type combineTypes(ExpressionNode::Type left, ExpressionNode::Type right) const {
        int combined = left | right;
        switch(combined) {
        case 1:
            return ExpressionNode::Int;

        case 2:
        case 3:
            return ExpressionNode::Float;

        default:
            return ExpressionNode::Invalid;
        }
    }

    ExpressionNode::Type expressionTypeFromString(const std::string& typeString) {
        if(typeString == "int") {
            return ExpressionNode::Int;
        } else if(typeString == "float") {
            return ExpressionNode::Float;
        }

        return ExpressionNode::Invalid;
    }

protected:
    ExpressionNode::Type m_type;
};

class StatementNode : public ASTNode
{
};

class IntegerNode : public ExpressionNode
{
public:
    IntegerNode(const IntegerNode& other) : value(other.value) {
        setType(ExpressionNode::Int);
    }

    explicit IntegerNode(int val) : value(val) {
        setType(ExpressionNode::Int);
    }

    virtual llvm::Value * codeGen(CodeGenContext& context);

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
    }

private:
    int value;
};

class FloatNode : public ExpressionNode
{
public:
    FloatNode(const FloatNode& other) : value(other.value) {
        setType(ExpressionNode::Float);
    }

    explicit FloatNode(float val) : value(val) {
        setType(ExpressionNode::Float);
    }

    virtual llvm::Value * codeGen(CodeGenContext& context);

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
    }

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

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
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

    virtual ExpressionNode::Type getType() const {
        ExpressionNode::Type leftType = leftExpression->type();
        ExpressionNode::Type rightType = rightExpression->type();
        return combineTypes(leftType, rightType);
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(leftExpression);
        visitor->visit(rightExpression);
        visitor->visit(this);
    }
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

    virtual void accept(ASTNodeVisitor * visitor) {
        for(std::vector<ExpressionNode *>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++) {
            visitor->visit(*iter);
        }

        visitor->visit(this);
    }

private:
    IdentifierNode id;
    std::vector<ExpressionNode *> arguments;
};

class BlockStatement : public StatementNode
{
public:
    BlockStatement(std::vector<ASTNode *>& statements) : statements(statements) {}

    virtual llvm::Value * codeGen(CodeGenContext& context);

    virtual void accept(ASTNodeVisitor * visitor) {
        for(std::vector<ASTNode *>::const_iterator iter = statements.begin(); iter != statements.end(); iter++) {
            visitor->visit(*iter);
        }

        visitor->visit(this);
    }

private:
    std::vector<ASTNode *> statements;
};

class VariableDeclaration : public StatementNode
{
public:
    VariableDeclaration(IdentifierNode* type, IdentifierNode * id) : type(type), id(id) {}
    VariableDeclaration(IdentifierNode* type, IdentifierNode * id, ExpressionNode * assingmentExpr)
        : type(type), id(id), assingmentExpr(assingmentExpr) {

        varName()->setType(varType()->idName());
    }

    IdentifierNode * varType() const {
        return type;
    }

    IdentifierNode * varName() const {
        return id;
    }

    virtual llvm::Value * codeGen(CodeGenContext& context);

    virtual void accept(ASTNodeVisitor * visitor) {
        if(assingmentExpr) {
            visitor->visit(assingmentExpr);
        }

        visitor->visit(this);
    }

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

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
        visitor->visit(block);
    }

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

    virtual void accept(ASTNodeVisitor * visitor) {
        if(expression) {
            visitor->visit(expression);
        }

        visitor->visit(this);
    }

private:
    ExpressionNode * expression;
};

class AssignmentExpression : public ExpressionNode
{
public:
    AssignmentExpression(IdentifierNode * id, ExpressionNode * rightExpression) : id(id), rightExpression(rightExpression) {}

    virtual llvm::Value * codeGen(CodeGenContext &context);

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(rightExpression);
        visitor->visit(this);
    }

private:
    IdentifierNode * id;
    ExpressionNode * rightExpression;
};

class Program : public ASTNode
{
public:
    Program(std::vector<MethodDeclaration *> methods) : m_methods(methods) {}

    virtual llvm::Value * codeGen(CodeGenContext &context);

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
        for(std::vector<MethodDeclaration *>::const_iterator iter = m_methods.begin(); iter != m_methods.end(); iter++) {
            visitor->visit(*iter);
        }
    }

private:
    std::vector<MethodDeclaration *> m_methods;
};

#endif
