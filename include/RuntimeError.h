#ifndef RUNTIME_ERROR_H
#define RUNTIME_ERROR_H

#include <exception>
#include <string>

class RuntimeError : public std::exception
{
  public:
    RuntimeError();
    RuntimeError(const char *);
    RuntimeError(const std::string &);
    virtual const char *what() const noexcept;

  private:
    std::string msg;
};

#endif // !RUNTIME_ERROR_H