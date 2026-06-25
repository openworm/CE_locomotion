
CC = g++ -Wall

ifeq ($(CONDA_PREFIX),)
    PYTHON ?= python3
else
    PYTHON ?= $(CONDA_PREFIX)/bin/python
endif

PYTHON_INCLUDE_DIR := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_paths()['include'])")
PYTHON_LIB_DIR := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('LIBDIR'))")
PYTHON_LDVERSION := $(shell $(PYTHON) -c "import sysconfig; print(sysconfig.get_config_var('LDVERSION'))")
PYTHON_LIB := python$(PYTHON_LDVERSION)

UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Linux)
    RPATH_FLAG = -Wl,-rpath,$(PYTHON_LIB_DIR)
else ifeq ($(UNAME_S),Darwin)
    RPATH_FLAG = -Wl,-rpath,@loader_path -Wl,-rpath,$(PYTHON_LIB_DIR)
else
    $(error Unsupported OS)
endif

CXXFLAGS := -Wall -I$(PYTHON_INCLUDE_DIR) -std=c++11
LDFLAGS := -L$(PYTHON_LIB_DIR) $(RPATH_FLAG)
LIBS := -l$(PYTHON_LIB) -lpthread -ldl -lutil -lm

# Extra flags for MacOS
LDFLAGS += "-L/opt/homebrew/lib"
CXXFLAGS += "-I/opt/homebrew/include"

EXTRA_FLAGS := -fPIE -O3 -Iinc

testc302NervousSystem.o: testc302NervousSystem.cpp ../c302NervousSystem.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) testc302NervousSystem.cpp
	@echo ' '

c302NervousSystem.o: ../c302NervousSystem.cpp #../owSignalSimulatorForWorm2D.h #../NervousSystemBase.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../c302NervousSystem.cpp 
	@echo ' '
c302ForW2D.o: ../c302ForW2D.cpp ../owSignalSimulatorForWorm2D.h  #../NervousSystemBase.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../c302ForW2D.cpp 
	@echo ' '


owSignalSimulator.o: ../owSignalSimulator.cpp ../owSignalSimulator.h ../owINeuronSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../owSignalSimulator.cpp
	@echo ' '

owSignalSimulatorForWorm2D.o: ../owSignalSimulatorForWorm2D.cpp ../owSignalSimulatorForWorm2D.h ../owSignalSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../owSignalSimulatorForWorm2D.cpp
	@echo ' '
testc302NervousSystem: testc302NervousSystem.o c302ForW2D.o c302NervousSystem.o owSignalSimulatorForWorm2D.o owSignalSimulator.o
	@echo '  Linking... '
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o testc302NervousSystem testc302NervousSystem.o c302ForW2D.o c302NervousSystem.o owSignalSimulatorForWorm2D.o owSignalSimulator.o $(LIBS)

all: testc302NervousSystem testc302NervousSystem.o

clean:
	rm -f *.o testc302NervousSystem
	@echo ' '
