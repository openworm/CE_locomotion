
CC = g++ -Wall
PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --embed --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)

EXTRA_FLAGS :=   -fPIE -O3  -Iinc

testc302NervousSystem.o: testc302NervousSystem.cpp c302NervousSystem.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) testc302NervousSystem.cpp
	@echo ' '

c302NervousSystem.o: c302NervousSystem.cpp owSignalSimulatorForWorm2D.h ../NervousSystemBase.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) c302NervousSystem.cpp 
	@echo ' '

owSignalSimulator.o: owSignalSimulator.cpp owSignalSimulator.h owINeuronSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) owSignalSimulator.cpp
	@echo ' '

owSignalSimulatorForWorm2D.o: owSignalSimulatorForWorm2D.cpp owSignalSimulatorForWorm2D.h owSignalSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) owSignalSimulatorForWorm2D.cpp
	@echo ' '
testc302NervousSystem: testc302NervousSystem.o c302NervousSystem.o owSignalSimulatorForWorm2D.o owSignalSimulator.o
	@echo '  Linking... '
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o testc302NervousSystem testc302NervousSystem.o c302NervousSystem.o owSignalSimulatorForWorm2D.o owSignalSimulator.o $(LIBS)

all: testc302NervousSystem testc302NervousSystem.o

clean:
	rm -f *.o testc302NervousSystem
	@echo ' '
