#include "ast.h"
#include "astcreator.h"
#include "astnodevisitor.h"

#include "rppcompiler.h"

#include <iostream>

int main(int /* argc */, char **  /* argv[] */)
{
    RppCompiler compiler;
    compiler.compile("example.rpp");

    return 0;
}
