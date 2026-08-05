#include "RandomIA.h"
#include "Move.h"
#include "Board.h"

Move RandomIA::getMove(Board board, Box player)
{
	std::vector<Move> moves = board.getAllMoves();

	Move move = moves[rand() % moves.size()];
	move.box = player;

	return move;
}