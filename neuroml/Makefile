
CC = g++ -Wall
PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --embed --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)

EXTRA_FLAGS :=   -fPIE -O3  -Iinc

main.o: main.cpp c302NervousSystem.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) main.cpp 
	@echo ' '

c302NervousSystem.o: c302NervousSystem.cpp owSignalSimulator.h ../NervousSystemBase.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) c302NervousSystem.cpp 
	@echo ' '

owSignalSimulator.o: owSignalSimulator.cpp owSignalSimulator.h owINeuronSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) owSignalSimulator.cpp
	@echo ' '
main: main.o c302NervousSystem.o owSignalSimulator.o
	@echo '  Linking... '
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o main main.o c302NervousSystem.o owSignalSimulator.o $(LIBS)

all: main main.o

clean:
	rm -f *.o main
	@echo ' '
