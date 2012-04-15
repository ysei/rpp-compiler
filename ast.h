#ifndef _AST_H__
#define _AST_H__

#include <map>
#include <stack>
#include <vector>
#include <iostream>
#include <algorithm>

#include <llvm/Function.h>
#include <llvm/DerivedTypes.h>

#include "astnodevisitor.h"

#include "utils.h"

class llvm::Module;

class ASTNode
{
public:
    virtual ~ASTNode() {}
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

    static ExpressionNode::Type expressionTypeFromString(const std::string& typeString) {
        if(typeString == "int") {
            return ExpressionNode::Int;
        } else if(typeString == "float") {
            return ExpressionNode::Float;
        }

        return ExpressionNode::Invalid;
    }

    static ExpressionNode::Type combineTypes(ExpressionNode::Type left, ExpressionNode::Type right) {
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

    static std::string toString(ExpressionNode::Type type) {
        switch(type) {
        case Int:
            return "int";
        case Float:
            return "float";
        case Invalid:
            return "Invalid";
        }

        return "Unknown";
    }

    ExpressionNode() : m_type(ExpressionNode::Invalid) {}

    virtual ExpressionNode::Type type() const {
        return m_type;
    }

    std::string typeString() const {
        return toString(m_type);
    }

    void setType(const ExpressionNode::Type type) {
        m_type = type;
    }

    void setType(const std::string& typeString) {
        m_type = expressionTypeFromString(typeString);
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
    IntegerNode(const IntegerNode& other) : m_value(other.m_value) {
        setType(ExpressionNode::Int);
    }

    explicit IntegerNode(int val) : m_value(val) {
        setType(ExpressionNode::Int);
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
    }

    int value() const {
        return m_value;
    }

private:
    int m_value;
};

class FloatNode : public ExpressionNode
{
public:
    FloatNode(const FloatNode& other) : m_value(other.m_value) {
        setType(ExpressionNode::Float);
    }

    explicit FloatNode(float val) : m_value(val) {
        setType(ExpressionNode::Float);
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
    }

    float value() const {
        return m_value;
    }

private:
    float m_value;
};

class IdentifierNode : public ExpressionNode
{
public:
    IdentifierNode(const IdentifierNode& other) : m_name(other.m_name), m_declared(false) {}
    virtual ~IdentifierNode() {}
    explicit IdentifierNode(const std::string& name) : m_name(name) {}

    std::string name() const {
        return m_name;
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);
    }

    ExpressionNode::Type asType() const {
        return ExpressionNode::expressionTypeFromString(m_name);
    }

    void setDeclared() {
        m_declared = true;
    }

    bool isDeclared() {
        return m_declared;
    }

private:
    std::string m_name;
    bool m_declared;
};

class BinaryOpExpression : public ExpressionNode
{
public:
    BinaryOpExpression(ExpressionNode * leftExpression, ExpressionNode * rightExpression, const std::string& op)
        : m_leftExpression(leftExpression), m_rightExpression(rightExpression), m_op(op) {}

    virtual ~BinaryOpExpression() {
        delete m_leftExpression;
        delete m_rightExpression;
    }

    virtual ExpressionNode::Type getType() const {
        ExpressionNode::Type leftType = m_leftExpression->type();
        ExpressionNode::Type rightType = m_rightExpression->type();
        return combineTypes(leftType, rightType);
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        m_leftExpression->accept(visitor);
        m_rightExpression->accept(visitor);
        visitor->visit(this);
    }

    const std::string op() const {
        return m_op;
    }

    const ExpressionNode * left() const {
        return m_leftExpression;
    }

    const ExpressionNode * right() const {
        return m_rightExpression;
    }

private:
    ExpressionNode * m_leftExpression;
    ExpressionNode * m_rightExpression;
    const std::string m_op;
};

class MethodCallExpression : public ExpressionNode
{
public:
    MethodCallExpression(IdentifierNode * id, std::vector<ExpressionNode *> & arguments)
        : id(id), arguments(arguments)
    {
    }

    virtual ~MethodCallExpression() {
        delete id;
        std::for_each(arguments.begin(), arguments.end(), rpp::delete_pointer_element<ExpressionNode *>);
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        for(std::vector<ExpressionNode *>::const_iterator iter = arguments.begin(); iter != arguments.end(); iter++) {
            ExpressionNode * exprNode = *iter;
            exprNode->accept(visitor);
        }

        visitor->visit(this);
    }

    IdentifierNode * methodName() const {
        return id;
    }

private:
    IdentifierNode * id;
    std::vector<ExpressionNode *> arguments;
};

class BlockStatement : public StatementNode
{
public:
    BlockStatement(std::vector<ASTNode *>& statements) : statements(statements) {}

    virtual ~BlockStatement() {
        std::for_each(statements.begin(), statements.end(), rpp::delete_pointer_element<ASTNode *>);
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visit(this);

        for(std::vector<ASTNode *>::const_iterator iter = statements.begin(); iter != statements.end(); iter++) {
            ASTNode * node = *iter;
            node->accept(visitor);
        }
    }

private:
    std::vector<ASTNode *> statements;
};

class VariableDeclaration : public StatementNode
{
public:
    VariableDeclaration(IdentifierNode* type, IdentifierNode * id, ExpressionNode * assingmentExpr = NULL)
        : m_type(type), m_id(id), m_assingmentExpr(assingmentExpr) {

        varName()->setType(varType()->name());
    }

    virtual ~VariableDeclaration() {
        delete m_type;
        delete m_id;
        delete m_assingmentExpr;
    }

    IdentifierNode * varType() const {
        return m_type;
    }

    IdentifierNode * varName() const {
        return m_id;
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        if(m_assingmentExpr) {
            m_assingmentExpr->accept(visitor);
        }

        visitor->visit(this);
    }

private:
    IdentifierNode * m_type;
    IdentifierNode * m_id;
    ExpressionNode * m_assingmentExpr;
};

class MethodDeclaration : public StatementNode
{
public:
    MethodDeclaration(IdentifierNode * name, IdentifierNode * returnType,
                      std::vector<VariableDeclaration *> & arguments, BlockStatement * block)
        : m_name(name), m_returnType(returnType), m_arguments(arguments), m_block(block) {}

    virtual ~MethodDeclaration() {
        std::for_each(m_arguments.begin(), m_arguments.end(), rpp::delete_pointer_element<VariableDeclaration*>);
        delete m_name;
        delete m_returnType;
        delete m_block;
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visitEnter(this);
        m_block->accept(visitor);
        visitor->visitExit(this);
    }

    const std::vector<VariableDeclaration *> & arguments() const {
        return m_arguments;
    }

    const std::string name() const {
        return m_name->name();
    }

    ExpressionNode::Type returnType() const {
        return m_returnType->asType();
    }

private:
    IdentifierNode * m_name;
    IdentifierNode * m_returnType;
    std::vector<VariableDeclaration *> m_arguments;
    BlockStatement * m_block;
};

class ReturnStatement : public StatementNode
{
public:
    ReturnStatement(ExpressionNode * expresion) : m_expression(expresion), m_returnType(ExpressionNode::Int) {}

    virtual ~ReturnStatement() {
        delete m_expression;
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        if(m_expression) {
            m_expression->accept(visitor);
        }

        visitor->visit(this);
    }

    ExpressionNode * expression() {
        return m_expression;
    }


    ExpressionNode::Type returnType() const {
        return m_returnType;
    }

    void setReturnType(ExpressionNode::Type returnType) {
        m_returnType = returnType;
    }

private:
    ExpressionNode * m_expression;
    ExpressionNode::Type m_returnType;
};

class AssignmentExpression : public ExpressionNode
{
public:
    AssignmentExpression(IdentifierNode * id, ExpressionNode * rightExpression) : m_id(id), m_rightExpression(rightExpression) {
        std::cout << "Assign: " << this << std::endl;
    }

    virtual ~AssignmentExpression() {
        delete m_id;
        delete m_rightExpression;
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        m_rightExpression->accept(visitor);
        visitor->visit(this);
    }

    const std::string varName() {
        return m_id->name();
    }

    IdentifierNode * id() {
        return m_id;
    }

private:
    IdentifierNode * m_id;
    ExpressionNode * m_rightExpression;
};

class Program : public ASTNode
{
public:
    Program(std::vector<MethodDeclaration *> methods) : m_methods(methods) {}

    virtual ~Program() {
        std::for_each(m_methods.begin(), m_methods.end(), rpp::delete_pointer_element<MethodDeclaration *>);
    }

    virtual void accept(ASTNodeVisitor * visitor) {
        visitor->visitEnter(this);
        for(std::vector<MethodDeclaration *>::const_iterator iter = m_methods.begin(); iter != m_methods.end(); iter++) {
            MethodDeclaration * methodDecl = *iter;
            methodDecl->accept(visitor);
        }
        visitor->visitExit(this);
    }

private:
    std::vector<MethodDeclaration *> m_methods;
};

#endif
