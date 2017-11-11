
CCFLAGS = -std=gnu++11 -Wall -g
CC=g++ $(CCFLAGS)
LD=g++

all: unicorn

unicorn: compile test.cpp
	$(CC) -c test.cpp -o test.o
	$(LD) -o unicorn test.o unicorn_block.o unicorn_graph.o unicorn_platform_specific.o unicorn_types.o unicorn_library.o unicorn_lib_std.o unicorn_lib_send.o unicorn_lib_recv.o unicorn_lib_linecode.o unicorn_lib_const.o
	make clean

compile: unicorn_block.o unicorn_graph.o unicorn_platform_specific.o unicorn_types.o unicorn_library.o unicorn_lib_std.o unicorn_lib_send.o unicorn_lib_recv.o unicorn_lib_linecode.o unicorn_lib_const.o

unicorn_block.o: unicorn_block.cpp
	$(CC) -c unicorn_block.cpp

unicorn_graph.o: unicorn_graph.cpp
	$(CC) -c unicorn_graph.cpp

unicorn_library.o: unicorn_library.cpp
	$(CC) -c unicorn_library.cpp

unicorn_lib_std.o: unicorn_lib_std.cpp
	$(CC) -c unicorn_lib_std.cpp

unicorn_lib_send.o: unicorn_lib_send.cpp
	$(CC) -c unicorn_lib_send.cpp

unicorn_lib_recv.o: unicorn_lib_recv.cpp
	$(CC) -c unicorn_lib_recv.cpp

unicorn_lib_linecode.o: unicorn_lib_linecode.cpp
	$(CC) -c unicorn_lib_linecode.cpp

unicorn_lib_const.o: unicorn_lib_const.cpp
	$(CC) -c unicorn_lib_const.cpp
	
unicorn_platform_specific.o: unicorn_platform_specific.cpp
	$(CC) -c unicorn_platform_specific.cpp
	
unicorn_types.o: unicorn_types.cpp
	$(CC) -c unicorn_types.cpp

recompile: clean unicorn_block.o unicorn_graph.o unicorn_library.o unicorn_platform_specific.o unicorn_types.o

clean:
	rm -f *.o

deepclean:
	rm -f *.o
	rm -f unicorn
