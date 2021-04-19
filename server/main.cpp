#include <iostream>
#include "server.h"

using namespace std;

#define IP "127.0.0.1"
#define PORT 8000

int main() {
  Server s(IP, PORT);
  
  return 0;
}
