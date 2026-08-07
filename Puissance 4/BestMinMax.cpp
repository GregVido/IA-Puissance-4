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
	const Box winner = board.getWinner();

	// Victoire IA
	if (winner == player)
		return 1000000 + depth;

	// Victoire adversaire
	if (winner != Box::EMPTY)
		return -1000000 - depth;

	// Match nul
	if (board.isFull())
		return 0;

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

int BestMinMax::evalutate(const Board& board, Box player)
{
	const Box opponent =
		player == Box::RED
		? Box::YELLOW
		: Box::RED;

	int score = 0;

	int playerThreats = 0;
	int opponentThreats = 0;

	// --------------------------------------------------
	// 1. Contrôle du centre
	// --------------------------------------------------

	const int centerColumn = WIDTH / 2;

	for (int row = 0; row < HEIGHT; row++)
	{
		const Box cell = board.getCell(row, centerColumn);

		if (cell == player)
			score += 30;

		else if (cell == opponent)
			score -= 30;
	}

	// --------------------------------------------------
	// Fonction qui vérifie si une case vide est jouable
	// immédiatement (gravité du Puissance 4)
	// --------------------------------------------------

	auto isPlayable = [&](int row, int column)
		{
			if (board.getCell(row, column) != Box::EMPTY)
				return false;

			// Dernière ligne
			if (row == HEIGHT - 1)
				return true;

			// Il faut un pion juste en dessous
			return board.getCell(row + 1, column) != Box::EMPTY;
		};

	// --------------------------------------------------
	// Évaluation d'une fenêtre de 4 cases
	// --------------------------------------------------

	auto evaluateWindow =
		[&](int startRow, int startCol, int rowDir, int colDir)
		{
			int playerCount = 0;
			int opponentCount = 0;
			int emptyCount = 0;

			int emptyRow = -1;
			int emptyCol = -1;

			for (int i = 0; i < 4; i++)
			{
				const int row = startRow + rowDir * i;
				const int col = startCol + colDir * i;

				const Box cell = board.getCell(row, col);

				if (cell == player)
				{
					playerCount++;
				}
				else if (cell == opponent)
				{
					opponentCount++;
				}
				else
				{
					emptyCount++;
					emptyRow = row;
					emptyCol = col;
				}
			}

			// Fenêtre bloquée :
			// les deux joueurs sont présents
			if (playerCount > 0 && opponentCount > 0)
				return;

			// ----------------------------------------------
			// IA
			// ----------------------------------------------

			if (playerCount == 4)
			{
				score += 100000;
			}
			else if (playerCount == 3 && emptyCount == 1)
			{
				if (isPlayable(emptyRow, emptyCol))
				{
					score += 5000;
					playerThreats++;
				}
				else
				{
					score += 500;
				}
			}
			else if (playerCount == 2 && emptyCount == 2)
			{
				score += 100;
			}
			else if (playerCount == 1 && emptyCount == 3)
			{
				score += 5;
			}

			// ----------------------------------------------
			// Adversaire
			// ----------------------------------------------

			if (opponentCount == 4)
			{
				score -= 100000;
			}
			else if (opponentCount == 3 && emptyCount == 1)
			{
				if (isPlayable(emptyRow, emptyCol))
				{
					// On pénalise légèrement plus que
					// notre propre attaque :
					// bloquer une victoire est prioritaire.
					score -= 7000;
					opponentThreats++;
				}
				else
				{
					score -= 700;
				}
			}
			else if (opponentCount == 2 && emptyCount == 2)
			{
				score -= 150;
			}
			else if (opponentCount == 1 && emptyCount == 3)
			{
				score -= 5;
			}
		};

	// ==================================================
	// HORIZONTAL
	// ==================================================

	for (int row = 0; row < HEIGHT; row++)
	{
		for (int col = 0; col <= WIDTH - 4; col++)
		{
			evaluateWindow(row, col, 0, 1);
		}
	}

	// ==================================================
	// VERTICAL
	// ==================================================

	for (int row = 0; row <= HEIGHT - 4; row++)
	{
		for (int col = 0; col < WIDTH; col++)
		{
			evaluateWindow(row, col, 1, 0);
		}
	}

	// ==================================================
	// DIAGONALE \
	// ==================================================

	for (int row = 0; row <= HEIGHT - 4; row++)
	{
		for (int col = 0; col <= WIDTH - 4; col++)
		{
			evaluateWindow(row, col, 1, 1);
		}
	}

	// ==================================================
	// DIAGONALE /
	// ==================================================

	for (int row = 0; row <= HEIGHT - 4; row++)
	{
		for (int col = 3; col < WIDTH; col++)
		{
			evaluateWindow(row, col, 1, -1);
		}
	}

	// --------------------------------------------------
	// Double menace
	//
	// Deux façons différentes de gagner au prochain
	// tour sont extrêmement fortes.
	// --------------------------------------------------

	if (playerThreats >= 2)
		score += 20000;

	if (opponentThreats >= 2)
		score -= 25000;

	return score;
}