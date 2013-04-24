#include "prompt_io.h"

#include <iostream>
#include <string>
#include <boost/bind.hpp>
#include <boost/thread/thread.hpp>

void do_io(prompt_io *pio) {
  prompt_io &p = *pio;
  while (1) {
    sleep(5);
    p << "Other thread prints!" << std::endl;
  }
}

int main() {
  prompt_io pio ("prompt> ");
  boost::thread t (boost::bind(&do_io, &pio));
  while (!pio.eof()) {
    std::string l = pio.getline();
    pio << l << std::endl;
  }
  return 0;
}
