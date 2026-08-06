#pragma once

#include <vector>
#include "IA.h"
#include "Board.h"

struct MoveResult
{
	Move move;
	int value;
};

class MinMaxIA : public IA
{
public:
	Move getMove(Board board, Box player) override;

private:
	static int minmax(const Board& board, int depth, bool maximizingPlayer, int alpha, int beta, Box player);
	static int evalutate(const Board& board, Box player, Box winner);
};