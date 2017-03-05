#ifndef SDL_H
#define SDL_H

#ifdef __unix
	#include <SDL2/SDL.h>
#else
	#include <SDL.h>
#endif
#undef main // this prevents including default main() from SDL

class SDL 
{
public:
	SDL(uint32_t flags = 0);
	virtual ~SDL();
};

#endif //!SDL_H