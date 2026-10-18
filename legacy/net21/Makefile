main: main.o Worm.o WormBody.o Segment.o NervousSystem.o Muscles.o TSearch.o random.o argUtils.o
	g++ -pthread -o main main.o Worm.o WormBody.o argUtils.o Segment.o NervousSystem.o Muscles.o TSearch.o random.o
random.o: random.cpp random.h VectorMatrix.h
	g++ -c -O3 -flto random.cpp
argUtils.o: ../argUtils.cpp ../argUtils.h
	g++ -c -O3 -std=c++11 -flto ../argUtils.cpp	
TSearch.o: TSearch.cpp TSearch.h
	g++ -c -O3 -flto TSearch.cpp
Worm.o: Worm.cpp Worm.h
	g++ -c -O3 -flto Worm.cpp
WormBody.o: WormBody.cpp WormBody.h
	g++ -c -O3 -flto WormBody.cpp
Segment.o: Segment.cpp Segment.h
	g++ -c -O3 -flto Segment.cpp
NervousSystem.o: NervousSystem.cpp NervousSystem.h VectorMatrix.h random.h
	g++ -c -O3 -flto NervousSystem.cpp
Muscles.o: Muscles.cpp Muscles.h VectorMatrix.h random.h
	g++ -c -O3 -flto Muscles.cpp
main.o: main.cpp Worm.h WormBody.h Muscles.h TSearch.h
	g++ -c -O3 -flto main.cpp
clean:
	rm -f *.o main
