#pragma once

#include <vector>
#include <map>
#include <list>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprite.h"

struct animation{
	sprite* obj;
	SDL_Rect start;
	SDL_Rect end;
	unsigned int begin;
	unsigned int stop;
};

class renderer{
	SDL_Window* window;

	SDL_Renderer* r;

	std::map<unsigned int, SDL_Texture*> tex;
	std::list<sprite*> objs;
	std::list<animation> animations;
	public:
	SDL_PixelFormat* format;
	renderer();
	bool init( const char* caption, int width, int height );
	bool loadImg( const char* fname, unsigned int nr );
	void register_obj( sprite* obj );
	void renderFrame();
	void animate( sprite* obj, SDL_Rect target, unsigned int time );
	~renderer();
};