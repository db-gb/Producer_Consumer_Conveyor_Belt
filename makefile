mizzo: main.o workers.o conveyor.o io.o
	gcc -g main.o workers.o conveyor.o io.o -o mizzo -lpthread -lrt

main.o: conveyor.h workers.h io.h main.c
	gcc -g -c main.c -lrt
	
io.o: io.h io.c
	gcc -g -c io.c

workers.o: workers.h workers.c
	gcc -g -c workers.c

conveyor.o: conveyor.h conveyor.c
	gcc -g -c conveyor.c

	

