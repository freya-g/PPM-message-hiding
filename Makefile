CFLAGS=-Wall
all:
	gcc -o hide hide.c $(CFLAGS)
	gcc -o unhide unhide.c $(CFLAGS)
hide:
	gcc -o hide hide.c $(CFLAGS)
unhide:
	gcc -o unhide unhide.c $(CFLAGS)
clean:
	rm hide
	rm unhide