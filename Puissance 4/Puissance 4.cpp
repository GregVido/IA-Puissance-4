#include <iostream>

#include "Board.h"
#include "RandomIA.h"
#include "MinMaxIA.h"
#include "IA.h"

int main()
{
	std::cout << "Hello World!\n";

	Board board = Board();
	board.draw();

	Box winner = Box::EMPTY;

	while ((winner = board.getWinner()) == Box::EMPTY && !board.isFull()) {
		std::unique_ptr<IA> ia;

		if (board.currentPlayer == Box::YELLOW)
			ia = std::make_unique<MinMaxIA>();

		else
			ia = std::make_unique<MinMaxIA>();

		Move move = ia->getMove(board, board.currentPlayer);

		board.play(move);
		board.draw();

		std::cout << "Joueur : " << (winner == Box::RED ? "Rouge" : "Jaune") << " joue sur la colonne " << move.column << std::endl;

		board.next();
	}

	std::cout << "Gagnant : " << (winner == Box::RED ? "Rouge" : winner == Box::YELLOW ? "Jaune" : "Personne") << std::endl;

	return 0;
}