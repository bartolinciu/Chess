#include "sprite.h"
#include "renderer.h"

sprite::sprite( renderer* r, unsigned int img ,  SDL_Rect pos){
		this->img = img;
		this->pos = pos;
		r->register_obj(this);
		this->r = r;
		this->isAnimated = false;
	}



void sprite::move( int x, int y ){
		this->pos.x = x;
		this->pos.y = y;
	}

void sprite::moveOffset( int x, int y ){
		this->pos.x += x;
		this->pos.y += y;
	}

void sprite::resize( int w, int h ){
		this->pos.w = w;
		this->pos.h = h;
	}

void sprite::resizeOffset( int w, int h ){
		this->pos.w += w;
		this->pos.h += h;
	}