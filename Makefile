CC = llvm-g++
LLVM_MODULES = core jit native
CPPFLAGS = `llvm-config --cppflags $(LLVM_MODULES)`
LDFLAGS = `llvm-config --ldflags $(LLVM_MODULES)`
LIBS = `llvm-config --libs $(LLVM_MODULES)`

all: rpp

SRCS = parser.cpp \
    lexer.cpp \
    ast.cpp \
    global.cpp \
    main.cpp

OBJDIR = .obj
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)

DEPDIR = .deps
df = $(DEPDIR)/$(*F)

DEPS := $(OBJS:%.o=$(DEPDIR)/%.d)

-include $(DEPS)

clean:
	$(RM) -rf parser.cpp parser.hpp rpp lexer.cpp $(OBJS) 

parser.cpp: parser.y
	@echo "Generating parser"
	@bison -d -o $@ $^
    
parser.hpp: parser.cpp

lexer.cpp: lexer.l parser.hpp
	@echo "Generating lexer"
	@flex -o $@ $^

dirtree:
	@mkdir -p .obj

$(OBJDIR)/%.o: %.cpp | dirtree
	@echo "Compiling " $<
	@$(CC) -MD -c $(CPPFLAGS) -o $@ $<

rpp: $(OBJS)
	@echo "Linking..."
	@$(CC) -o $@ $(LDFLAGS) $(OBJS) $(LIBS)
