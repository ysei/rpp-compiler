all: parser

OBJS = parser.o     \
       lexer.c      \
       main.cpp     \
       
LLVM_MODULES = core jit native

CPPFLAGS = `llvm-config --cppflags $(LLVM_MODULES)`
LDFLAGS = `llvm-config --ldflags $(LLVM_MODULES)`
LIBS = 'llvm-config --libs $(LLVM_MODULES)`

clean:
    $(RM) -rf parser.cpp parser.hpp parser lexer.cpp $(OBJS)

parser.cpp: parser.y
    bison -d -o $@ $^

parser.h: parser.cpp

lexer.cpp: lexer.l parser.h
    flex -o $@ $^

%.o: %.cpp
    g++ -c $(CPPFLAGS) -o $@ $<

parser: $(OBJS)
    g++ -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
