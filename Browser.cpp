#include "Browser.h"

Browser::Browser() {}

Browser::~Browser() {}

void Browser::run() {
  std::string url;
  std::cout << "=============\n";
  std::cout << "KAMBrowser\n";

  while (url != "quit") {
    std::cout << "=============\n";
    std::cout << ">> ";
    std::getline(std::cin, url);
    std::cout << "=============\n";

    if (url != "quit") {
      std::cout << "Sending HTTP request to " << url << std::endl;
      this->sendHttpRequest(url);
    }
  }

  std::cout << "=============\n";
  std::cout << "Exiting...\n";
  std::cout << "=============\n";
   
} 

void Browser::sendHttpRequest(std::string url) {
  std::cout << url << std::endl;
  /* fork this process and use child to execute http request */
  int rc = fork();
  if (rc < 0) {
    throw("failed to fork");
  } else if (rc == 0) {
    char *requestArgs[2];
    requestArgs[0] = "./networking/http_request";
    requestArgs[1] = &url[0];
    requestArgs[2] = NULL;
    /* execute http request */
    execvp(requestArgs[0], requestArgs);
  } else {
    wait(NULL);
    this->parseResponse();
  }
}

void Browser::parseResponse() {
  response.open("response.txt");
  headers.open("response_headers.txt");
  body.open("response_body.txt");

  std::string line; 
  bool isBody = false;

  /* separate headers & body from response.txt */ 
  while (!response.eof()) {
    std::getline(response, line);
    /* check for carriage return (x0d) or linefeed (x0a) */
    if (line == "\x0a" || line == "\x0d") {
      isBody = true;
      continue;
    }

    if (isBody) {
      body << line << "\n";
    } else {
      headers << line << "\n";
    }
  }

  response.close();
}
