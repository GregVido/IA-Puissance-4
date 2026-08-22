#include "User.h"

Move User::getMove(Board board, Box player)
{
	std::vector<Move> moves = board.getAllMoves();

	Move move = moves[rand() % moves.size()];
	move.box = player;

	return move;
}