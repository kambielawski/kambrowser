#define MAX_REQ_LENGTH 1024

#ifndef BROWSER_H
#define BROWSER_H

#include <string>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/wait.h>

class Browser {
private:

public:
Browser();
~Browser();

void run();
void sendHttpRequest(std::string url);

};

#endif
