#pragma once
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include "sprite.h"
#include <vector>
#include <list>

class chessBoard;
class chessPiece;
class renderer;

#define NONE 0
#define HIGHLIGHT 1
#define BOARD 2

#define BLACK_ROOK 10
#define BLACK_KNIGHT 11
#define BLACK_BISHOP 12
#define BLACK_KING 13
#define BLACK_QUEEN 14
#define BLACK_PAWN 15

#define WHITE_ROOK 20
#define WHITE_KNIGHT 21
#define WHITE_BISHOP 22
#define WHITE_KING 23
#define WHITE_QUEEN 24
#define WHITE_PAWN 25


struct tile{
	chessPiece* piece;
	sprite* highlight;
};

struct move_t{
	int col;
	int row;
};


class chessPiece: public sprite{
	protected:
	int col;
	int row;

	chessBoard* board;
	std::vector<move_t> _availableMoves;

	chessPiece* beaten;
	bool beating;

	public:

	enum{
		PAWN,
		ROOK,
		BISHOP,
		KNIGHT,
		KING,
		QUEEN
	} type;

	bool first_move;

	bool white;
	chessPiece( bool white, int piece, int col, int row, chessBoard* board );
	void forceMove( int col, int row );
	virtual void move( int col, int row );
	bool isMoveAvailable(int col, int row);
	void beat();
	void animationStopped();
	virtual std::vector<move_t> getAvailableMoves()=0;
	virtual std::vector<move_t> getBeatingRange()=0;
	friend class chessBoard;
};

class chessBoard: public sprite{
	void highlight( int col, int row );
	void clearHighlight();


	bool sthSelected;
	move_t last;
	public:
	chessPiece* black_king;
	chessPiece* white_king;

	void animationStopped();
	std::list<chessPiece*> beaten_whites;
	std::list<chessPiece*> beaten_blacks;
	tile board[8][8];
	chessBoard(renderer* r);
	void handleClick( SDL_MouseButtonEvent e );
	bool isHovered( int x, int y );
	bool nowMovesWhite;
	std::list<chessPiece*> whites;
	std::list<chessPiece*> blacks;

	bool isCheck();
	bool isMate();

	friend class chessPiece;
};

class rook: public chessPiece{
	public:
	rook( int col, int row, bool white, chessBoard* board );
	std::vector<move_t> getAvailableMoves();
	std::vector<move_t> getBeatingRange();
};

class knight: public chessPiece{
	public:
	knight( int col, int row, bool white, chessBoard* board );
	std::vector<move_t> getAvailableMoves();
	std::vector<move_t> getBeatingRange();
};

class bishop: public chessPiece{
	public:
	bishop( int col, int row, bool white, chessBoard* board );
	std::vector<move_t> getAvailableMoves();
	std::vector<move_t> getBeatingRange();
};

class king: public chessPiece{
	public:
	king( int col, int row, bool white, chessBoard* board );
	std::vector<move_t> getAvailableMoves();
	std::vector<move_t> getBeatingRange();
	void move( int col, int row );
};

class pawn: public chessPiece{
	public:
	pawn( int col, int row, bool white, chessBoard* board );
	std::vector<move_t> getAvailableMoves();
	std::vector<move_t> getBeatingRange();
};

class queen: public chessPiece{
	public:
	queen( int col, int row, bool white, chessBoard* board );
	std::vector<move_t> getAvailableMoves();
	std::vector<move_t> getBeatingRange();
};