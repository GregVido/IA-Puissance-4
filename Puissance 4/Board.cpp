#include "Board.h"
#include "Move.h"

#include <iostream>

Board::Board()
	: currentPlayer(Box::RED)
{
	grid.fill(Box::EMPTY);
}

void Board::play(Move move)
{
	const int column = move.column;
	const Box player = move.box;

	if (column < 0 || column >= WIDTH)
		return;

	if (isColumnFull(column))
		return;

	for (int row = HEIGHT - 1; row >= 0; row--)
	{
		const int index = row * WIDTH + column;

		if (grid[index] == Box::EMPTY)
		{
			grid[index] = player;
			break;
		}
	}
}

bool Board::isColumnFull(int column) const
{
	if (column < 0 || column >= WIDTH)
		return true;

	// La colonne est pleine si sa case la plus haute est occupée
	return grid[column] != Box::EMPTY;
}

std::vector<Move> Board::getAllMoves() const
{
	std::vector<Move> moves;
	moves.reserve(WIDTH);

	for (int column = 0; column < WIDTH; column++)
	{
		if (!isColumnFull(column))
		{
			moves.emplace_back(Box::EMPTY, column);
		}
	}

	return moves;
}

bool Board::isFull() const
{
	// Seule la première ligne doit être vérifiée
	for (int column = 0; column < WIDTH; column++)
	{
		if (grid[column] == Box::EMPTY)
			return false;
	}

	return true;
}

Box Board::getWinner() const
{
	constexpr int directions[4][2] = {
		{0, 1},  // Horizontal
		{1, 0},  // Vertical
		{1, 1},  // Diagonale descendante droite
		{1, -1}  // Diagonale descendante gauche
	};

	for (int row = 0; row < HEIGHT; row++)
	{
		for (int column = 0; column < WIDTH; column++)
		{
			const Box player = grid[row * WIDTH + column];

			if (player == Box::EMPTY)
				continue;

			for (const auto& direction : directions)
			{
				const int rowDirection = direction[0];
				const int columnDirection = direction[1];

				bool hasWon = true;

				for (int offset = 1; offset < 4; offset++)
				{
					const int nextRow =
						row + rowDirection * offset;

					const int nextColumn =
						column + columnDirection * offset;

					if (nextRow < 0 || nextRow >= HEIGHT ||
						nextColumn < 0 || nextColumn >= WIDTH)
					{
						hasWon = false;
						break;
					}

					const int nextIndex =
						nextRow * WIDTH + nextColumn;

					if (grid[nextIndex] != player)
					{
						hasWon = false;
						break;
					}
				}

				if (hasWon)
					return player;
			}
		}
	}

	return Box::EMPTY;
}

GameState Board::getGameState() const
{
	GameState state;
	state.winner = Box::EMPTY;
	state.nbWin = 0;
	state.nb3Token = 0;
	state.nb2Token = 0;
	state.currentPlayer = currentPlayer;

	constexpr int directions[4][2] = {
		{0, 1},  // Horizontal
		{1, 0},  // Vertical
		{1, 1},  // Diagonale descendante droite
		{1, -1}  // Diagonale descendante gauche
	};

	for (int row = 0; row < HEIGHT; row++)
	{
		for (int column = 0; column < WIDTH; column++)
		{
			const Box player = grid[row * WIDTH + column];

			if (player == Box::EMPTY)
				continue;

			for (const auto& direction : directions)
			{
				const int rowDirection = direction[0];
				const int columnDirection = direction[1];

				bool hasWon = true;

				int count = 1;
				for (int offset = 1; offset < 4; offset++)
				{
					const int nextRow = row + rowDirection * offset;
					const int nextColumn =						column + columnDirection * offset;

					if (nextRow < 0 || nextRow >= HEIGHT || nextColumn < 0 || nextColumn >= WIDTH) {
						hasWon = false;
						break;
					}
					const int nextIndex = nextRow * WIDTH + nextColumn;
					if (grid[nextIndex] == player)
					{
						count++;
					}
					else
					{
						hasWon = false;
						break;
					}
				}
				if (count == 4)
					state.nbWin++;
				
				else if (count == 3)
					state.nb3Token++;
				
				else if (count == 2)
					state.nb2Token++;

				if (hasWon)
					state.winner = player;
			}
		}
	}
	return state;
}

void Board::next()
{
	if (currentPlayer == Box::RED)
		currentPlayer = Box::YELLOW;
	else
		currentPlayer = Box::RED;
}

Board Board::duplicate() const
{
	return *this;
}

void Board::draw() const
{
	for (int row = 0; row < HEIGHT; row++)
	{
		for (int column = 0; column < WIDTH; column++)
			std::cout << "----";

		std::cout << "-" << std::endl;
		std::cout << "| ";

		for (int column = 0; column < WIDTH; column++)
		{
			const Box box = grid[row * WIDTH + column];

			switch (box)
			{
			case Box::EMPTY:
				std::cout << ".";
				break;

			case Box::RED:
				std::cout << "R";
				break;

			case Box::YELLOW:
				std::cout << "Y";
				break;
			}

			std::cout << " | ";
		}

		std::cout << std::endl;
	}

	for (int column = 0; column < WIDTH; column++)
		std::cout << "----";

	std::cout << "-" << std::endl;

	std::cout << "  ";

	for (int column = 0; column < WIDTH; column++)
		std::cout << column << "   ";

	std::cout << std::endl;
}