ifeq ($(CONDA_PREFIX),)
    PYTHON_CONFIG ?= python3-config
else
    PYTHON_CONFIG ?= $(CONDA_PREFIX)/bin/python3-config
endif


#LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
#LDFLAGS := $(shell $(PYTHON_CONFIG) --ldflags)
#REMOVE=-arch arm64 -arch x86_64
#REPLACE=
#CXXFLAGS0 := $(shell $(PYTHON_CONFIG) --embed --cflags)
#CXXFLAGS := $(subst $(REMOVE),$(REPLACE),$(CXXFLAGS0))

LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --embed --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --includes)

# Add Homebrew paths for macOS
LDFLAGS += "-L/opt/homebrew/lib"
CXXFLAGS += "-I/opt/homebrew/include"

# In a conda env, python3-config points -L at the (lib-less) config dir;
# add the env's actual lib dir to the link path and embed an rpath for runtime.
ifneq ($(CONDA_PREFIX),)
    LDFLAGS += -L$(CONDA_PREFIX)/lib -Wl,-rpath,$(CONDA_PREFIX)/lib
endif

main: info main.o jsonUtils.o argUtils.o Worm.o WormBody.o NervousSystem.o StretchReceptor.o Muscles.o TSearch.o random.o c302NervousSystem.o c302ForW2D.o owSignalSimulatorForWorm2D.o owSignalSimulator.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o main main.o jsonUtils.o argUtils.o  Worm.o WormBody.o NervousSystem.o c302NervousSystem.o c302ForW2D.o owSignalSimulatorForWorm2D.o owSignalSimulator.o StretchReceptor.o Muscles.o TSearch.o random.o $(LIBS)


info:
	@echo ' '
	@echo '  - Compiling with '
	@echo '      CXXFLAGS: ' $(CXXFLAGS) 
	@echo '      LIBS: ' $(LIBS) 
	@echo '      LDFLAGS: ' $(LDFLAGS) 
	@echo ' '



random.o: random.cpp random.h VectorMatrix.h
	g++ -c -O3 -std=c++11 -flto random.cpp
TSearch.o: TSearch.cpp TSearch.h
	g++ -c -O3 -std=c++11 -flto TSearch.cpp

jsonUtils.o: jsonUtils.cpp jsonUtils.h
	g++ -c -O3 -std=c++11   -flto $(CXXFLAGS) $(LDFLAGS) jsonUtils.cpp	

argUtils.o: argUtils.cpp argUtils.h
	g++ -c -O3 -std=c++11 -flto argUtils.cpp
Worm.o: Worm.cpp Worm.h
	g++ -c -O3 -std=c++11 -flto $(CXXFLAGS) $(LDFLAGS) Worm.cpp
WormBody.o: WormBody.cpp WormBody.h
	g++ -c -O3 -std=c++11 -flto WormBody.cpp
NervousSystem.o: NervousSystem.cpp NervousSystem.h VectorMatrix.h random.h neuromlLocal/NervousSystemBase.h
	g++ -c -O3 -std=c++11 -flto NervousSystem.cpp
owSignalSimulatorForWorm2D.o: neuromlLocal/owSignalSimulatorForWorm2D.cpp neuromlLocal/owSignalSimulatorForWorm2D.h #neuromlLocal/owSignalSimulator.h 
	$(CC) -c -O3 -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) neuromlLocal/owSignalSimulatorForWorm2D.cpp
owSignalSimulator.o: neuromlLocal/owSignalSimulator.cpp neuromlLocal/owSignalSimulator.h #neuromlLocal/owINeuronSimulator.h
	$(CC) -c -O3 -std=c++11 $(CXXFLAGS) $(LDFLAGS) neuromlLocal/owSignalSimulator.cpp
c302NervousSystem.o: neuromlLocal/c302NervousSystem.cpp random.h #neuromlLocal/NervousSystemBase.h
	$(CC) -c -O3 -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) neuromlLocal/c302NervousSystem.cpp
c302ForW2D.o: neuromlLocal/c302ForW2D.cpp neuromlLocal/owSignalSimulatorForWorm2D.h #random.h #neuromlLocal/NervousSystemBase.h
	$(CC) -c -O3 -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) neuromlLocal/c302ForW2D.cpp
StretchReceptor.o: StretchReceptor.cpp StretchReceptor.h
	g++ -c -O3 -std=c++11 -flto StretchReceptor.cpp
Muscles.o: Muscles.cpp Muscles.h VectorMatrix.h random.h
	g++ -c -O3 -std=c++11 -flto Muscles.cpp
main.o: main.cpp Worm.h WormBody.h StretchReceptor.h Muscles.h TSearch.h
	g++ -c -O3 -std=c++11 -flto $(CXXFLAGS) $(LDFLAGS) main.cpp
tests.o: tests.cpp NervousSystem.h 
	g++ -c -O3 -std=c++11 -flto tests.cpp
tests: info tests.o NervousSystem.o random.o
	g++ -pthread -o tests tests.o NervousSystem.o random.o

tests2.o: tests2.cpp NervousSystem.h random.h jsonUtils.h argUtils.h
	g++ -c -O3 -std=c++11 -flto $(CXXFLAGS) $(LDFLAGS) tests2.cpp
tests2: tests2.o NervousSystem.o random.o jsonUtils.o argUtils.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o tests2 tests2.o NervousSystem.o random.o argUtils.o jsonUtils.o $(LIBS)

clean:
	rm -f *.o neuromlLocal/*.o main tests tests2
