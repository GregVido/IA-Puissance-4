#pragma once

#include <vector>
#include "IA.h"

class RandomIA : public IA
{
public:
	Move getMove(Board board, Box player) override;
};

