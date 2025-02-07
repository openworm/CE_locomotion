PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)

ifeq ($(MAKE_JSON),1)
CXXFLAGS += -DMAKE_JSON
endif
ifeq ($(MAKE_JSON),1)
main: main.o jsonUtils.o utils.o Worm.o WormBody.o NervousSystem.o StretchReceptor.o Muscles.o TSearch.o random.o NervousSystem2D.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o main main.o jsonUtils.o utils.o  Worm.o WormBody.o NervousSystem.o NervousSystem2D.o StretchReceptor.o Muscles.o TSearch.o random.o $(LIBS)
else
main: main.o utils.o  Worm.o WormBody.o NervousSystem.o StretchReceptor.o Muscles.o TSearch.o random.o NervousSystem2D.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o main main.o utils.o  Worm.o WormBody.o NervousSystem.o NervousSystem2D.o StretchReceptor.o Muscles.o TSearch.o random.o $(LIBS)
endif
random.o: random.cpp random.h VectorMatrix.h
	g++ -c -O3 -flto random.cpp
TSearch.o: TSearch.cpp TSearch.h
	g++ -c -O3 -flto TSearch.cpp
ifeq ($(MAKE_JSON),1)	
jsonUtils.o: jsonUtils.cpp jsonUtils.h
	g++ -c -O3 -std=c++11 -I/opt/homebrew/Cellar/nlohmann-json/3.11.3/include  -flto $(CXXFLAGS) $(LDFLAGS) jsonUtils.cpp	
endif
utils.o: utils.cpp utils.h
	g++ -c -O3 -flto utils.cpp
Worm.o: Worm.cpp Worm.h
	g++ -c -O3 -flto Worm.cpp
WormBody.o: WormBody.cpp WormBody.h
	g++ -c -O3 -flto WormBody.cpp
NervousSystem.o: NervousSystem.cpp NervousSystem.h VectorMatrix.h random.h NervousSystemBase.h
	g++ -c -O3 -flto NervousSystem.cpp
NervousSystem2D.o: neuroml/NervousSystem2D.cpp neuroml/NervousSystem2D.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS)  neuroml/NervousSystem2D.cpp
StretchReceptor.o: StretchReceptor.cpp StretchReceptor.h
	g++ -c -O3 -flto StretchReceptor.cpp
Muscles.o: Muscles.cpp Muscles.h VectorMatrix.h random.h
	g++ -c -O3 -flto Muscles.cpp
main.o: main.cpp Worm.h WormBody.h StretchReceptor.h Muscles.h TSearch.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS) main.cpp
tests.o: tests.cpp NervousSystem.o random.o
	g++ -c -O3 -flto tests.cpp
tests: tests.o 
	g++ -pthread -o tests tests.o 
ifeq ($(MAKE_JSON),1)	
tests2.o: tests2.cpp NervousSystem.h random.h jsonUtils.h utils.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS) tests2.cpp
tests2: tests2.o NervousSystem.o random.o jsonUtils.o utils.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o tests2 tests2.o NervousSystem.o random.o utils.o jsonUtils.o $(LIBS)
endif		
clean:
	rm -f *.o main tests tests2
