#include "Browser.h"

Browser::Browser() {}

Browser::~Browser() {}

void Browser::run() {
  std::string url;
  std::cout << "=============\n";
  std::cout << "KAMBrowser\n";
  std::cout << "=============\n";

  while (url != "quit") {
    std::cout << ">> ";
    std::getline(std::cin, url);
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
  int rc = fork();
  if (rc < 0) {
    throw("failed to fork");
  } else if (rc == 0) {
    char *httpRequest[2];
    httpRequest[0] = "./networking/http_request";
    httpRequest[1] = &url[0];
    httpRequest[2] = NULL;
    execvp(httpRequest[0], httpRequest);
  } else {
    wait(NULL);
  }
}
