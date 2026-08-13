#pragma once

#include <cstdint>
#include <unordered_map>

#include "IA.h"
#include "Board.h"

class BestMinMax : public IA
{
public:
	BestMinMax(int depth);
	Move getMove(Board board, Box player) override;

private:
	int _depth;

	enum class Bound : std::uint8_t
	{
		Exact,
		Lower,
		Upper
	};

	struct TTKey
	{
		std::uint64_t red;
		std::uint64_t yellow;
		bool redToMove;

		bool operator==(const TTKey& other) const noexcept
		{
			return red == other.red &&
				   yellow == other.yellow &&
				   redToMove == other.redToMove;
		}
	};

	struct TTKeyHash
	{
		std::size_t operator()(const TTKey& key) const noexcept;
	};

	struct TTEntry
	{
		int value;
		int depth;
		int bestColumn;
		Bound bound;
	};

	std::unordered_map<TTKey, TTEntry, TTKeyHash> transpositionTable;

	int minmax(
		Board& board,
		int depth,
		int alpha,
		int beta,
		Box player
	);

	static int evaluate(const Board& board, Box player) noexcept;
	static TTKey makeKey(const Board& board) noexcept;
};
