#pragma once

#include <vector>
#include "Move.h"
#include "Board.h"

class IA
{
public:
	IA() = default;
	virtual Move getMove(Board board, Box player) = 0;
};
