#include "html_parser/HtmlParser.h"
#include "Browser.h"
#include <string>

int main (int argc, char *argv[]) {
  HtmlParser parser;
  parser.parseHtmlFromFile("response_body.txt");
//  Browser kambrowser;
//  kambrowser.run();
 
  return (0);
}
