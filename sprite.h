#pragma once
#include <SDL2/SDL.h>

class renderer;


class sprite{
protected:
	SDL_Rect pos;
	bool isAnimated;
	renderer* r;

public:
	sprite( renderer* r, unsigned int img ,  SDL_Rect pos = {0, 0, 0, 0} );

	void move( int x, int y );
	void moveOffset( int x, int y );
	void resize( int w, int h );
	void resizeOffset( int w, int h );
	Uint32 img;
	virtual void animationStopped();
	
	friend class renderer;
};