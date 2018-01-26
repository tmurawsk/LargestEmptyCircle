CC = g++
CLAGS = -std=c++11

all:	main.o voronoi.o
	$(CC) $(CFLAGS) main.o voronoi.o -o largestEmptyCircle

main.o: main.cpp
	$(CC) $(CFLAGS) -c main.cpp -o main.o

voronoi.o:	Voronoi.cpp
	$(CC) $(CFLAGS) -c Voronoi.cpp -o voronoi.o

clean:
	rm -f *.o
