#pragma once

#include <vector>
#include "IA.h"
#include "Board.h"
#include "MoveResult.h"
#include "GameState.h"

class BestMinMax : public IA
{
public:
	Move getMove(Board board, Box player) override;

private:
	static int minmax(const Board& board, int depth, bool maximizingPlayer, int alpha, int beta, Box player);
	static int evalutate(const Board& board, GameState gameState, Box player);
};