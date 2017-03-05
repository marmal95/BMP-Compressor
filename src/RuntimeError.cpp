#include "RuntimeError.h"
#include "SDL_Local.h"

// @construct
RuntimeError::RuntimeError()
    : exception(), msg(SDL_GetError()) {}
RuntimeError::RuntimeError(const char *m)
    : exception(), msg(m) {}
RuntimeError::RuntimeError(const std::string &m)
    : exception(), msg(m) {}

// @implement
const char *RuntimeError::what() const noexcept
{
    return msg.c_str();
}
