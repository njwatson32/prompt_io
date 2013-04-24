#include "prompt_io.h"

#include <termios.h>
#include <iostream>
#include <string>

#define BS 8
#define DEL 127
#define ERASE "\b \b"

#ifdef _WIN32
#define NL "\r\n"
#elif defined macintosh
#define NL "\r"
#else
#define NL "\n"
#endif

using namespace std;
using namespace boost;

prompt_io::prompt_io(const std::string &_prompt) :
  prompt_(_prompt), promptSize_(_prompt.size()), eof_(false) {
  receivingInput_ = false;
  // Save and turn off echoing and canonical mode
  tcgetattr(fileno(stdin), &saved_);
  tcgetattr(fileno(stdin), &raw_);
  //raw_.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR |
  //                  ICRNL | IXON);
  raw_.c_lflag &= ~(ECHO | ICANON);
  tcsetattr(fileno(stdin), TCSANOW, &raw_);
}

prompt_io::~prompt_io() {
  // Restore original settings
  tcsetattr(fileno(stdin), TCSANOW, &saved_);
}

void prompt_io::set_prompt(const string &_prompt) {
  ioLock_.lock();
  prompt_ = _prompt;
  promptSize_ = prompt_.size();
}

void prompt_io::delete_input() {
  unsigned int n = buf_.size();
  string bs (n + promptSize_, BS);
  string ws (n + promptSize_, ' ');
  cout << bs + ws + bs;
}

// This works with a template but not a function
// Delete written characters so far, print the output, reprint the characters
#define MY_COUT(X)                                                      \
  ioLock_.lock();                                                       \
  if (!receivingInput_)                                                 \
    cout << X;                                                          \
  else {                                                                \
    delete_input();                                                     \
    cout << X << prompt_ << string(buf_.begin(), buf_.end()) << flush;  \
  }                                                                     \
  ioLock_.unlock();                                                     \
  return *this

template <typename T>
prompt_io &prompt_io::operator<<(const T &val) {
  MY_COUT(val);
}

// For IO manipulators
prompt_io &prompt_io::operator<<(ostream& (*pf)(ostream &)) {
  MY_COUT(pf);
}
prompt_io &prompt_io::operator<<(ios& (*pf)(ios &)) {
  MY_COUT(pf);
}
prompt_io &prompt_io::operator<<(ios_base& (*pf)(ios_base &)) {
  MY_COUT(pf);
}

string prompt_io::getline(bool includeNL) {
  ioLock_.lock();
  receivingInput_ = true;
  cout << prompt_ << flush;
  ioLock_.unlock();
  eof_ = false;

  // Get one character at a time
  char c = fgetc(stdin);
  while (c != 10 && c != 13) {
    ioLock_.lock();
    // Print printable characters
    if (c >= 32 && c <= 126) {
      putchar(c);
      buf_.push_back(c);    
    }
    // Handle backspaces
    else if ((c == BS || c == DEL) && buf_.size() > 0) {
      fputs(ERASE, stdout);      
      buf_.pop_back();
    }
    // EOF
    else if (c == 4) {
      eof_ = true;
      ioLock_.unlock();
      break;
    }
    ioLock_.unlock();
    c = fgetc(stdin);
  }
  // Create string to return and clear buffer
  string line (buf_.begin(), buf_.end());
  if (includeNL)
    line += NL;
  ioLock_.lock();
  delete_input();
  receivingInput_ = false;
  buf_.clear();
  ioLock_.unlock();
  return line;
}
