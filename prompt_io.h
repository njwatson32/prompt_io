#ifndef __PROMPT_IO_H__
#define __PROMPT_IO_H__

#include <termios.h>
#include <iostream>
#include <vector>
#include <string>
#include <boost/signals2/mutex.hpp>

class prompt_io {

  std::string prompt_;
  unsigned int promptSize_;
  std::vector<char> buf_;
  boost::signals2::mutex ioLock_;
  struct termios saved_;
  struct termios raw_;
  bool eof_;
  // Indicates whether we are currently receiving input, which output
  // uses to decide whether to move the prompt
  bool receivingInput_;

public:
  // Create a prompt_io with the given prompt
  prompt_io(const std::string &_prompt = "");
  // Disable copy constructors
  prompt_io(const prompt_io &other) = delete;

  // Destroy a prompt_io and reset I/O settings
  ~prompt_io();

  // Set the prompt
  void set_prompt(const std::string &_prompt);

  // Output data to stdout
  template <typename T>
  prompt_io &operator<<(const T &val);
  prompt_io &operator<<(std::ostream& (*pf)(std::ostream &));
  prompt_io &operator<<(std::ios& (*pf)(std::ios &));
  prompt_io &operator<<(std::ios_base& (*pf)(std::ios_base &));

  // Gets a line of input delimeted by a newline, optionally including the
  // newline character at the end. Sets EOF = 1 iff Ctrl-D is pressed.
  std::string getline(bool includeNL = false);

  // Sees whether EOF was set during the last call to getline.
  bool eof() const { return eof_; }

private:
  // Erases input and prompt from the screen (but not from buffer)
  void delete_input();
};

#endif // __PROMPT_IO_H__
