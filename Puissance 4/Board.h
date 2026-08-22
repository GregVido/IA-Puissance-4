#pragma once

#include <array>
#include <cstdint>
#include <vector>

#include "Box.h"
#include "Move.h"
#include "GameState.h"

#define WIDTH 7
#define HEIGHT 6

class Board
{
private:
	std::array<Box, WIDTH * HEIGHT> grid;

	std::array<std::uint8_t, WIDTH> heights;
	std::array<std::uint8_t, WIDTH * HEIGHT> historyColumns;
	std::uint8_t moveCount;
	std::uint64_t redBits;
	std::uint64_t yellowBits;

	static bool hasConnectFour(std::uint64_t bits) noexcept;

public:
	Box currentPlayer;

	Board();

	void play(Move move);

	// Version rapide utilisee par l'IA.
	// Retourne false si le coup est impossible.
	bool playColumn(int column, Box player) noexcept;

	// Annule uniquement le dernier pion pose.
	// currentPlayer n'est volontairement pas modifie.
	void undo() noexcept;

	bool isColumnFull(int column) const noexcept;
	std::vector<Move> getAllMoves() const;
	bool isFull() const noexcept;
	Box getWinner() const noexcept;
	GameState getGameState() const;
	void next() noexcept;

	Box getCell(int row, int column) const noexcept
	{
		return grid[row * WIDTH + column];
	}

	int getLandingRow(int column) const noexcept
	{
		if (column < 0 || column >= WIDTH || heights[column] >= HEIGHT)
			return -1;

		return HEIGHT - 1 - static_cast<int>(heights[column]);
	}

	int getMoveCount() const noexcept
	{
		return static_cast<int>(moveCount);
	}

	std::uint64_t getRedBits() const noexcept
	{
		return redBits;
	}

	std::uint64_t getYellowBits() const noexcept
	{
		return yellowBits;
	}

	Board duplicate() const;
	void draw() const;

	int getWidth() const noexcept
	{
		return WIDTH;
	}
};
