
PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --libs)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)

test_py_cpp.o: test_py_cpp.cpp
	g++ -c -O3 $(CXXFLAGS) $(LIBS) test_py_cpp.cpp

test_py_cpp: test_py_cpp.o 
	g++ $(CXXFLAGS) $(LIBS) -pthread -o test_py_cpp test_py_cpp.o

all: test_py_cpp test_py_cpp.o

clean:
	rm -f *.o test_py_cpp
