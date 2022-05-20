#ifndef SDL_UTILS__H
#define SDL_UTILS__H

#include <iostream>
#include <SDL.h>
#include <SDL_image.h>

using namespace std;
void logSDLError(std::ostream& os,
                 const std::string &msg, bool fatal = false);
SDL_Texture* loadTexture( string path, SDL_Renderer* renderer );

void initSDL(SDL_Window* &window, SDL_Renderer* &renderer,
	int screenWidth, int screenHeight, const char* windowTitle) ;

void quitSDL(SDL_Window* window, SDL_Renderer* renderer);

void waitUntilKeyPressed();

#endif
