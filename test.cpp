
#include "time.h"
#include "unistd.h"
#include <ctime>
#include <iostream>
extern "C"

void printDate() {
  std::time_t local = std::time(nullptr);
  std::cout << "Hello from printDate " << std::ctime(&local) << '\n';
}

#include <string>
extern "C" {

int rust_function(const std::string & s1, std::string &s2) {
  s2="'This should have been the sha but it had been replaced'";
  return 2;
}
} // extern "C"
