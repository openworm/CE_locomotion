CC = g++ -Wall
PYTHON_CONFIG ?= python3-config

LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --embed --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --includes)
#LDFLAGS += -L$(brew --prefix nlohmann-json)/lib
#CXXFLAGS="-I/opt/homebrew/include"
#CXXFLAGS += -I$(brew --prefix nlohmann-json)/include
LDFLAGS += "-L/opt/homebrew/lib"
#LDFLAGS += "-L$(brew --prefix nlohmann-json)/lib"
CXXFLAGS += "-I/opt/homebrew/include"

EXTRA_FLAGS :=   -fPIE -O3  -Iinc
main: main.o WormAgent.o CTRNN.o EvolutionCO.o Evolvable.o TSearchCO.o c302ForW2D.o owSignalSimulator.o Simulation.o owSignalSimulatorForWorm2D.o Worm2D21.o Worm2DCE.o WormRS18.o Worm21.o WormCE.o Segment21.o Worm2D.o WormBody.o Evolution21.o Evolution.o EvolutionCE.o EvolutionRS18.o NervousSystem.o StretchReceptor18.o StretchReceptorCE.o Muscles.o TSearch.o random.o jsonUtils.o NSToMuscles.o
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o main main.o WormAgent.o Evolvable.o EvolutionCO.o CTRNN.o TSearchCO.o Simulation.o Worm2D21.o Worm2DCE.o c302ForW2D.o owSignalSimulator.o owSignalSimulatorForWorm2D.o WormRS18.o Worm21.o WormCE.o Segment21.o Evolution21.o Evolution.o EvolutionCE.o EvolutionRS18.o Worm2D.o WormBody.o NervousSystem.o StretchReceptor18.o StretchReceptorCE.o NSToMuscles.o Muscles.o jsonUtils.o TSearch.o random.o $(LIBS)
c302ForW2D.o: ../neuromlLocal/c302ForW2D.cpp ../neuromlLocal/owSignalSimulatorForWorm2D.h  #../NervousSystemBase.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../neuromlLocal/c302ForW2D.cpp 
	@echo ' '
owSignalSimulator.o: ../neuromlLocal/owSignalSimulator.cpp ../neuromlLocal/owSignalSimulator.h ../neuromlLocal/owINeuronSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../neuromlLocal/owSignalSimulator.cpp
	@echo ' '
owSignalSimulatorForWorm2D.o: ../neuromlLocal/owSignalSimulatorForWorm2D.cpp ../neuromlLocal/owSignalSimulatorForWorm2D.h ../neuromlLocal/owSignalSimulator.h
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) ../neuromlLocal/owSignalSimulatorForWorm2D.cpp
	@echo ' '
random.o: ../random.cpp ../random.h #VectorMatrix.h
	$(CC) -c -O3 -flto ../random.cpp
jsonUtils.o: jsonUtils.cpp jsonUtils.h
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto jsonUtils.cpp	
TSearch.o: ../TSearch.cpp ../TSearch.h
	$(CC) -c -O3 -flto ../TSearch.cpp
Worm2D.o: Worm2D.cpp Worm2D.h jsonUtils.h
	$(CC) -c -O3 -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Worm2D.cpp
EvolutionRS18.o: EvolutionRS18.cpp EvolutionRS18.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto EvolutionRS18.cpp
Simulation.o: Simulation.cpp Simulation.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Simulation.cpp
EvolutionCE.o: EvolutionCE.cpp EvolutionCE.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto EvolutionCE.cpp
EvolutionCO.o: EvolutionCO.cpp EvolutionCO.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto EvolutionCO.cpp	
Evolution21.o: Evolution21.cpp Evolution21.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Evolution21.cpp
Evolution.o: Evolution.cpp Evolution.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Evolution.cpp
Evolvable.o: Evolvable.cpp Evolvable.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Evolvable.cpp
NSToMuscles.o: NSToMuscles.cpp NSToMuscles.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto NSToMuscles.cpp
WormRS18.o: WormRS18.cpp WormRS18.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto WormRS18.cpp
Worm2DCE.o: Worm2DCE.cpp Worm2DCE.h 
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -std=c++11  -flto Worm2DCE.cpp 
WormCE.o: WormCE.cpp WormCE.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto WormCE.cpp
Worm2D21.o: Worm2D21.cpp Worm2D21.h 
	$(CC) -c -O3 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -std=c++11  -flto Worm2D21.cpp 
Worm21.o: Worm21.cpp Worm21.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Worm21.cpp
Worm2Dmods.o: Worm2Dmods.cpp Worm2Dmods.h 
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Worm2Dmods.cpp	
Segment21.o: Segment21.cpp Segment21.h
	$(CC) -c -O3 -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto Segment21.cpp	
WormBody.o: ../WormBody.cpp ../WormBody.h
	$(CC) -c -O3 -flto ../WormBody.cpp
NervousSystem.o: ../NervousSystem.cpp ../NervousSystem.h #VectorMatrix.h random.h ../neuromlLocal/NervousSystemBase.h
	$(CC) -c -O3 -std=c++11 -flto ../NervousSystem.cpp
StretchReceptor18.o: StretchReceptor18.cpp StretchReceptor18.h
	$(CC) -c -O3 -std=c++11 -flto StretchReceptor18.cpp
StretchReceptorCE.o: StretchReceptorCE.cpp StretchReceptorCE.h
	$(CC) -c -O3 -std=c++11 -flto StretchReceptorCE.cpp	
Muscles.o: ../Muscles.cpp ../Muscles.h #VectorMatrix.h random.h
	$(CC) -c -O3 -flto ../Muscles.cpp
CTRNN.o: CTRNN.cpp CTRNN.h #random.h
	$(CC) -c -O3 -std=c++11 -flto CTRNN.cpp
TSearchCO.o: TSearchCO.cpp TSearchCO.h
	$(CC) -c -O3 -std=c++11 -flto TSearchCO.cpp
WormAgent.o: WormAgent.cpp WormAgent.h #TSearchCO.h CTRNN.h #VectorMatrixCO.h
	$(CC) -c -O3 -std=c++11 $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto WormAgent.cpp	
main.o: main.cpp  #WormRS18.h Worm2D.h WormBody.h StretchReceptor18.h Muscles.h TSearch.h
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto main.cpp
main_osc.o: main_osc.cpp  #WormRS18.h Worm2D.h WormBody.h StretchReceptor18.h Muscles.h TSearch.h
	$(CC) -c -O3 -std=c++11  $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -flto main_osc.cpp
main_osc: main_osc.o c302ForW2D.o Evolvable.o Worm21.o Worm2D21.o owSignalSimulator.o owSignalSimulatorForWorm2D.o Simulation.o Worm2Dmods.o Worm2D.o WormBody.o Evolution.o NervousSystem.o StretchReceptor18.o StretchReceptorCE.o Muscles.o TSearch.o random.o jsonUtils.o NSToMuscles.o
	$(CC) $(CXXFLAGS) $(LDFLAGS) $(EXTRA_FLAGS) -pthread -o main_osc c302ForW2D.o Evolvable.o Worm21.o Worm2D21.o owSignalSimulator.o owSignalSimulatorForWorm2D.o main_osc.o Simulation.o Worm2Dmods.o Worm2D.o WormBody.o Evolution.o NervousSystem.o StretchReceptor18.o StretchReceptorCE.o Muscles.o TSearch.o random.o jsonUtils.o NSToMuscles.o $(LIBS)		
clean:
	rm -f *.o main
