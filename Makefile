PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --embed --cflags)


main: info main.o jsonUtils.o utils.o Worm.o WormBody.o NervousSystem.o StretchReceptor.o Muscles.o TSearch.o random.o c302NervousSystem.o owSignalSimulatorForWorm2D.o owSignalSimulator.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o main main.o jsonUtils.o utils.o  Worm.o WormBody.o NervousSystem.o c302NervousSystem.o owSignalSimulatorForWorm2D.o owSignalSimulator.o StretchReceptor.o Muscles.o TSearch.o random.o $(LIBS)


info:
	@echo ' '
	@echo '  - Compiling with '
	@echo '      CXXFLAGS: ' $(CXXFLAGS) 
	@echo '      LIBS: ' $(LIBS) 
	@echo '      LDFLAGS: ' $(LDFLAGS) 
	@echo ' '

random.o: random.cpp random.h VectorMatrix.h
	g++ -c -O3 -flto random.cpp
TSearch.o: TSearch.cpp TSearch.h
	g++ -c -O3 -flto TSearch.cpp

jsonUtils.o: jsonUtils.cpp jsonUtils.h Worm.h
	g++ -c -O3 -std=c++11 -I/opt/homebrew/Cellar/nlohmann-json/3.11.3/include  -flto $(CXXFLAGS) $(LDFLAGS) jsonUtils.cpp	

utils.o: utils.cpp utils.h
	g++ -c -O3 -flto utils.cpp
Worm.o: Worm.cpp Worm.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS) Worm.cpp
WormBody.o: WormBody.cpp WormBody.h
	g++ -c -O3 -flto WormBody.cpp
NervousSystem.o: NervousSystem.cpp NervousSystem.h VectorMatrix.h random.h NervousSystemBase.h
	g++ -c -O3 -flto NervousSystem.cpp
owSignalSimulatorForWorm2D.o: neuromlLocal/owSignalSimulatorForWorm2D.cpp neuromlLocal/owSignalSimulatorForWorm2D.h #neuromlLocal/owSignalSimulator.h 
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) neuromlLocal/owSignalSimulatorForWorm2D.cpp
owSignalSimulator.o: neuromlLocal/owSignalSimulator.cpp neuromlLocal/owSignalSimulator.h #neuromlLocal/owINeuronSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) neuromlLocal/owSignalSimulator.cpp
#c302NervousSystem.o: neuromlLocal/c302NervousSystem.cpp neuromlLocal/c302NervousSystem.h NervousSystemBase.h neuromlLocal/owSignalSimulator.h
#	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS)  neuromlLocal/c302NervousSystem.cpp
c302NervousSystem.o: neuromlLocal/c302NervousSystem.cpp neuromlLocal/owSignalSimulatorForWorm2D.h #NervousSystemBase.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) neuromlLocal/c302NervousSystem.cpp 	
StretchReceptor.o: StretchReceptor.cpp StretchReceptor.h
	g++ -c -O3 -flto StretchReceptor.cpp
Muscles.o: Muscles.cpp Muscles.h VectorMatrix.h random.h
	g++ -c -O3 -flto Muscles.cpp
main.o: main.cpp Worm.h WormBody.h StretchReceptor.h Muscles.h TSearch.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS) main.cpp
tests.o: tests.cpp NervousSystem.o random.o
	g++ -c -O3 -flto tests.cpp
tests: info tests.o 
	g++ -pthread -o tests tests.o 

tests2.o: tests2.cpp NervousSystem.h random.h jsonUtils.h utils.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS) tests2.cpp
tests2: tests2.o NervousSystem.o random.o jsonUtils.o utils.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o tests2 tests2.o NervousSystem.o random.o utils.o jsonUtils.o $(LIBS)

clean:
	rm -f *.o main tests tests2
