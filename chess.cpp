#include "chess.h"
#include "renderer.h"

void wrongMove(){
	printf("wrong\n");
};

chessBoard::chessBoard( renderer* r ):sprite( r, BOARD, { 150, 50, 500, 500 }){
		this->nowMovesWhite = true;
		for( int i=0; i<8; i++ ){
			for( int j = 0; j<8; j++ ){
				this->board[i][j] = { NULL, new sprite( r,NONE, { this->pos.x +42 + i*52, this->pos.y + 42 + j*52, 52, 52 } ) };
			}
		}

		this->board[0][0].piece = new rook( 0, 0, true, this );
		this->board[7][0].piece = new rook( 7, 0, true, this );
		//this->board[1][0].piece = new knight( 1, 0, true, this );
		//this->board[6][0].piece = new knight( 6, 0, true, this );
		//this->board[2][0].piece = new bishop( 2, 0, true, this );
		//this->board[5][0].piece = new bishop( 5, 0, true, this );
		this->board[3][0].piece = new queen( 3, 0, true, this );
		this->board[4][0].piece = new king( 4, 0, true, this );
		this->board[4][7].piece = new king( 4, 7, false, this );
		this->board[3][7].piece = new queen( 3, 7, false, this );

		//for( int i = 0; i < 8; i++ )
		//	this->board[i][1].piece = new pawn( i, 1, true, this );

		this->sthSelected = false;
	};

void chessBoard::handleClick( SDL_MouseButtonEvent e ){
	int col = (e.x - ( this->pos.x + 42 ))/52;
	int row = (e.y - ( this->pos.y + 42 ))/52;
	if( col > 7 || row > 7  )
		return;
	if( !this->sthSelected ){
		if( this->board[col][row].piece != NULL && this->nowMovesWhite == this->board[col][row].piece->white ){
			std::vector<move_t>availableMoves = this->board[col][row].piece->getAvailableMoves();
			for( move_t i : availableMoves ){
				this->highlight( i.col, i.row );
			}
			this->last = { col, row };
			this->sthSelected = true;
		}
	}
	else{
		if( last.col != col || last.row != row )
			this->board[last.col][last.row].piece->move(col, row);
		this->clearHighlight();
		this->sthSelected = false;
	}
}

bool chessBoard::isHovered( int x, int y ){
	return x > this->pos.x+42 && x < this->pos.x + this->pos.w-42 && y > this->pos.y+42 && y < this->pos.y + this->pos.h-42;
}

void chessBoard::highlight( int col, int row ){
	if( col > 7 || row > 7 )
		return;
	this->board[ col ][ row ].highlight->img = HIGHLIGHT;
}

void chessBoard::clearHighlight(){
	for( int i = 0; i < 8; i++ )
		for( int j = 0; j < 8; j++ )
			this->board[i][j].highlight->img = NONE;
}

chessPiece::chessPiece( bool white, int piece, int col, int row, chessBoard* board ): sprite( board->r, piece + white * 10, { board->pos.x+42 + col*52, board->pos.y+42 + row*52, 52, 52 } ) {
		this->beating = false;
		this->board = board;
		this->col = col;
		this->row = row;
		this->white = white;
		if( white )
			board->whites.push_back(this);
		else
			board->blacks.push_back(this);
		this->first_move = true;
	}

void chessPiece::move( int col, int row ){
		if( !this->isMoveAvailable( col, row ) || this->isAnimated ){
			wrongMove();
			return;
		}
		if( this->board->board[col][row].piece != NULL ){
			this->beating = true;
			this->beaten = this->board->board[col][row].piece;
		}
		this->forceMove(col, row);
		this->board->nowMovesWhite = !this->white;
		if(this->board->isCheck()){
			printf("check");
			if( this->board->isMate() )
				printf(" mate");
			printf("\n");
		}

	}

bool chessBoard::isCheck(){
	for( chessPiece* piece:this->whites ){
		for( move_t move:piece->getAvailableMoves() ){
			if( move.col == this->black_king->col && move.row == this->black_king->row )
				return true;
		}
	}

	for( chessPiece* piece:this->blacks ){
		for( move_t move:piece->getAvailableMoves() ){
			if( move.col == this->white_king->col && move.row == this->white_king->row )
				return true;
		}
	}
	return false;
}

bool chessBoard::isMate(){
	if( this->white_king->getAvailableMoves().size() == 0 || this->black_king->getAvailableMoves().size() == 0 )
		return true;

	return false;
}

void chessPiece::animationStopped(){
	if( this->beating )
		this->beaten->beat();
	this->beating = false;
}

void chessPiece::forceMove( int col, int row ){
	this->board->board[this->col][this->row].piece = NULL;
	this->col = col;
	this->row = row;
	this->board->board[this->col][this->row].piece = this;
	this->r->animate( this, { this->board->pos.x + 42 + col*52, this->board->pos.y + 42 + row*52, this->pos.w, this->pos.h }, 500 );
	this->first_move = false;
}

rook::rook( int col, int row, bool white, chessBoard* board ):chessPiece( white, BLACK_ROOK, col, row, board ){
	this->type = ROOK;
}
knight::knight( int col, int row, bool white, chessBoard* board ):chessPiece( white, BLACK_KNIGHT, col, row, board ){
	this->type = KNIGHT;
}
bishop::bishop( int col, int row, bool white, chessBoard* board ):chessPiece( white, BLACK_BISHOP, col, row, board ){
	this->type = BISHOP;
}
king::king( int col, int row, bool white, chessBoard* board ):chessPiece( white, BLACK_KING, col, row, board ){
	this->type = KING;
	if(white)
		board->white_king = this; 
	else
		board->black_king = this;
}
queen::queen( int col, int row, bool white, chessBoard* board ):chessPiece( white, BLACK_QUEEN, col, row, board ){
	this->type = QUEEN;
}
pawn::pawn( int col, int row, bool white, chessBoard* board ):chessPiece( white, BLACK_PAWN, col, row, board ){
	this->type = PAWN;
}

void chessPiece::beat(){
	this->r->animate( this, { this->board->pos.x + ( this->white ? 505 : - 57), this->board->pos.y + (int)( this->white ? this->board->beaten_whites : this->board->beaten_blacks ).size()*52, this->pos.w, this->pos.h }, 500 );
	( this->white ? this->board->beaten_whites : this->board->beaten_blacks ).push_back(this);
	( this->white ? this->board->whites : this->board->blacks ).remove( this );
}

bool chessPiece::isMoveAvailable(int col, int row){
	bool available = false;
	for( move_t i : this->_availableMoves ){
		if( i.col == col && i.row == row ){
			available = true;
			break;
		}
	}
	return available;
}

std::vector<move_t> rook::getAvailableMoves(){
	std::vector<move_t> availableMoves;
	for( int i = this->col+1; i < 8; i++ ){
		if( this->board->board[i][this->row].piece == NULL )
			availableMoves.push_back( { i, this->row } );
		else{
			if( this->board->board[i][this->row].piece->white != this->white )
				availableMoves.push_back( { i, this->row } );
			break;
		}
	}

	for( int i = this->col-1; i > -1; i-- ){
		if( this->board->board[i][this->row].piece == NULL )
			availableMoves.push_back( { i, this->row } );
		else{
			if( this->board->board[i][this->row].piece->white != this->white )
				availableMoves.push_back( { i, this->row } );
			break;
		}
	}

	for( int i = this->row+1; i < 8; i++ ){
		if( this->board->board[this->col][i].piece == NULL )
			availableMoves.push_back( { this->col, i } );
		else{
			if( this->board->board[this->col][i].piece->white != this->white )
				availableMoves.push_back( { this->col, i } );
			break;
		}
	}

	for( int i = this->row-1; i > -1; i-- ){
		if( this->board->board[this->col][i].piece == NULL )
			availableMoves.push_back( { this->col, i } );
		else{
			if( this->board->board[this->col][i].piece->white != this->white )
				availableMoves.push_back( { this->col, i } );
			break;
		}
	}

	this->_availableMoves = availableMoves;
	return availableMoves;
}
std::vector<move_t> pawn::getAvailableMoves(){
	std::vector<move_t> availableMoves;

	if( this->board->board[this->col][this->row + (this->white ? 1 : -1) ].piece == NULL || this->board->board[this->col][this->row + (this->white ? 1 : -1) ].piece->white != this->white )
		availableMoves.push_back( {this->col, this->row + (this->white ? 1 : -1 ) } );

	if( this->first_move && ( this->board->board[this->col][this->row + (this->white ? 2 : -2) ].piece == NULL || this->board->board[this->col][this->row + (this->white ? 2 : -2) ].piece->white != this->white ) )
		availableMoves.push_back( {this->col, this->row + (this->white ? 2 : -2 ) } );

	if( this->board->board[this->col-1][this->row + (this->white ? -1 : 1)].piece != NULL && this->board->board[this->col-1][this->row + (this->white ? -1 : 1)].piece->white!=this->white )
		availableMoves.push_back( {this->col-1, this->row + (this->white ? -1 : 1)} );

	if( this->board->board[this->col+1][this->row + (this->white ? -1 : 1)].piece != NULL && this->board->board[this->col+1][this->row + (this->white ? -1 : 1)].piece->white!=this->white )
		availableMoves.push_back( {this->col+1, this->row + (this->white ? -1 : 1)} );

	this->_availableMoves = availableMoves;
	return availableMoves;
}
std::vector<move_t> knight::getAvailableMoves(){
	std::vector<move_t> availableMoves;
	int col = this->col;
	int row = this->row;
	move_t moves[8] = {
		{ col - 1, row - 2 },
		{ col + 1, row - 2 },
		{ col - 1, row + 2 },
		{ col + 1, row + 2 },
		{ col - 2, row - 1 },
		{ col - 2, row + 1 },
		{ col + 2, row - 1 },
		{ col + 2, row + 1 }
	};
	for( int i = 0; i < 8; i++ ){
		if( -1 < moves[i].col && moves[i].col < 8 && -1 < moves[i].row && moves[i].row < 8 ){
			if( this->board->board[ moves[i].col ][ moves[i].row ].piece == NULL ){
				availableMoves.push_back( moves[i] );

			}
			else if( this->board->board[ moves[i].col ][ moves[i].row ].piece->white != this->white )
				availableMoves.push_back( moves[i] );
		}
	}

	

	this->_availableMoves = availableMoves;
	return availableMoves;
}
std::vector<move_t> bishop::getAvailableMoves(){
	std::vector<move_t> availableMoves;

	for( int i = this->col + 1, j = this->row + 1; i < 8 && j < 8; i++,j++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	for( int i = this->col + 1, j = this->row - 1; i < 8 && j > -1; i++,j-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	for( int i = this->col - 1, j = this->row + 1; i > -1 && j < 8; i--,j++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	for( int i = this->col - 1, j = this->row - 1; i > -1 && j > -1; i--,j-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	this->_availableMoves = availableMoves;
	return availableMoves;
}
std::vector<move_t> king::getAvailableMoves(){
	std::vector<move_t> availableMoves;
	int col = this->col;
	int row = this->row;
	move_t moves[8] = {
		{ col - 1, row - 1 },
		{ col, row - 1 },
		{ col + 1, row - 1 },
		{ col - 1, row },
		{ col + 1, row },
		{ col - 1, row + 1 },
		{ col, row + 1 },
		{ col + 1, row + 1 }
	};

	bool safe;
	for( int i = 0; i < 8; i++ ){
		if( -1 < moves[i].col && moves[i].col < 8 && -1 < moves[i].row && moves[i].row < 8 ){
			safe = true;

			for( chessPiece* piece: (this->white ? this->board->blacks : this->board->whites ) ){
				if( piece->type == chessPiece::KING )
					continue;
				for( move_t move : ( piece->type != chessPiece::KNIGHT && piece->type != chessPiece::BISHOP ? piece->getBeatingRange() : piece->getAvailableMoves() ) ){
					if( move.col == moves[i].col && move.row == moves[i].row ){
						safe = false;
					}
				}
			}

			if( !safe )
				continue;

			if( this->board->board[ moves[i].col ][ moves[i].row ].piece == NULL ){
				availableMoves.push_back( moves[i] );

			}
			else if( this->board->board[ moves[i].col ][ moves[i].row ].piece->white != this->white )
				availableMoves.push_back( moves[i] );
		}
	}

	if( this->first_move ){
		if( this->board->board[0][this->row].piece != NULL &&
		    this->board->board[0][this->row].piece->white == this->white &&
		    this->board->board[0][this->row].piece->type == chessPiece::ROOK && 
		    this->board->board[0][this->row].piece->first_move &&
		    this->board->board[this->col - 1][this->row].piece == NULL &&
		    this->board->board[this->col - 2][this->row].piece == NULL ){

			bool castling_safe = true;

			for( chessPiece* piece: (this->white ? this->board->blacks : this->board->whites ) ){
				for( move_t move : piece->getAvailableMoves() ){
					if( (move.col == this->col - 1 && move.row == this->row) || (move.col == this->col - 2 && move.row == this->row) ){
						castling_safe = false;
					}
				}
			}

			if( castling_safe )
				availableMoves.push_back( {this->col - 2, this->row} );

		}

		if( this->board->board[7][this->row].piece != NULL &&
		    this->board->board[7][this->row].piece->white == this->white &&
		    this->board->board[7][this->row].piece->type == chessPiece::ROOK && 
		    this->board->board[7][this->row].piece->first_move &&
		    this->board->board[this->col - 1][this->row].piece == NULL &&
		    this->board->board[this->col - 2][this->row].piece == NULL ){

			bool castling_safe = true;

			for( chessPiece* piece: (this->white ? this->board->blacks : this->board->whites ) ){
				for( move_t move : piece->getAvailableMoves() ){
					if( (move.col == this->col + 1 && move.row == this->row) || (move.col == this->col + 2 && move.row == this->row) ){
						castling_safe = false;
					}
				}
			}

			if( castling_safe )
				availableMoves.push_back( {this->col + 2, this->row} );

		}
	}
	

	this->_availableMoves = availableMoves;
	return availableMoves;
}
std::vector<move_t> queen::getAvailableMoves(){
	std::vector<move_t> availableMoves;

	for( int i = this->col + 1, j = this->row + 1; i < 8 && j < 8; i++,j++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	for( int i = this->col + 1, j = this->row - 1; i < 8 && j > -1; i++,j-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	for( int i = this->col - 1, j = this->row + 1; i > -1 && j < 8; i--,j++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	for( int i = this->col - 1, j = this->row - 1; i > -1 && j > -1; i--,j-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white != this->white )
				availableMoves.push_back( { i, j } );
			break;
		}
	}

	for( int i = this->col+1; i < 8; i++ ){
		if( this->board->board[i][this->row].piece == NULL )
			availableMoves.push_back( { i, this->row } );
		else{
			if( this->board->board[i][this->row].piece->white != this->white )
				availableMoves.push_back( { i, this->row } );
			break;
		}
	}

	for( int i = this->col-1; i > -1; i-- ){
		if( this->board->board[i][this->row].piece == NULL )
			availableMoves.push_back( { i, this->row } );
		else{
			if( this->board->board[i][this->row].piece->white != this->white )
				availableMoves.push_back( { i, this->row } );
			break;
		}
	}

	for( int i = this->row+1; i < 8; i++ ){
		if( this->board->board[this->col][i].piece == NULL )
			availableMoves.push_back( { this->col, i } );
		else{
			if( this->board->board[this->col][i].piece->white != this->white )
				availableMoves.push_back( { this->col, i } );
			break;
		}
	}

	for( int i = this->row-1; i > -1; i-- ){
		if( this->board->board[this->col][i].piece == NULL )
			availableMoves.push_back( { this->col, i } );
		else{
			if( this->board->board[this->col][i].piece->white != this->white )
				availableMoves.push_back( { this->col, i } );
			break;
		}
	}

	this->_availableMoves = availableMoves;
	return availableMoves;
}

std::vector<move_t> king::getBeatingRange(){
	int col = this->col;
	int row = this->row;
	//move_t moves[8] = ;
	return std::vector<move_t>({
		{ col - 1, row - 2 },
		{ col + 1, row - 2 },
		{ col - 1, row + 2 },
		{ col + 1, row + 2 },
		{ col - 2, row - 1 },
		{ col - 2, row + 1 },
		{ col + 2, row - 1 },
		{ col + 2, row + 1 }
	});
}
std::vector<move_t> pawn::getBeatingRange(){
	std::vector<move_t> availableMoves;
	availableMoves.push_back( { this->col-1, this->row + ( this->white ? -1 : 1 ) } );
	availableMoves.push_back( { this->col+1, this->row + ( this->white ? -1 : 1 ) } );
	return availableMoves;
}
std::vector<move_t> queen::getBeatingRange(){
	std::vector<move_t> availableMoves;

	for( int i = this->col + 1, j = this->row + 1; i < 8 && j < 8; i++,j++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING ){
				availableMoves.push_back( { i, j } );
				continue;
			}
			break;
		}
	}

	for( int i = this->col + 1, j = this->row - 1; i < 8 && j > -1; i++,j-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING )
				continue;
			break;
		}
	}

	for( int i = this->col - 1, j = this->row + 1; i > -1 && j < 8; i--,j++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING )
				continue;
			break;
		}
	}

	for( int i = this->col - 1, j = this->row - 1; i > -1 && j > -1; i--,j-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING )
				continue;
			break;
		}
	}

	for( int i = this->col+1; i < 8; i++ ){
		if( this->board->board[i][this->row].piece == NULL )
			availableMoves.push_back( { i, this->row } );
		else{
			if( this->board->board[i][this->row].piece->white == this->white )
				availableMoves.push_back( { i, this->row } );
			else if( this->board->board[i][this->row].piece->type == chessPiece::KING )
				continue;
			break;
		}
	}

	for( int i = this->col-1; i > -1; i-- ){
		if( this->board->board[i][this->row].piece == NULL )
			availableMoves.push_back( { i, this->row } );
		else{
			if( this->board->board[i][this->row].piece->white == this->white )
				availableMoves.push_back( { i, this->row } );
			else if( this->board->board[i][this->row].piece->type == chessPiece::KING )
				continue;
			break;
		}
	}

	for( int i = this->row+1; i < 8; i++ ){
		if( this->board->board[this->col][i].piece == NULL )
			availableMoves.push_back( { this->col, i } );
		else{
			if( this->board->board[this->col][i].piece->white == this->white )
				availableMoves.push_back( { this->col, i } );
			else if( this->board->board[this->col][i].piece->type == chessPiece::KING )
				continue;
			break;
		}
	}

	for( int i = this->row-1; i > -1; i-- ){
		if( this->board->board[this->col][i].piece == NULL )
			availableMoves.push_back( { this->col, i } );
		else{
			if( this->board->board[this->col][i].piece->white == this->white )
				availableMoves.push_back( { this->col, i } );
			else if( this->board->board[this->col][i].piece->type == chessPiece::KING )
				continue;
			break;
		}
	}

	this->_availableMoves = availableMoves;
	return availableMoves;
}
std::vector<move_t> knight::getBeatingRange(){}
std::vector<move_t> bishop::getBeatingRange(){}
std::vector<move_t> rook::getBeatingRange(){
	std::vector<move_t> availableMoves;
	int i,j;
	for( i = this->col+1, j = this->row; i < 8; i++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING ){
				availableMoves.push_back( { i, j } );
				continue;
			}
			break;
		}
	}

	for( i = this->col-1, j = this->row; i > -1; i-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING ){
				availableMoves.push_back( { i, j } );
				continue;
			}
			break;
		}
	}

	for( j = this->row+1, i = this->col; j < 8; j++ ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING ){
				availableMoves.push_back( { i, j } );
				continue;
			}
			break;
		}
	}

	for( j = this->row-1, i = this->col; j > -1; j-- ){
		if( this->board->board[i][j].piece == NULL )
			availableMoves.push_back( { i, j } );
		else{
			if( this->board->board[i][j].piece->white == this->white )
				availableMoves.push_back( { i, j } );
			else if( this->board->board[i][j].piece->type == chessPiece::KING ){
				availableMoves.push_back( { i, j } );
				continue;
			}
			break;
		}
	}

	return availableMoves;
}

void king::move( int col, int row ){
	this->chessPiece::move(col, row);
	if( this->first_move )
		this->board->board[ ( col == 2 ? 0 : 7 ) ][this->row].piece->forceMove( ( col == 2 ? this->col + 1 : this->col - 1 ), this->row );
}

void sprite::animationStopped(){}
void chessBoard::animationStopped(){}
