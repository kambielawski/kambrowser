browser: main.o Browser.o
	g++ -std=c++11 main.o Browser.o -o browser

main.o: main.cpp Browser.h
	g++ -c main.cpp

Browser.o: Browser.cpp Browser.h networking/http_request.c
	g++ -c Browser.cpp

clean:
	rm -f main *.o
