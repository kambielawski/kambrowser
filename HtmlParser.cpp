#include "HtmlParser.h"

HtmlParser::HtmlParser() : string_pos(-1), stack_size(1)
{
  HtmlToken root_token = { "root", "" };

  token_stack = new TokenStackNode;
  token_stack->token = root_token;
  token_stack->next = nullptr;
}

HtmlParser::~HtmlParser() {}

void HtmlParser::parseHtmlString (char *string, int length) {
  // TODO: ensure valid html
  html_string = string;
  string_length = length;
  this->getNextChar();
  this->getToken();
}

void HtmlParser::getToken () {
  this->skipWhitespace();

  if (curChar == '\0') // end of file
  {
    // pop root node 
    this->tokenStackPop();
    return;
  } 
  else if (curChar == '<') // html tag
  {
    this->getNextChar();
    if (curChar != '/') // opening tag
    {
      HtmlToken nextToken;
      nextToken.token_type = this->readUntil('>');
      this->tokenStackPush(nextToken);

      this->getNextChar();
      this->getToken();
    } 
    else // closing tag
    {
      this->getNextChar();

      std::string closingTag = this->readUntil('>');
      this->tokenStackPop();

      this->getNextChar();
      this->getToken();
    }
  } 
  else // content between tags
  {
    // parse content and add to top of token_stack's content string
    std::string content = this->readUntil('<');
    token_stack->token.token_content = content;
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

void HtmlParser::tokenStackPush(struct HtmlToken token)
{
  struct TokenStackNode *new_node = new TokenStackNode;
  new_node->token = token;
  new_node->next = token_stack;
  token_stack = new_node;
  stack_size++;

  new_node = nullptr;
}

void HtmlParser::tokenStackPop()
{
  struct TokenStackNode *temp = token_stack->next;

  if (!token_stack->token.token_content.empty())
  {
    std::cout << token_stack->token.token_content << std::endl;
  }

  delete token_stack;
  token_stack = temp;
  stack_size--;

  temp = nullptr;
}
