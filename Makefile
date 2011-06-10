CC = g++
LLVM_MODULES = core jit native
CPPFLAGS = `/usr/local/bin/llvm-config --cppflags $(LLVM_MODULES)` -g
LDFLAGS = `/usr/local/bin/llvm-config --ldflags $(LLVM_MODULES)`
LIBS = `/usr/local/bin/llvm-config --libs $(LLVM_MODULES)` -lantlr3c
ANTLR = antlr

all: rpp

SRCS = ast.cpp \
    global.cpp \
    main.cpp

OBJDIR = .obj
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)

DEPS := $(OBJS:%.o=%.d)

-include $(DEPS)

clean:
	$(RM) -rf rpp rppLexer.h rppLexer.c rppParser.h rppParser.c rpp.tokens $(OBJDIR)/*.o $(OBJDIR)/*.d

rppLexer.h: rpp.g
	@echo "Generating lexer and parser from " $<
	@antlr rpp.g

rppLexer.c : rppLexer.h

rppParser.c : rppLexer.h

rppParser.h : rppLexer.h

ast.cpp : rppLexer.h

dirtree:
	@mkdir -p .obj

$(OBJDIR)/%.o: %.cpp rppParser.h | dirtree
	@echo "Compiling " $<
	@$(CC) -MD -c $(CPPFLAGS) -o $@ $<

$(OBJDIR)/rppLexer.o: rppLexer.h rppLexer.c
	@echo "Compiling " $<
	@$(CC) -c $(CPPFLAGS) -o $(OBJDIR)/rppLexer.o rppLexer.c

$(OBJDIR)/rppParser.o: rppParser.h rppParser.c
	@echo "Compiling " $<
	@$(CC) -c $(CPPFLAGS) -o $(OBJDIR)/rppParser.o rppParser.c

rpp: $(OBJS) $(OBJDIR)/rppLexer.o $(OBJDIR)/rppParser.o
	@echo "Linking..."
	@$(CC) -o $@ $(LDFLAGS) $(OBJS) $(OBJDIR)/rppLexer.o $(OBJDIR)/rppParser.o  $(LIBS)
