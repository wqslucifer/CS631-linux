objects = ls.o print.o cmp.o

ls : $(objects)
	cc -o ls $(objects)

ls.o: ls.c ls.h
	cc -Wall -c -g ls.c

print.o: print.c print.h cmp.h 
	cc -Wall -c -g print.c
cmp.o: cmp.c cmp.h
	cc -Wall -c -g cmp.c

.PHONY: clearn
-clearn: rm ls $(objects)
