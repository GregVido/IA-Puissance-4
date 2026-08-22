#include "User.h"
#include <iostream>

Move User::getMove(Board board, Box player)
{

	Move move;
	move.box = player;
	move.column = -1;

	std::cout << "Sur quel colonne voulez vous placer votre jeton ? (1 - " << board.getWidth() << ") : ";
	std::cin >> move.column;
	move.column--;

	return move;
}