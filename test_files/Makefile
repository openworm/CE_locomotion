
CC = g++ -Wall
PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)

EXTRA_FLAGS :=   -fPIE -O3  -Iinc

test_py_cpp.o: test_py_cpp.cpp
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) test_py_cpp.cpp
	@echo ' '

test_py_cpp: test_py_cpp.o 

	@echo '  Linking... '
	$(CC) $(CXXFLAGS) $(LIBS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o test_py_cpp test_py_cpp.o

all: test_py_cpp test_py_cpp.o

clean:
	rm -f *.o test_py_cpp
	@echo ' '
