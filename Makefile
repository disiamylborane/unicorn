
CCFLAGS = -std=gnu++11 -Wall
CC=g++ $(CCFLAGS)
LD=ld

all: unicorn

unicorn: unicorn_types.o

unicorn_types.o: unicorn_types.cpp
	$(CC) -c unicorn_types.cpp

clean:
	rm -f *.o


