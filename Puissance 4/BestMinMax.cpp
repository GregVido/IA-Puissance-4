#include "BestMinMax.h"
#include "Board.h"
#include "Box.h"
#include "Move.h"

#include <algorithm>
#include <future>
#include <limits>
#include <random>
#include <vector>

#define DEPTH 10

Move BestMinMax::getMove(Board board, Box player)
{

	std::vector<Move> moves = board.getAllMoves();

	if (moves.empty())
		return Move(Box::EMPTY, -1);

	std::vector<std::future<MoveResult>> futures;
	futures.reserve(moves.size());

	for (Move currentMove : moves)
	{
		currentMove.box = player;

		futures.push_back(
			std::async(
				std::launch::async,
				[localBoard = board, currentMove, player]() mutable -> MoveResult
				{
					localBoard.play(currentMove);
					localBoard.next();

					const int moveValue = BestMinMax::minmax(
						localBoard,
						DEPTH - 1,
						false,
						-1000000,
						1000000,
						player
					);

					return {
						currentMove,
						moveValue
					};
				}
			)
		);
	}

	int bestValue = std::numeric_limits<int>::min();
	std::vector<Move> bestMoves;

	for (std::future<MoveResult>& future : futures)
	{
		MoveResult result = future.get();

		if (result.value > bestValue)
		{
			bestValue = result.value;
			bestMoves.clear();
			bestMoves.push_back(result.move);
		}
		else if (result.value == bestValue)
		{
			bestMoves.push_back(result.move);
		}
	}

	static thread_local std::mt19937 generator(
		std::random_device{}()
	);

	std::uniform_int_distribution<std::size_t> distribution(
		0,
		bestMoves.size() - 1
	);

	return bestMoves[distribution(generator)];
}

int BestMinMax::minmax(const Board& board, int depth, bool maximizingPlayer, int alpha, int beta, Box player)
{
	Box winner = board.getWinner();

	if (depth == 0 || winner != Box::EMPTY || board.isFull())
		return evalutate(board, player, winner);

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

int BestMinMax::evalutate(const Board& board, Box player, Box winner)
{
	if (winner == player)
		return 10000;

	else if (winner != Box::EMPTY)
		return -10000;

	return 0;
}