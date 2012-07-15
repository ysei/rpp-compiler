CC = clang
LLVM_MODULES = core jit native
CPPFLAGS = -I/usr/local/include -D_GNU_SOURCE -D__STDC_CONSTANT_MACROS -D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS -g
LDFLAGS = `/usr/local/bin/llvm-config --ldflags $(LLVM_MODULES)`
LIBS = `/usr/local/bin/llvm-config --libs $(LLVM_MODULES)` -lantlr3c -lstdc++
ANTLR = antlr

all: ../bin/rpp

SRCS = main.cpp ast.cpp astbuilder.cpp rppcompiler.cpp semanticanalysis.cpp llvmcodegen.cpp bindmethods.cpp

OBJDIR = .obj
OBJS := $(SRCS:%.cpp=$(OBJDIR)/%.o)

DEPS := $(OBJS:%.o=%.d)

-include $(DEPS)

clean:
	$(RM) -rf ../bin/rpp rppLexer.h rppLexer.c rppParser.h rppParser.c rpp.tokens $(OBJDIR)/*.o $(OBJDIR)/*.d

rppLexer.h: rpp.g
	@echo "Generating lexer and parser from " $<
	@antlr rpp.g

rppLexer.c : rppLexer.h

rppParser.c : rppLexer.h

rppParser.h : rppLexer.h

dirtree:
	@mkdir -p .obj

$(OBJDIR)/%.o: %.cpp rppParser.h | dirtree
	@echo "Compiling " $<
	$(CC) -MD -c $(CPPFLAGS) -o $@ $<

$(OBJDIR)/rppLexer.o: rppLexer.h rppLexer.c
	@echo "Compiling " $<
	@$(CC) -c $(CPPFLAGS) -o $(OBJDIR)/rppLexer.o rppLexer.c

$(OBJDIR)/rppParser.o: rppParser.h rppParser.c
	@echo "Compiling " $<
	@$(CC) -c $(CPPFLAGS) -o $(OBJDIR)/rppParser.o rppParser.c

../bin/rpp: $(OBJS) $(OBJDIR)/rppLexer.o $(OBJDIR)/rppParser.o
	@echo "Linking.." $@
	@$(CC) -o $@ $(LDFLAGS) $(OBJS) $(OBJDIR)/rppLexer.o $(OBJDIR)/rppParser.o  $(LIBS)
