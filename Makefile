PYTHON_CONFIG ?= python3-config


LIBS := $(shell $(PYTHON_CONFIG) --embed --libs)
LDFLAGS := $(shell $(PYTHON_CONFIG) --ldflags)
CXXFLAGS := $(shell $(PYTHON_CONFIG) --cflags)


main: main.o Worm.o Worm2D.o utils.o WormBody.o NervousSystem.o StretchReceptor.o Muscles.o TSearch.o random.o NervousSystem2D.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o main main.o Worm2D.o utils.o Worm.o WormBody.o NervousSystem.o NervousSystem2D.o StretchReceptor.o Muscles.o TSearch.o random.o $(LIBS)
random.o: random.cpp random.h VectorMatrix.h
	g++ -c -O3 -flto random.cpp
TSearch.o: TSearch.cpp TSearch.h
	g++ -c -O3 -flto TSearch.cpp
Worm.o: Worm.cpp Worm.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS) Worm.cpp
Worm2D.o: Worm2D.cpp Worm2D.h
	g++ -c -O3 -std=c++11 -I/opt/homebrew/Cellar/nlohmann-json/3.11.3/include  -flto $(CXXFLAGS) $(LDFLAGS) Worm2D.cpp	
utils.o: utils.cpp utils.h
	g++ -c -O3 -flto utils.cpp
WormBody.o: WormBody.cpp WormBody.h
	g++ -c -O3 -flto WormBody.cpp
NervousSystem.o: NervousSystem.cpp NervousSystem.h VectorMatrix.h random.h
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
tests2.o: tests2.cpp NervousSystem.h random.h Worm2D.h
	g++ -c -O3 -flto $(CXXFLAGS) $(LDFLAGS) tests2.cpp
tests2: tests2.o NervousSystem.o random.o Worm2D.o
	g++ $(CXXFLAGS) $(LDFLAGS) -pthread -o tests2 tests2.o NervousSystem.o random.o Worm2D.o $(LIBS)	
clean:
	rm -f *.o main tests tests2
