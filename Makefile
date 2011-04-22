
all: rpp

OBJS = parser.o \
       lexer.o \
       main.o


LLVM_MODULES = core jit native

CPPFLAGS = `llvm-config --cppflags $(LLVM_MODULES)`
LDFLAGS = `llvm-config --ldflags $(LLVM_MODULES)`
LIBS = `llvm-config --libs $(LLVM_MODULES)`

clean:
	$(RM) -rf parser.cpp parser.hpp rpp lexer.cpp $(OBJS)

parser.cpp: parser.y
	bison -d -o $@ $^
    
parser.hpp: parser.cpp

lexer.cpp: lexer.l parser.hpp
	flex -o $@ $^

%.o: %.cpp
	g++ -c $(CPPFLAGS) -o $@ $<

rpp: $(OBJS)
	g++ -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
