#define MAX_REQ_LENGTH 1024

#ifndef BROWSER_H
#define BROWSER_H

#include <string>
#include <cstring>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/wait.h>

class Browser {
private:
std::ifstream  response;
std::ofstream  headers;
std::ofstream  body;

public:
Browser();
~Browser();

void run();
void sendHttpRequest(std::string url);
void parseResponse();

};

#endif
