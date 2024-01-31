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

<<<<<<< HEAD
EXE = greedy_cleaner
=======
EXE = mrclean-greedy
>>>>>>> eb6d5a779d8584000b1c0e0df1c0bd89bf1bafdc

#---------------------------------------------------------------------------------------------------
# Object files
#---------------------------------------------------------------------------------------------------

<<<<<<< HEAD
OBJ = DataContainer.o GreedySolver.o AddRowGreedy.o Timer.o CleanSolution.o
=======
OBJ = GreedySolver.o Timer.o CleanSolution.o BinContainer.o AddRowGreedy.o
>>>>>>> eb6d5a779d8584000b1c0e0df1c0bd89bf1bafdc
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

<<<<<<< HEAD
greedy_cleaner: $(addprefix $(OBJDIR)/, main.o)
	$(CXX) -o $@ $(addprefix $(OBJDIR)/, $(ALL_OBJ))

$(OBJDIR)/main.o:	$(addprefix $(SRCDIR)/, main.cpp) \
									$(addprefix $(OBJDIR)/, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/CleanSolution.o: $(addprefix $(SRCDIR)/, CleanSolution.cpp CleanSolution.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/AddRowGreedy.o:	$(addprefix $(SRCDIR)/, AddRowGreedy.cpp AddRowGreedy.h) \
													$(addprefix $(OBJDIR)/, DataContainer.o)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/GreedySolver.o:	$(addprefix $(SRCDIR)/, GreedySolver.cpp GreedySolver.h) \
													$(addprefix $(SRCDIR)/, MrCleanUtils.h) \
				   								$(addprefix $(OBJDIR)/, DataContainer.o)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/DataContainer.o: $(addprefix $(SRCDIR)/, DataContainer.cpp DataContainer.h)
=======
mrclean-greedy: $(addprefix $(OBJDIR)/, main.o)
	$(CXX) -o $@ $(addprefix $(OBJDIR)/, $(ALL_OBJ))

$(OBJDIR)/main.o:	$(addprefix $(SRCDIR)/, main.cpp) \
	$(addprefix $(OBJDIR)/, $(OBJ))
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/AddRowGreedy.o:	$(addprefix $(SRCDIR)/, AddRowGreedy.cpp AddRowGreedy.h) \
							            $(addprefix $(OBJDIR)/, BinContainer.o)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/CleanSolution.o: $(addprefix $(SRCDIR)/, CleanSolution.cpp CleanSolution.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/GreedySolver.o:	$(addprefix $(SRCDIR)/, GreedySolver.cpp GreedySolver.h) \
													$(addprefix $(SRCDIR)/, MrCleanUtils.h) \
				   								$(addprefix $(OBJDIR)/, BinContainer.o)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<

$(OBJDIR)/BinContainer.o: $(addprefix $(SRCDIR)/, BinContainer.cpp BinContainer.h)
>>>>>>> eb6d5a779d8584000b1c0e0df1c0bd89bf1bafdc
	$(CXX) $(CXXFLAGS) -c -o $@ $<

$(OBJDIR)/Timer.o: $(addprefix $(SRCDIR)/, Timer.cpp Timer.h)
	$(CXX) $(CXXFLAGS) -c -o $@ $<

#---------------------------------------------------------------------------------------------------
.PHONY: clean
clean:
	/bin/rm -f $(OBJDIR)/*.o
#---------------------------------------------------------------------------------------------------