main: main.o CTRNN.o TSearch.o WormAgent.o random.o argUtils.o
	g++ -pthread -o main main.o CTRNN.o TSearch.o WormAgent.o random.o argUtils.o
random.o: random.cpp random.h VectorMatrix.h
	g++ -pthread -c -O3 random.cpp
CTRNN.o: CTRNN.cpp random.h CTRNN.h
	g++ -pthread -c -O3 CTRNN.cpp
TSearch.o: TSearch.cpp TSearch.h
	g++ -pthread -c -O3 TSearch.cpp
argUtils.o: ../argUtils.cpp ../argUtils.h
	g++ -c -O3 -std=c++11 -flto ../argUtils.cpp		
WormAgent.o: WormAgent.cpp WormAgent.h TSearch.h CTRNN.h random.h VectorMatrix.h
	g++ -pthread -c -O3 WormAgent.cpp
main.o: main.cpp CTRNN.h WormAgent.h TSearch.h
	g++ -pthread -c -O3 main.cpp
clean:
	rm -f *.o main
