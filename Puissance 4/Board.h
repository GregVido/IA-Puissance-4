#pragma once

#include <vector>
#include "Box.h"
#include "Move.h"
#include <array>
#include "GameState.h"

#define WIDTH 7
#define HEIGHT 6

class Board
{
private:
	std::array<Box, WIDTH* HEIGHT> grid;

public:
	Box currentPlayer;

	Board();
	void play(Move move);
	bool isColumnFull(int column) const;
	std::vector<Move> getAllMoves() const;
	bool isFull() const;
	Box getWinner() const;
	GameState getGameState() const;
	void next();
	Board duplicate() const;
	void draw() const;
};

	