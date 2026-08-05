#include "MinMaxIA.h"
#include "Board.h"
#include "Box.h"
#include "Move.h"

#define DEPTH 5

Move MinMaxIA::getMove(Board board, Box player)
{
	std::vector<Move> moves = board.getAllMoves();

	int bestMove = -1;
	int bestValue = -1000000;

	for (int move = 0; move < moves.size(); move++)
	{
		Move currentMove = moves[move];
		if (currentMove.box == Box::EMPTY)
		{
			Board newBoard = board.duplicate();

			currentMove.box = player;
			newBoard.play(currentMove);

			int moveValue = minmax(newBoard, DEPTH, false, player);

			if (moveValue > bestValue)
			{
				bestValue = moveValue;
				bestMove = currentMove.column;
			}
		}
	}

	Move move;
	move.box = player;
	move.column = bestMove;
	return move;
}

int MinMaxIA::minmax(Board board, int depth, bool maximizingPlayer, Box player)
{
	if (depth == 0)
		return evalutate(board, player);

	if (maximizingPlayer) {
		int maxEval = -1000000;

		for (int i = 0; i < WIDTH; i++) {
			if (!board.isColumnFull(i)) {
				Board newBoard = board.duplicate();
				Move move = Move(newBoard.currentPlayer, i);
				newBoard.play(move);
				newBoard.next();
				int eval = minmax(newBoard, depth - 1, false, player);
				maxEval = std::max(maxEval, eval);
			}
		}

		return maxEval;
	}

	int minEval = 1000000;

	for (int i = 0; i < WIDTH; i++) {
		if (!board.isColumnFull(i)) {
			Board newBoard = board.duplicate();
			Move move = Move(newBoard.currentPlayer, i);
			newBoard.play(move);
			newBoard.next();
			int eval = minmax(newBoard, depth - 1, true, player);
			minEval = std::min(minEval, eval);
		}
	}

	return minEval;
}

int MinMaxIA::evalutate(Board board, Box player)
{
	Box winner = board.getWinner();

	if (winner == player)
		return 1000;

	else if (winner != Box::EMPTY)
		return -1000;

	return 0;
}