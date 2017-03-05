## Source settings 
SRCDIR := src
SRCEXT := cpp
# file with main() function to exclude from test build
SRCMAIN := Application
# Sources for tests
TESTDIR := test

## Build dir for compiler settings
BUILDDIR := build
BUILDEXT := o

## Executable files
TARGET := bin/app
TARGETTEST := bin/test

## Objects for linker
# from SRCDIR
SOURCES := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(BUILDEXT)))
# from TESTDIR
TESTS := $(shell find $(TESTDIR) -type f -name *.$(SRCEXT))
TOBJECTS := $(patsubst $(TESTDIR)/%,$(BUILDDIR)/%,$(TESTS:.$(SRCEXT)=.$(BUILDEXT)))

## Compiler settings
CC := g++

## CFLAGS FOR DEBUG
#CFLAGS := -g -Wall -D_DEBUG -O2 -m64 -std=c++14

## CFLAGS FOR PRODUCTION VERSION
CFLAGS := -g -m64 -O2 -std=c++14

LIB := -L lib -lSDL2
INC := -I include

### DEFAULT
## Compiles TARGET
#
# Usage: make
#    or: make TARGET
#
$(TARGET): $(OBJECTS)
	@echo " Linking...";
	@echo " $(CC) $^ -o $(TARGET) $(LIB)"; $(CC) $^ -o $(TARGET) $(LIB)

## Compiles TARGETTEST
#
# Usage: make TARGETTEST
#
$(TARGETTEST): $(filter-out $(BUILDDIR)/$(SRCMAIN).$(BUILDEXT),$(OBJECTS)) $(TOBJECTS)
	@echo " Linking test.. without source file: $(SRCMAIN).$(SRCEXT)";
	@echo " $(CC) $^ -o $(TARGETTEST) $(LIB)"; $(CC) $^ -o $(TARGETTEST) $(LIB)

## Adds SRCDIR objects to linker
$(BUILDDIR)/%.$(BUILDEXT): $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

## Adds TESTDIR objects to linker
$(BUILDDIR)/%.$(BUILDEXT): $(TESTDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
	@echo " $(CC) $(CFLAGS) $(INC) -c -o $@ $<"; $(CC) $(CFLAGS) $(INC) -c -o $@ $<

## Removes executables and BUILDDIR
# 
# Usage: make clean
#
clean:
	@echo " Cleaning..."; 
	@echo " $(RM) -r $(BUILDDIR) $(TARGET) $(TARGETTEST)"; $(RM) -r $(BUILDDIR) $(TARGET) $(TARGETTEST)

## Performs fresh compilation of TARGET
# (build all dependencies even if not changed)
#
# Usage: make install
#
install: clean $(TARGET)

## Performs fresh compilation of TARGETTEST
# (build all dependencies even if not changed)
#
# Usage: make test
#
test: clean $(TARGETTEST)

.PHONY: clean test
