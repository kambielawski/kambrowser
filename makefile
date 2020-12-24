main: 
	gcc -ggdb3 -std=c99 -Wall -Wextra -o wget wget.c

clean:
	rm -f main *.o
