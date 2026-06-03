# ===============================
# Conda-safe, cross-platform Makefile
# ===============================
CC = g++

# ---------------------------
# Detect Python interpreter
# ---------------------------
ifeq ($(CONDA_PREFIX),)
    PYTHON ?= python3
else
    PYTHON ?= $(CONDA_PREFIX)/bin/python
endif

# ---------------------------
# Get include/lib dirs via sysconfig
# ---------------------------
PYTHON_INCLUDE_DIR := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_paths()['include'])")
PYTHON_LIB_DIR := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")
PYTHON_LDVERSION := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('LDVERSION'))")
PYTHON_LIB := python$(PYTHON_LDVERSION)

# ---------------------------
# Check that Python headers exist
# ---------------------------
ifeq ($(wildcard $(PYTHON_INCLUDE_DIR)/Python.h),)
$(error Python.h not found in $(PYTHON_INCLUDE_DIR). Make sure Python dev headers are installed in this environment)
endif

# ---------------------------
# OS-specific rpath
# ---------------------------
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    RPATH_FLAG = -Wl,-rpath,$(PYTHON_LIB_DIR)
else ifeq ($(UNAME_S),Darwin)
    RPATH_FLAG = -Wl,-rpath,@loader_path -Wl,-rpath,$(PYTHON_LIB_DIR)
else
    $(error Unsupported OS)
endif

# ---------------------------
# Compiler / linker flags
# ---------------------------
CXXFLAGS := -Wall -I$(PYTHON_INCLUDE_DIR)
LDFLAGS := -L$(PYTHON_LIB_DIR) $(RPATH_FLAG)
LIBS := -l$(PYTHON_LIB) -lpthread -ldl -lutil -lm

# Extra flags for MacOS
LDFLAGS += "-L/opt/homebrew/lib"
CXXFLAGS += "-I/opt/homebrew/include"

EXTRA_FLAGS := -fPIE -O3 -Iinc
SANFLAG :=

# ---------------------------
# Source files
# ---------------------------
SRC_COMMON :=  ../neuromlLocal/c302ForW2D.cpp ../neuromlLocal/owSignalSimulatorForWorm2D.cpp \
        ../neuromlLocal/owSignalSimulator.cpp ../utils.cpp Evolvable.cpp \
        WormAgent.cpp Worm2DSR.cpp Simulation.cpp \
        Worm2D21.cpp Worm2DCE.cpp WormRS18.cpp Worm21.cpp \
        Segment21.cpp Worm2D.cpp ../WormBody.cpp Evolution21.cpp \
        Evolution.cpp EvolutionCE.cpp EvolutionRS18.cpp ../NervousSystem.cpp \
        StretchReceptor18.cpp StretchReceptorCE.cpp StretchReceptor.cpp \
        ../Muscles.cpp ../TSearch.cpp TSearchCO.cpp ../random.cpp \
        jsonUtils.cpp NSToMuscles.cpp EvolutionCO.cpp Worm2Dmods.cpp

# Main binaries sources
SRC_MAIN := main.cpp $(SRC_COMMON)
SRC_MAIN_OSC := main_osc.cpp $(SRC_COMMON)

# Object files
OBJS_MAIN := $(notdir $(SRC_MAIN:.cpp=.o))
OBJS_MAIN_OSC := $(notdir $(SRC_MAIN_OSC:.cpp=.o))

# ---------------------------
# Search paths for sources
# ---------------------------
vpath %.cpp ../neuromlLocal
vpath %.cpp ../

# ---------------------------
# Rules
# ---------------------------
all: info main main_osc


info:
	@echo ' '
	@echo '  - Compiling with '
	@echo '      CXXFLAGS: ' $(CXXFLAGS) 
	@echo '      LIBS: ' $(LIBS) 
	@echo '      LDFLAGS: ' $(LDFLAGS) 
	@echo ' '


# Generic compilation rule
%.o: %.cpp
	$(CC) -c $(CXXFLAGS) $(EXTRA_FLAGS) $(SANFLAG) -std=c++11 $< -o $@

# Main binary
main: $(OBJS_MAIN)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) $(SANFLAG) -pthread -o $@ $^ $(LIBS)

# Main_osc binary
main_osc: $(OBJS_MAIN_OSC)
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) $(SANFLAG) -pthread -o $@ $^ $(LIBS)

# Clean
clean:
	rm -f *.o main main_osc
