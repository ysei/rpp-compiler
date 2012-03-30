#include "rppcompiler.h"

int main(int /* argc */, char **  /* argv[] */)
{
    RppCompiler compiler;
    compiler.compile("example.rpp");

    return 0;
}
