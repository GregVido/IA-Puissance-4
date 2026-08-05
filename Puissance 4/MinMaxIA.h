#pragma once

#include <vector>
#include "IA.h"
#include "Board.h"

class MinMaxIA : public IA
{
public:
	Move getMove(Board board, Box player) override;
	int minmax(Board board, int depth, bool maximizingPlayer, Box player);
	int evalutate(Board board, Box player);
};