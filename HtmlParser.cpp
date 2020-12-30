#include "HtmlParser.h"

HtmlParser::HtmlParser() : string_pos(-1), tokenStack(nullptr) {}

HtmlParser::~HtmlParser() {}

void HtmlParser::parseHtmlString (char *string, int length) {
  // TODO: ensure valid html
  std::cout << string << std::endl;
  html_string = string;
  string_length = length;
  this->getNextChar();
  this->getToken();
}

void HtmlParser::getToken () {
  this->skipWhitespace();

  if (curChar == '\0') {
    std::cout << "eof\n";
    return;
  } else if (curChar == '<') {
    this->getNextChar();
    if (curChar != '/') {
      HtmlToken nextToken;
      // opening tag
      nextToken.tokenType = this->readUntil('>');
      // TODO: push token to top of stack
      std::cout << nextToken.tokenType << std::endl;
      this->getNextChar();
      this->getToken();
    } else {
      // closing tag
      // ensure closes correct tokenType (check top of tokenStack)
      std::string closingTag = this->readUntil('>');
      std::cout << closingTag << "\n";
      this->getNextChar();
      this->getToken();
    }
  } else {
    // parse content and add to top of tokenStack's content string
    std::string content = this->readUntil('<');
    std::cout << content << "\n";
    this->getToken();
  }
}

std::string HtmlParser::readUntil (char until) {
  std::string ret;
  while (curChar != until) {
    ret.append(1, curChar);
    this->getNextChar();
  }
  return ret;
}

void HtmlParser::getNextChar () {
  string_pos++; 
  if (string_pos <= string_length)
    curChar = html_string[string_pos];
}

void HtmlParser::skipWhitespace () {
  if (curChar == '\n' || curChar == ' ' || curChar == '\r') {
    this->getNextChar();
  }
}

char HtmlParser::peekNext() const {
  if (string_pos > string_length)
    throw std::runtime_error("peeked off end of html string\n");
  return html_string[string_pos+1];
}


