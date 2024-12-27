main: main.o Worm.o Worm2D.o WormBody.o NervousSystem.o StretchReceptor.o Muscles.o TSearch.o random.o
	g++ -pthread -o main main.o Worm2D.o Worm.o WormBody.o NervousSystem.o StretchReceptor.o Muscles.o TSearch.o random.o
random.o: random.cpp random.h VectorMatrix.h
	g++ -c -O3 -flto random.cpp
TSearch.o: TSearch.cpp TSearch.h
	g++ -c -O3 -flto TSearch.cpp
Worm.o: Worm.cpp Worm.h
	g++ -c -O3 -flto Worm.cpp
Worm2D.o: Worm2D.cpp Worm2D.h
	g++ -c -O3 -std=c++11 -flto Worm2D.cpp	
WormBody.o: WormBody.cpp WormBody.h
	g++ -c -O3 -flto WormBody.cpp
NervousSystem.o: NervousSystem.cpp NervousSystem.h VectorMatrix.h random.h
	g++ -c -O3 -flto NervousSystem.cpp
StretchReceptor.o: StretchReceptor.cpp StretchReceptor.h
	g++ -c -O3 -flto StretchReceptor.cpp
Muscles.o: Muscles.cpp Muscles.h VectorMatrix.h random.h
	g++ -c -O3 -flto Muscles.cpp
main.o: main.cpp Worm.h WormBody.h StretchReceptor.h Muscles.h TSearch.h
	g++ -c -O3 -flto main.cpp
clean:
	rm -f *.o main
