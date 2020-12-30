#include <iostream>
#include <fstream>
#include <string>

struct HtmlToken {
  std::string tokenType; // type of token
  std::string tokenContent; // inside token (i.e. 'hello' in "<p>hello</p>")
};

struct TokenStackNode {
  HtmlToken token;
  TokenStackNode *next;
};

#ifndef HTML_PARSER_H
#define HTML_PARSER_H

class HtmlParser {
private:
char *html_string;
int string_pos;
int string_length;
char curChar;
TokenStackNode *tokenStack; 

public:
HtmlParser();
~HtmlParser();
void parseHtmlString (char *html_string, int length);
void getNextChar ();
void getToken ();
void skipWhitespace();
char peekNext() const;
std::string readUntil(char until);

};

#endif

