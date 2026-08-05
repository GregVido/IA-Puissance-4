#include "Board.h"
#include "Move.h"

#include <iostream>

Board::Board()
	: grid(HEIGHT, std::vector<Box>(WIDTH, Box::EMPTY)), currentPlayer(Box::RED)
{
}

void Board::play(Move move)
{
	int column = move.column;
	Box player = move.box;

	if (!isColumnFull(column)) {
		for (int i = HEIGHT - 1; i >= 0; i--)
		{
			if (grid[i][column] == Box::EMPTY)
			{
				grid[i][column] = player;
				break;
			}
		}
	}
}

bool Board::isColumnFull(int column)
{
	int nbBox = 0;

	for (int i = 0; i < HEIGHT; i++)
	{
		if (grid[i][column] != Box::EMPTY)
			nbBox++;
	}
	return nbBox == HEIGHT;
}

std::vector<Move> Board::getAllMoves()
{
	std::vector<Move> moves;
	for (int j = 0; j < WIDTH; j++)
	{
		if (!isColumnFull(j))
		{
			Move move = Move(Box::EMPTY, j);
			moves.push_back(move);
		}
	}
	return moves;
}

bool Board::isFull()
{
	std::vector<Box> line = grid[0];
	for (int col = 0; col < line.size(); col++)
	{
		if (line[col] == Box::EMPTY)
			return false;
	}
	return true;
}

Box Board::getWinner()
{
	const int directions[4][2] = {
		{0, 1},
		{1, 0},
		{1, 1},
		{1, -1}
	};

	for (int row = 0; row < HEIGHT; row++)
	{
		for (int col = 0; col < WIDTH; col++)
		{
			const Box player = grid[row][col];

			if (player == Box::EMPTY)
				continue;

			for (const auto& direction : directions)
			{
				const int rowDirection = direction[0];
				const int colDirection = direction[1];

				bool hasWon = true;

				for (int i = 1; i < 4; i++)
				{
					const int nextRow = row + rowDirection * i;
					const int nextCol = col + colDirection * i;

					if (nextRow < 0 || nextRow >= HEIGHT ||
						nextCol < 0 || nextCol >= WIDTH)
					{
						hasWon = false;
						break;
					}

					if (grid[nextRow][nextCol] != player)
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

void Board::next()
{
	if (currentPlayer == Box::RED)
		currentPlayer = Box::YELLOW;
	else
		currentPlayer = Box::RED;
}

Board Board::duplicate()
{
	Board newBoard;
	newBoard.grid = grid;
	newBoard.currentPlayer = currentPlayer;
	return newBoard;
}

void Board::draw() {
	for (int row = 0; row < grid.size(); row++) {
		for (int col = 0; col < grid[row].size() * 2; col++) {
			std::cout << "- ";
		}

		std::cout << "-";
		std::cout << std::endl;
		std::cout << "| ";

		for (int col = 0; col < grid[row].size(); col++) {
			switch (grid[row][col]) {
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

	for (int col = 0; col < grid[0].size() * 2; col++)
		std::cout << "- ";

	std::cout << "-";
	std::cout << std::endl;
}