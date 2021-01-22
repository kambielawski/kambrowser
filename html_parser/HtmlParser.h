#include <iostream>
#include <fstream>
#include <string>
#include <stdlib.h>
#include <stdio.h>

#include "DomTree.h"
/*
Attribute struct,
DomNode struct,
enum DomNodeType
*/

struct HtmlToken {
    std::string token_type;         /* type of token (string) */
    std::string token_content;      /* inside token (i.e. 'hello' in "<p>hello</p>") */
    std::string token_attributes;   /* attributes of html tag */ 
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
    std::string doctype;

  public:
    HtmlParser();
    ~HtmlParser();
    void parseHtmlString (char *html_string);
    void parseHtmlFromFile (const char *file_name);

  protected:
    int getStrLen(char *string) const;
    void getNextChar ();
    void getToken ();
    void skipWhitespace();
    char peekNext() const;
    std::string readUntil(char until);
    void ignoreComment ();
    bool checkValidDoctype () const;
    void parseDoctype();
    void emitDomNode(HtmlToken token);

    // token stack management
    void tokenStackPush(struct HtmlToken token);
    void tokenStackPop();

};

#endif

