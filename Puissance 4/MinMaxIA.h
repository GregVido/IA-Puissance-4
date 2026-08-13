#pragma once

#include <vector>
#include "IA.h"
#include "Board.h"
#include "MoveResult.h"

class MinMaxIA : public IA
{
public:
	MinMaxIA(int depth);
	Move getMove(Board board, Box player) override;

private:
	int _depth;
	static int minmax(const Board& board, int depth, bool maximizingPlayer, int alpha, int beta, Box player);
	static int evalutate(const Board& board, Box player, Box winner);
};