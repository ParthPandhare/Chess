#ifndef CONSTANTS_H
#define CONSTANTS_H


namespace constants
{
	/* all the dimensions are in pixels */
	const int SCREEN_WIDTH = 512;
	const int SCREEN_HEIGHT = 512;
	const int BOARD_DIMENSION = 512;
	const int SQUARE_DIMENSION = 64;
}

/*
defines for the piece_images_ array to make it easier to access as follows:
FIRST_SECOND --> COLOR_PIECE
*/
#define EMPTY -1

#define B_ROOK 0
#define B_KNIGHT 1
#define B_BISHOP 2
#define B_QUEEN 3
#define B_KING 4
#define B_PAWN 5

#define W_ROOK 6 
#define W_KNIGHT 7
#define W_BISHOP 8
#define W_QUEEN 9
#define W_KING 10
#define W_PAWN 11

/* defines the boolean for team */
#define WHITE -1
#define BLACK 1

/* defines gamemodes */
#define TWO_PLAYER -2
#define ONE_PLAYER_WHITE -3
#define ONE_PLAYER_BLACK -4

#endif // !CONSTANTS_H
