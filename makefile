browser: main.o Browser.o HtmlParser.o
	g++ -std=c++11 main.o Browser.o html_parser/HtmlParser.o -o browser

main.o: main.cpp Browser.h
	g++ -c main.cpp

Browser.o: Browser.cpp Browser.h
	g++ -c Browser.cpp

HtmlParser.o: HtmlParser.cpp HtmlParser.h
	g++ -c html_parser/HtmlParser.cpp

all:
	$(MAKE) -C networking
	$(MAKE) -C html_parser

clean:
	rm -f main *.o
	cd html_parser && rm -f HtmlParser.o
