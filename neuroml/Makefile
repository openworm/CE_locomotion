
CC = g++ -Wall
PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --embed --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)

EXTRA_FLAGS :=   -fPIE -O3  -Iinc

main.o: main.cpp NervousSystem2D.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) main.cpp 
	@echo ' '

NervousSystem2D.o: NervousSystem2D.cpp owSignalSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) NervousSystem2D.cpp 
	@echo ' '

owSignalSimulator.o: owSignalSimulator.cpp owSignalSimulator.h owINeuronSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) owSignalSimulator.cpp
	@echo ' '
NervousSystem.o: ../NervousSystem.cpp ../NervousSystem.h ../VectorMatrix.h ../random.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../NervousSystem.cpp
	@echo ' '

main: main.o NervousSystem2D.o owSignalSimulator.o NervousSystem.o

	@echo '  Linking... '
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o main main.o NervousSystem2D.o owSignalSimulator.o NervousSystem.o $(LIBS)

all: main main.o

clean:
	rm -f *.o main
	@echo ' '
