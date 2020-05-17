all:
	gcc -Wall -O3 -o scand scand.c 
so: 
	gcc -Wall -O3 -fPIC -shared -o scand.so scand.c
clean:
	rm -f scand.so scand
