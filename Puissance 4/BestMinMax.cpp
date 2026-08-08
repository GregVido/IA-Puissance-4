#include "BestMinMax.h"

#include "Board.h"
#include "Box.h"
#include "Move.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace
{
	constexpr int DEPTH = 11;
	constexpr int WIN_SCORE = 1'000'000;
	constexpr int INF = 2'000'000;

	// L'ordre des coups est capital pour l'efficacite de l'alpha-beta.
	constexpr std::array<int, WIDTH> MOVE_ORDER = { 3, 2, 4, 1, 5, 0, 6 };

	// Les 69 alignements possibles de 4 cases sur un plateau 7x6.
	constexpr auto makeWindows()
	{
		std::array<std::array<int, 4>, 69> windows{};
		std::size_t n = 0;

		// Horizontal : 6 * 4 = 24
		for (int row = 0; row < HEIGHT; ++row)
		{
			for (int col = 0; col <= WIDTH - 4; ++col)
			{
				for (int i = 0; i < 4; ++i)
					windows[n][i] = row * WIDTH + col + i;

				++n;
			}
		}

		// Vertical : 3 * 7 = 21
		for (int row = 0; row <= HEIGHT - 4; ++row)
		{
			for (int col = 0; col < WIDTH; ++col)
			{
				for (int i = 0; i < 4; ++i)
					windows[n][i] = (row + i) * WIDTH + col;

				++n;
			}
		}

		// Diagonale \ : 3 * 4 = 12
		for (int row = 0; row <= HEIGHT - 4; ++row)
		{
			for (int col = 0; col <= WIDTH - 4; ++col)
			{
				for (int i = 0; i < 4; ++i)
					windows[n][i] = (row + i) * WIDTH + col + i;

				++n;
			}
		}

		// Diagonale / : 3 * 4 = 12
		for (int row = 0; row <= HEIGHT - 4; ++row)
		{
			for (int col = 3; col < WIDTH; ++col)
			{
				for (int i = 0; i < 4; ++i)
					windows[n][i] = (row + i) * WIDTH + col - i;

				++n;
			}
		}

		return windows;
	}

	constexpr auto WINDOWS = makeWindows();

	inline std::uint64_t mix64(std::uint64_t x) noexcept
	{
		x ^= x >> 30;
		x *= 0xbf58476d1ce4e5b9ULL;
		x ^= x >> 27;
		x *= 0x94d049bb133111ebULL;
		x ^= x >> 31;
		return x;
	}
}

std::size_t BestMinMax::TTKeyHash::operator()(const TTKey& key) const noexcept
{
	std::uint64_t hash = mix64(key.red);
	hash ^= mix64(key.yellow + 0x9e3779b97f4a7c15ULL);

	if (key.redToMove)
		hash ^= 0xd6e8feb86659fd93ULL;

	return static_cast<std::size_t>(hash);
}

BestMinMax::TTKey BestMinMax::makeKey(const Board& board) noexcept
{
	return {
		board.getRedBits(),
		board.getYellowBits(),
		board.currentPlayer == Box::RED
	};
}

Move BestMinMax::getMove(Board board, Box player)
{
	if (board.isFull())
		return Move(Box::EMPTY, -1);

	// On garde les buckets entre les tours, mais on supprime les anciennes
	// evaluations car les scores sont calcules du point de vue de "player".
	transpositionTable.clear();

	if (transpositionTable.bucket_count() < 200'000)
	{
		transpositionTable.max_load_factor(0.75f);
		transpositionTable.reserve(200'000);
	}

	int bestValue = -INF;
	int bestColumn = -1;
	int alpha = -INF;

	// Centre -> bords.
	for (const int column : MOVE_ORDER)
	{
		if (board.isColumnFull(column))
			continue;

		if (!board.playColumn(column, player))
			continue;

		board.next();

		const int value = minmax(
			board,
			DEPTH - 1,
			alpha,
			INF,
			player
		);

		// Remet exactement le plateau dans son etat initial.
		board.next();
		board.undo();

		if (value > bestValue)
		{
			bestValue = value;
			bestColumn = column;
		}

		alpha = std::max(alpha, bestValue);

		// On ne peut pas faire mieux qu'une victoire immediate.
		if (bestValue >= WIN_SCORE - (board.getMoveCount() + 1))
			break;
	}

	if (bestColumn == -1)
		return Move(Box::EMPTY, -1);

	return Move(player, bestColumn);
}

int BestMinMax::minmax(
	Board& board,
	int depth,
	int alpha,
	int beta,
	Box player
)
{
	const Box winner = board.getWinner();

	if (winner == player)
		return WIN_SCORE - board.getMoveCount();

	if (winner != Box::EMPTY)
		return -WIN_SCORE + board.getMoveCount();

	if (board.isFull())
		return 0;

	if (depth == 0)
		return evaluate(board, player);

	const int originalAlpha = alpha;
	const int originalBeta = beta;

	const TTKey key = makeKey(board);

	int preferredColumn = -1;

	if (const auto it = transpositionTable.find(key);
		it != transpositionTable.end())
	{
		const TTEntry& entry = it->second;
		preferredColumn = entry.bestColumn;

		if (entry.depth >= depth)
		{
			if (entry.bound == Bound::Exact)
				return entry.value;

			if (entry.bound == Bound::Lower)
				alpha = std::max(alpha, entry.value);
			else
				beta = std::min(beta, entry.value);

			if (alpha >= beta)
				return entry.value;
		}
	}

	const bool maximizing = board.currentPlayer == player;
	const Box mover = board.currentPlayer;

	int bestValue = maximizing ? -INF : INF;
	int bestColumn = -1;

	auto searchColumn = [&](int column) -> bool
	{
		if (column < 0 || column >= WIDTH || board.isColumnFull(column))
			return false;

		if (!board.playColumn(column, mover))
			return false;

		board.next();

		const int value = minmax(
			board,
			depth - 1,
			alpha,
			beta,
			player
		);

		board.next();
		board.undo();

		if (maximizing)
		{
			if (value > bestValue)
			{
				bestValue = value;
				bestColumn = column;
			}

			alpha = std::max(alpha, bestValue);
		}
		else
		{
			if (value < bestValue)
			{
				bestValue = value;
				bestColumn = column;
			}

			beta = std::min(beta, bestValue);
		}

		return alpha >= beta;
	};

	// Le meilleur coup memorise par la TT passe en premier.
	if (preferredColumn != -1)
	{
		if (searchColumn(preferredColumn))
			goto store_result;
	}

	for (const int column : MOVE_ORDER)
	{
		if (column == preferredColumn)
			continue;

		if (searchColumn(column))
			break;
	}

store_result:
	Bound bound = Bound::Exact;

	if (bestValue <= originalAlpha)
		bound = Bound::Upper;
	else if (bestValue >= originalBeta)
		bound = Bound::Lower;

	const TTEntry newEntry{
		bestValue,
		depth,
		bestColumn,
		bound
	};

	const auto existing = transpositionTable.find(key);

	if (existing == transpositionTable.end())
	{
		transpositionTable.emplace(key, newEntry);
	}
	else if (depth >= existing->second.depth)
	{
		existing->second = newEntry;
	}

	return bestValue;
}

int BestMinMax::evaluate(const Board& board, Box player) noexcept
{
	const Box opponent =
		(player == Box::RED)
		? Box::YELLOW
		: Box::RED;

	int score = 0;
	int playerThreats = 0;
	int opponentThreats = 0;

	// Le centre participe a plus d'alignements que les bords.
	constexpr int centerColumn = WIDTH / 2;

	for (int row = 0; row < HEIGHT; ++row)
	{
		const Box cell = board.getCell(row, centerColumn);

		if (cell == player)
			score += 30;
		else if (cell == opponent)
			score -= 30;
	}

	// Toutes les fenetres sont pre-calculees a la compilation.
	for (const auto& window : WINDOWS)
	{
		int playerCount = 0;
		int opponentCount = 0;
		int emptyCount = 0;
		int emptyIndex = -1;

		for (const int index : window)
		{
			const int row = index / WIDTH;
			const int col = index % WIDTH;
			const Box cell = board.getCell(row, col);

			if (cell == player)
			{
				++playerCount;
			}
			else if (cell == opponent)
			{
				++opponentCount;
			}
			else
			{
				++emptyCount;
				emptyIndex = index;
			}
		}

		// Fenetre deja bloquee par les deux couleurs.
		if (playerCount != 0 && opponentCount != 0)
			continue;

		bool emptyIsPlayable = false;

		if (emptyCount == 1)
		{
			const int emptyRow = emptyIndex / WIDTH;
			const int emptyCol = emptyIndex % WIDTH;

			emptyIsPlayable =
				board.getLandingRow(emptyCol) == emptyRow;
		}

		if (playerCount == 4)
		{
			score += 100'000;
		}
		else if (playerCount == 3 && emptyCount == 1)
		{
			if (emptyIsPlayable)
			{
				score += 5'000;
				++playerThreats;
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

		if (opponentCount == 4)
		{
			score -= 100'000;
		}
		else if (opponentCount == 3 && emptyCount == 1)
		{
			if (emptyIsPlayable)
			{
				score -= 7'000;
				++opponentThreats;
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
	}

	if (playerThreats >= 2)
		score += 20'000;

	if (opponentThreats >= 2)
		score -= 25'000;

	return score;
}
