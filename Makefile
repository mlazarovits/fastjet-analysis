#include ROOT cflags and libraries
ROOTCFLAGS  = $(shell root-config --cflags)
ROOTGLIBS   = $(shell root-config --glibs)


#include FastJet cxxflags and libraries
FJCFLAGS    = $(shell ~/fastjet-install/bin/fastjet-config --cxxflags)
FJGLIBS     = $(shell ~/fastjet-install/bin/fastjet-config --libs)

#include Delphes include files and libraries
DELPHES_DIR = /Users/margaretlazarovits/delphes
DELPHES_INC = -I$(DELPHES_DIR) -I$(DELPHES_DIR)/external
DELPHES_LIBS_NOFJ = -Wl,-rpath,$(DELPHES_DIR) -L$(DELPHES_DIR) -lDelphesNoFastJet
DELPHES_LIBS = -Wl,-rpath,$(DELPHES_DIR) -L$(DELPHES_DIR) -lDelphes

#set c(xx)flags and libraries
CXXFLAGS    = $(ROOTCFLAGS)
CXXFLAGS   += $(FJCFLAGS)
CXXFLAGS   += $(DELPHES_INC)

GLIBS       = $(ROOTGLIBS)
GLIBS      += $(FJGLIBS)
#specify compiler
CXX         = g++

#specify local paths
INCLUDEDIR  = ./include/
SRCDIR      = ./src/
#make sure compiler knows where local include files are
CXX	   += -I$(INCLUDEDIR) -I.
OUTOBJ	    = ./obj/

#specify local source, include, and object files
CC_FILES    = $(wildcard src/*.cc)
HH_FILES    = $(wildcard include/*.hh)
OBJ_FILES   = $(addprefix $(OUTOBJ),$(notdir $(CC_FILES:.cc=.o)))


#specify what to make
all: alltargets
example: short-example.x
delphes: delphes-analysis.x
fastjet-cluster: fastjet-cluster.x
alltargets: fastjet-cluster.x delphes-analysis.x

#executables
short-example.x: $(SRCDIR)short-example.C $(OBJ_FILES) $(HH_FILES)
	$(CXX) $(CXXFLAGS) -o short-example.x $(OUTOBJ)/*.o $(GLIBS) $ $<
	touch short-example.x

fastjet-cluster.x: $(SRCDIR)fastjet-cluster.C $(OBJ_FILES) $(HH_FILES)
	$(CXX) $(CXXFLAGS) -o fastjet-cluster.x $(OUTOBJ)/*.o $(GLIBS) $(DELPHES_LIBS_NOFJ) $ $<
	touch fastjet-cluster.x
	
delphes-analysis.x: $(SRCDIR)delphes-analysis.C $(OBJ_FILES) $(HH_FILES)
	$(CXX) $(CXXFLAGS) -o delphes-analysis.x $(OUTOBJ)/*.o $(GLIBS) $(DELPHES_LIBS) $ $<
	touch delphes-analysis.x
#where to put object files
$(OUTOBJ)%.o: src/%.cc include/%.hh
	$(CXX) $(CXXFLAGS) -c $< -o $@

#clean options
clean:
	rm -f $(OUTOBJ)*.o
	rm -f *.x
	rm -f AutoDict*
	rm -f *.d



