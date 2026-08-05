#pragma once

#include <vector>
#include "Box.h"
#include "Move.h"

#define WIDTH 7
#define HEIGHT 6

class Board
{
private:
	std::vector<std::vector<Box>> grid;

public:
	Box currentPlayer;

	Board();
	void play(Move move);
	bool isColumnFull(int column);
	std::vector<Move> getAllMoves();
	bool isFull();
	Box getWinner();
	void next();
	Board duplicate();
	void draw();
};

	