CC = llvm-g++
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
	$(RM) -rf rpp rppLexer.h rppLexer.c rppParser.h rppParser.c $(OBJDIR)/*.o $(OBJDIR)/*.d

rppLexer.c rppLexer.h rppParser.c rppParser.h: rpp.g
	antlr rpp.g

dirtree:
	@mkdir -p .obj

$(OBJDIR)/%.o: %.cpp | dirtree
	@echo "Compiling " $<
	@$(CC) -MD -c $(CPPFLAGS) -o $@ $<

$(OBJDIR)/rppLexer.o : rppLexer.c rppLexer.h
	@echo "Compiling " $<
	@$(CC) -c $(CPPFLAGS) -o $@ $<

$(OBJDIR)/rppParser.o : rppParser.c rppParser.h
	@echo "Compiling " $<
	@$(CC) -c $(CPPFLAGS) -o $@ $<

rpp: $(OBJS) $(OBJDIR)/rppParser.o $(OBJDIR)/rppLexer.o rppLexer.o
	@echo "Linking..."
	@$(CC) -o $@ $(LDFLAGS) $(OBJS) .obj/rppLexer.o .obj/rppParser.o  $(LIBS)
