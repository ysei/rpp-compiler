#include <iostream>
#include "rppcompiler.h"

int main(int argc, char ** argv)
{
    if(argc == 1) {
        std::cerr << "Usage:" << std::endl;
        std::cerr << "  ./rpp <file.rpp>" << std::endl;
    } else {
        RppCompiler compiler;
        compiler.compile(argv[1]);
    }
    return 0;
}
