
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

const char * rust_function(const char *s){
  std::cout<<"Replaced!"<<std::endl;
  const char * ret= new char[10];
  ret="Replaced!";
  return ret;
}
} // extern "C"
