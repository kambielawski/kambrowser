#include <iostream>
#include <fstream>
#include <string>

struct HtmlToken {
  std::string token_type; // type of token
  std::string token_content; // inside token (i.e. 'hello' in "<p>hello</p>")
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
int stack_size;
char curChar;
TokenStackNode *token_stack; 

public:
HtmlParser();
~HtmlParser();
void parseHtmlString (char *html_string, int length);

protected:
void getNextChar ();
void getToken ();
void skipWhitespace();
char peekNext() const;
std::string readUntil(char until);

// token stack management
void tokenStackPush(struct HtmlToken token);
void tokenStackPop();

};

#endif

