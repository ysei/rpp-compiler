#include <llvm/Support/TargetSelect.h>
#include <llvm/Value.h>

#include "rppLexer.h"
#include "rppParser.h"

#include "ast.h"
#include "astcreator.h"

int main(int /* argc */, char **  /* argv[] */)
{
    llvm::InitializeNativeTarget();

    pANTLR3_UINT8 fileName = (pANTLR3_UINT8) "example.rpp";
    pANTLR3_INPUT_STREAM input = antlr3FileStreamNew(fileName, ANTLR3_ENC_8BIT);
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
        ASTNode * root = creator.createAst(prog.tree);
        CodeGenContext context;
        llvm::Value * program = root->codeGen(context);
        context.printAssembly();
    }

    parser->free(parser);
    tokenStream->free(tokenStream);
    lexer->free(lexer);
    input->free(input);

    return 0;
}
