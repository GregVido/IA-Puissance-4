#include "User.h"
#include <iostream>

Move User::getMove(Board board, Box player)
{

	Move move;
	move.box = player;
	move.column = -1;

	board.draw();

	while (move.column < 0 || move.column >= board.getWidth() || board.isColumnFull(move.column))
	{
		std::cout << "Sur quel colonne voulez vous placer votre jeton ? (1 - " << board.getWidth() << ") : ";
		std::cin >> move.column;
		move.column--;

		if (move.column < 0 || move.column >= board.getWidth())
			std::cout << "Colonne invalide. ";
		else if (board.isColumnFull(move.column))
			std::cout << "Colonne pleine. ";
	}

	return move;
}