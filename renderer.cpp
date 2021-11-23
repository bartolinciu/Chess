#include "renderer.h"
#include "chess.h"


renderer::renderer(){
	this->window = NULL;
	this->r = NULL;
}

bool renderer::init( const char* caption, int width, int height ){
	if( SDL_Init( SDL_INIT_EVERYTHING ) < 0 ){
		printf("Initialisation failed! Error:%s\n", SDL_GetError());
		return false;
	}

	this->window = SDL_CreateWindow( caption, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_SHOWN );
	if( this->window == NULL ){
		printf( "Couldn't create window! Error: %s\n", SDL_GetError() );
		return false;
	}
	
	this->r = SDL_CreateRenderer( this->window, -1, SDL_RENDERER_ACCELERATED );
	if(this->r == NULL){
		printf( "Renderer couldn't be created! SDL Error: %s\n", SDL_GetError() );
		return false;
	}




	SDL_SetRenderDrawColor( this->r, 0x44, 0xf0, 0xFF, 0xff );

	if( !( IMG_Init( IMG_INIT_PNG ) & IMG_INIT_PNG ) ){
		printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
		return false;
	}

	SDL_Surface* srf = SDL_CreateRGBSurface( 0, 52, 52, 32, 0xff000000, 0xff0000, 0xff00, 0xff );
	SDL_SetSurfaceBlendMode( srf, SDL_BLENDMODE_BLEND );
	SDL_FillRect( srf, NULL, SDL_MapRGBA( srf->format, 0xff, 114, 114, 127 ) );
	this->tex[HIGHLIGHT] = SDL_CreateTextureFromSurface( this->r, srf );
	SDL_FreeSurface(srf);

	this->tex[NONE] = NULL;

	return true;
}

bool renderer::loadImg( const char* fname, unsigned int nr ){
	printf("Loading img \"%s\" as %u\n", fname, nr);
	SDL_Surface* image;
	SDL_Texture* newTexture;
	image = IMG_Load(fname);
	if(!image) {
		printf( "Failed to load image: \"%s\"! Error: %s",fname, IMG_GetError() );
		SDL_FreeSurface(image);
		return false;
	}
	
	newTexture = SDL_CreateTextureFromSurface( this->r, image );
	if( newTexture == NULL ){
		printf( "Unable to crate texture from %s! SDL Error: %s\n", fname, SDL_GetError() );
		SDL_FreeSurface( image );
		return false;
	}

	this->tex[nr] = newTexture;

	SDL_FreeSurface( image );

	return true;
}

void renderer::animate( sprite* obj, SDL_Rect target, unsigned int time ){
	unsigned int now = SDL_GetTicks();
	this->animations.push_back( { obj, obj->pos, target, now, now + time } );
	obj->isAnimated = true;
}

void renderer::register_obj( sprite* obj ){
	this->objs.push_back( obj );
}

void renderer::renderFrame(){

	animation a;
	std::list< std::list<animation>::iterator > to_delete;
	for( auto i = this->animations.begin(); i != this->animations.end(); i++ ){
		a = *i;
		float ratio = (float)( SDL_GetTicks() - a.begin )/(float)( a.stop - a.begin );
		if( ratio > 1.0 ){
			ratio = 1.0;
			a.obj->isAnimated = false;
			a.obj->animationStopped();
			to_delete.push_back( i );
		}
		a.obj->pos.x = a.start.x + ( a.end.x - a.start.x )*ratio;
		a.obj->pos.y = a.start.y + ( a.end.y - a.start.y )*ratio;
		a.obj->pos.w = a.start.w + ( a.end.w - a.start.w )*ratio;
		a.obj->pos.h = a.start.h + ( a.end.h - a.start.h )*ratio;
		//printf( "%f %i %i %i %i\n", ratio, a.obj->pos.x, a.obj->pos.y, a.obj->pos.w, a.obj->pos.h );
	}

	for( auto i = to_delete.begin(); i != to_delete.end(); i++ ){
		this->animations.erase( *i );
	}
	//printf(".");
	//SDL_FillRect( screenSurface, NULL, SDL_MapRGBA( screenSurface->format, 0x44, 0xf0, 0xFF, 0xff ) );

	SDL_RenderClear( this->r );

	for( auto i = this->objs.begin(); i != this->objs.end(); i++ ){
			SDL_RenderCopy( this->r, this->tex[(*i)->img], NULL, &(*i)->pos );
		}


	SDL_RenderPresent( this->r );
	//SDL_UpdateWindowSurface( window );
}

renderer::~renderer(){
	for( auto i = this->tex.begin(); i != this->tex.end(); i++ )
		SDL_DestroyTexture( i->second );
	this->tex.erase( this->tex.begin(), this->tex.end() );
	SDL_DestroyWindow( this->window );
	SDL_DestroyRenderer( this->r );
	SDL_Quit();
}