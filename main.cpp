#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <cstdio>
#include "renderer.h"
#include "chess.h"

#define SWIDTH 800
#define SHEIGHT 600



int WinMain(){
	bool quit = false;

	renderer r;
	if( !r.init( "Chess", SWIDTH, SHEIGHT ) )
		return 1;
	r.loadImg( "gfx/chessboard.png", BOARD );
	r.loadImg( "gfx/white_rook.png", WHITE_ROOK );
	r.loadImg( "gfx/white_knight.png", WHITE_KNIGHT );
	r.loadImg( "gfx/white_bishop.png", WHITE_BISHOP );
	r.loadImg( "gfx/white_queen.png", WHITE_QUEEN );
	r.loadImg( "gfx/white_king.png", WHITE_KING );
	r.loadImg( "gfx/black_queen.png", BLACK_QUEEN );
	r.loadImg( "gfx/white_pawn.png", WHITE_PAWN );
	r.loadImg( "gfx/black_king.png", BLACK_KING );

	chessBoard board(&r);

	SDL_Event e;

	while( !quit ){

		while( SDL_PollEvent( &e ) != 0 ){

			switch( e.type ){

				case SDL_QUIT:
					quit = true;
					break;

				case SDL_MOUSEBUTTONDOWN:
					if( board.isHovered( e.button.x, e.button.y ) ){
						board.handleClick( e.button );
					}
					break;
			}
		}

		//board.draw( screenSurface );
		r.renderFrame();
	}
	return 0;
}