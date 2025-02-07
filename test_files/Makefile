
CC = g++ -Wall
PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --embed --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)

EXTRA_FLAGS :=   -fPIE -O3  -Iinc

test_py_cpp.o: test_py_cpp.cpp
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) test_py_cpp.cpp 
	@echo ' '

test_py_cpp: test_py_cpp.o 

	@echo '  Linking... '
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o test_py_cpp test_py_cpp.o $(LIBS)

all: test_py_cpp test_py_cpp.o

clean:
	rm -f *.o test_py_cpp
	@echo ' '
