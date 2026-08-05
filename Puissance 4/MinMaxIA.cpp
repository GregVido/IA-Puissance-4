#include "MinMaxIA.h"
#include "Board.h"
#include "Box.h"
#include "Move.h"

#define DEPTH 5

Move MinMaxIA::getMove(Board board, Box player)
{
	std::vector<Move> moves = board.getAllMoves();

	std::vector<Move> bestMove;
	int bestValue = -1000000;

	for (int move = 0; move < moves.size(); move++)
	{
		Move currentMove = moves[move];
		if (currentMove.box == Box::EMPTY)
		{
			Board newBoard = board.duplicate();

			currentMove.box = player;
			newBoard.play(currentMove);

			int moveValue = minmax(newBoard, DEPTH, false, -1000000, 1000000, player);

			if (moveValue > bestValue)
			{
				bestValue = moveValue;
				bestMove.push_back(currentMove);
			}
		}
	}

	return bestMove[rand() % bestMove.size()];
}

int MinMaxIA::minmax(Board board, int depth, bool maximizingPlayer, int alpha, int beta, Box player)
{
	if (depth == 0 || board.getWinner() != Box::EMPTY || board.isFull())
		return evalutate(board, player);

	if (maximizingPlayer) {
		int maxEval = -1000000;

		for (int i = 0; i < WIDTH; i++) {
			if (!board.isColumnFull(i)) {
				Board newBoard = board.duplicate();
				Move move = Move(newBoard.currentPlayer, i);
				newBoard.play(move);
				newBoard.next();
				int eval = minmax(newBoard, depth - 1, false, alpha, beta, player);
				maxEval = std::max(maxEval, eval);
				alpha = std::max(alpha, eval);
				if (beta <= alpha)
					break;
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
			int eval = minmax(newBoard, depth - 1, true, alpha, beta, player);
			minEval = std::min(minEval, eval);
			beta = std::min(beta, eval);
			if (beta <= alpha)
				break;
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