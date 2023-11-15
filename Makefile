#---------------------------------------------------------------------------------------------------
# Compiler selection
#---------------------------------------------------------------------------------------------------

CXX = g++

#---------------------------------------------------------------------------------------------------
# Directories
#---------------------------------------------------------------------------------------------------

OBJDIR = build
SRCDIR = src

#---------------------------------------------------------------------------------------------------
# Executables
#---------------------------------------------------------------------------------------------------

EXE = mrclean-greedy

#---------------------------------------------------------------------------------------------------
# Object files
#---------------------------------------------------------------------------------------------------

OBJ = GreedySolver.o Timer.o CleanSolution.o BinContainer.o
ALL_OBJ = $(OBJ) main.o

#---------------------------------------------------------------------------------------------------
# Compiler options
#---------------------------------------------------------------------------------------------------

CXXFLAGS = -O3 -Wall -fPIC -fexceptions -DIL_STD -std=c++11 -fno-strict-aliasing

#---------------------------------------------------------------------------------------------------
all: CXXFLAGS += -DNDEBUG
all: $(EXE)

debug: CXXFLAGS += -g
debug: $(EXE)

mrclean-greedy: $(addprefix $(OBJDIR)/, main.o)
	$(CXX) -o $@ $(addprefix $(OBJDIR)/, $(ALL_OBJ))

$(OBJDIR)/main.o:	$(addprefix $(SRCDIR)/, main.cpp) \
	$(addprefix $(OBJDIR)/, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/CleanSolution.o: $(addprefix $(SRCDIR)/, CleanSolution.cpp CleanSolution.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/GreedySolver.o:	$(addprefix $(SRCDIR)/, GreedySolver.cpp GreedySolver.h) \
													$(addprefix $(SRCDIR)/, MrCleanUtils.h) \
				   								$(addprefix $(OBJDIR)/, BinContainer.o)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/BinContainer.o: $(addprefix $(SRCDIR)/, BinContainer.cpp BinContainer.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/Timer.o: $(addprefix $(SRCDIR)/, Timer.cpp Timer.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#---------------------------------------------------------------------------------------------------
.PHONY: clean
clean:
	/bin/rm -f $(OBJDIR)/*.o
#---------------------------------------------------------------------------------------------------