#include "rppLexer.h"
#include "rppParser.h"

#include "ast.h"
#include "astcreator.h"
#include "rppcompiler.h"

using namespace std;

class PrintAST : public ASTNodeVisitor
{
public:
    virtual void visit(ASTNode * node) {
        cout << "Visited ast node" << endl;
    }

    virtual void visit(ExpressionNode * node) {
        cout << "Visited expression" << endl;
    }

    virtual void visit(StatementNode * node) {
        cout << "Visited statement" << endl;
    }

    virtual void visit(FloatNode * node) {
        cout << "Visited float" << endl;
    }

    virtual void visit(IdentifierNode * node) {
        cout << "Visited identifier" << endl;
    }

    virtual void visit(BinaryOpExpression * node) {
        cout << "Visited binary op" << endl;
    }

    virtual void visit(MethodCallExpression * node) {
        cout << "Visited method call" << endl;
    }

    virtual void visit(BlockStatement * node) {
        cout << "Visited block" << endl;
    }

    virtual void visit(VariableDeclaration * node) {
        cout << "Visited var decl" << endl;
    }

    virtual void visit(MethodDeclaration * node) {
        cout << "Visited method decl" << endl;
    }

    virtual void visit(ReturnStatement * node) {
        cout << "Visited return stmt" << endl;
    }

    virtual void visit(AssignmentExpression * node) {
        cout << "Visited assignment" << endl;
    }

    virtual void visit(Program * node) {
        cout << "Visited Program" << endl;
    }
};

RppCompiler::RppCompiler() : m_root(NULL)
{

}

RppCompiler::~RppCompiler()
{

}

void RppCompiler::compile(const char *fileName)
{
    pANTLR3_UINT8 pANTLR3fileName = (pANTLR3_UINT8) fileName;
    pANTLR3_INPUT_STREAM input = antlr3FileStreamNew(pANTLR3fileName, ANTLR3_ENC_8BIT);
    prppLexer lexer = rppLexerNew(input);
    pANTLR3_COMMON_TOKEN_STREAM tokenStream = antlr3CommonTokenStreamSourceNew(ANTLR3_SIZE_HINT, TOKENSOURCE(lexer));
    prppParser parser = rppParserNew(tokenStream);

    rppParser_prog_return prog;
    prog = parser->prog(parser);

    if (parser->pParser->rec->state->errorCount > 0)
    {
        fprintf(stderr, "The parser returned %d errors, tree walking aborted.\n", parser->pParser->rec->state->errorCount);
    } else {
        printf("Tree: %s\n", prog.tree->toStringTree(prog.tree)->chars);
        AstCreator creator;
        m_root = creator.createProgram(prog.tree);
        /*
        CodeGenContext context;
        llvm::Value * program = root->codeGen(context);
        context.printAssembly();
        */
        PrintAST printAST;
        m_root->accept(&printAST);
    }

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->free(input);

}

ASTNode *RppCompiler::rootAstNode() const
{
    return m_root;
}
