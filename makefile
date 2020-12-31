browser: main.o Browser.o HtmlParser.o
	g++ -std=c++11 main.o Browser.o HtmlParser.o -o browser

main.o: main.cpp Browser.h
	g++ -c main.cpp

Browser.o: Browser.cpp Browser.h
	g++ -c Browser.cpp

HtmlParser.o: HtmlParser.cpp HtmlParser.h
	g++ -c HtmlParser.cpp

clean:
	rm -f main *.o
