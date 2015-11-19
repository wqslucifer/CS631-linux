objects = main.o net.o http.o

sws : $(objects)
	cc -o sws $(objects) -lbsd

main.o: main.c main.h 
	cc -Wall -c -g main.c
net.o: net.c net.h 
	cc -Wall -c -g net.c
http.o: http.c http.h
	cc -Wall -c -g http.c

.PHONY: clean
-clean: rm ls $(objects)
