#include "Board.h"
#include "Move.h"

#include <iostream>

namespace
{
	constexpr std::array<int, WIDTH> MOVE_ORDER = { 3, 2, 4, 1, 5, 0, 6 };
}

Board::Board()
	: moveCount(0),
	  redBits(0),
	  yellowBits(0),
	  currentPlayer(Box::RED)
{
	grid.fill(Box::EMPTY);
	heights.fill(0);
	historyColumns.fill(0);
}

void Board::play(Move move)
{
	playColumn(move.column, move.box);
}

bool Board::playColumn(int column, Box player) noexcept
{
	if (column < 0 || column >= WIDTH)
		return false;

	if (player == Box::EMPTY || heights[column] >= HEIGHT)
		return false;

	const int row = HEIGHT - 1 - static_cast<int>(heights[column]);
	const int index = row * WIDTH + column;
	// Bitboard Puissance 4 classique : 7 bits par colonne
	// (6 cases + 1 bit sentinelle). La case du bas vaut 0.
	const int bitIndex =
		column * (HEIGHT + 1) + (HEIGHT - 1 - row);
	const std::uint64_t bit = std::uint64_t{ 1 } << bitIndex;

	grid[index] = player;
	historyColumns[moveCount] = static_cast<std::uint8_t>(column);

	++heights[column];
	++moveCount;

	if (player == Box::RED)
		redBits |= bit;
	else
		yellowBits |= bit;

	return true;
}

void Board::undo() noexcept
{
	if (moveCount == 0)
		return;

	const int column = static_cast<int>(historyColumns[moveCount - 1]);

	// La case du dernier pion est la case occupee la plus haute
	// de sa colonne.
	const int row = HEIGHT - static_cast<int>(heights[column]);
	const int index = row * WIDTH + column;
	const Box player = grid[index];
	const int bitIndex =
		column * (HEIGHT + 1) + (HEIGHT - 1 - row);
	const std::uint64_t bit = std::uint64_t{ 1 } << bitIndex;

	if (player == Box::RED)
		redBits &= ~bit;
	else if (player == Box::YELLOW)
		yellowBits &= ~bit;

	grid[index] = Box::EMPTY;

	--heights[column];
	--moveCount;
}

bool Board::isColumnFull(int column) const noexcept
{
	return column < 0 ||
		   column >= WIDTH ||
		   heights[column] >= HEIGHT;
}

std::vector<Move> Board::getAllMoves() const
{
	std::vector<Move> moves;
	moves.reserve(WIDTH);

	// Centre d'abord : meilleur ordre pour alpha-beta.
	for (const int column : MOVE_ORDER)
	{
		if (!isColumnFull(column))
			moves.emplace_back(Box::EMPTY, column);
	}

	return moves;
}

bool Board::isFull() const noexcept
{
	return moveCount == WIDTH * HEIGHT;
}

bool Board::hasConnectFour(std::uint64_t bits) noexcept
{
	// Representation 7 bits par colonne :
	// vertical = 1, horizontal = 7, diagonales = 6 et 8.
	constexpr int shifts[4] = {
		1,
		HEIGHT + 1,
		HEIGHT,
		HEIGHT + 2
	};

	for (const int shift : shifts)
	{
		const std::uint64_t pairs = bits & (bits >> shift);

		if ((pairs & (pairs >> (2 * shift))) != 0)
			return true;
	}

	return false;
}

Box Board::getWinner() const noexcept
{
	// Impossible d'avoir un gagnant avant le 7e coup.
	if (moveCount < 7)
		return Box::EMPTY;

	const int column = static_cast<int>(historyColumns[moveCount - 1]);
	const int row = HEIGHT - static_cast<int>(heights[column]);
	const Box player = grid[row * WIDTH + column];

	if (player == Box::RED)
		return hasConnectFour(redBits) ? Box::RED : Box::EMPTY;

	if (player == Box::YELLOW)
		return hasConnectFour(yellowBits) ? Box::YELLOW : Box::EMPTY;

	return Box::EMPTY;
}

GameState Board::getGameState() const
{
	GameState state;
	state.winner = getWinner();
	state.nbWin = 0;
	state.nb3Token = 0;
	state.nb2Token = 0;
	state.currentPlayer = currentPlayer;

	// Cette methode n'est pas utilisee dans le hot path du MinMax.
	// On conserve un calcul complet pour rester compatible avec ton API.
	constexpr int directions[4][2] = {
		{ 0, 1 },
		{ 1, 0 },
		{ 1, 1 },
		{ 1, -1 }
	};

	for (int row = 0; row < HEIGHT; ++row)
	{
		for (int column = 0; column < WIDTH; ++column)
		{
			const Box player = grid[row * WIDTH + column];

			if (player == Box::EMPTY)
				continue;

			for (const auto& direction : directions)
			{
				int count = 1;

				for (int offset = 1; offset < 4; ++offset)
				{
					const int nextRow = row + direction[0] * offset;
					const int nextColumn = column + direction[1] * offset;

					if (nextRow < 0 || nextRow >= HEIGHT ||
						nextColumn < 0 || nextColumn >= WIDTH)
						break;

					if (grid[nextRow * WIDTH + nextColumn] != player)
						break;

					++count;
				}

				if (count == 4)
					++state.nbWin;
				else if (count == 3)
					++state.nb3Token;
				else if (count == 2)
					++state.nb2Token;
			}
		}
	}

	return state;
}

void Board::next() noexcept
{
	currentPlayer =
		(currentPlayer == Box::RED)
		? Box::YELLOW
		: Box::RED;
}

Board Board::duplicate() const
{
	return *this;
}

void Board::draw() const
{
	for (int row = 0; row < HEIGHT; ++row)
	{
		for (int column = 0; column < WIDTH; ++column)
			std::cout << "----";

		std::cout << "-\n| ";

		for (int column = 0; column < WIDTH; ++column)
		{
			const Box box = grid[row * WIDTH + column];

			switch (box)
			{
			case Box::EMPTY:
				std::cout << ".";
				break;

			case Box::RED:
				std::cout << "\033[31mR\033[0m";
				break;

			case Box::YELLOW:
				std::cout << "Y";
				break;
			}

			std::cout << " | ";
		}

		std::cout << '\n';
	}

	for (int column = 0; column < WIDTH; ++column)
		std::cout << "----";

	std::cout << "-\n  ";

	for (int column = 0; column < WIDTH; ++column)
		std::cout << column + 1 << "   ";

	std::cout << '\n';
}
