#include <iostream>
#include <memory>

#include "Board.h"
#include "RandomIA.h"
#include "MinMaxIA.h"
#include "BestMinMax.h"
#include "IA.h"

int main()
{
	Board board;

	// Evite de recreer/detruire les IA a chaque tour.
	BestMinMax bestMinMax;
	MinMaxIA minMax;

	while (board.getWinner() == Box::EMPTY && !board.isFull())
	{
		const Box player = board.currentPlayer;

		IA* ia =
			(player == Box::YELLOW)
			? static_cast<IA*>(&bestMinMax)
			: static_cast<IA*>(&minMax);

		const Move move = ia->getMove(board, player);

		board.play(move);
		board.draw();

		std::cout
			<< "Joueur : "
			<< (player == Box::RED ? "Rouge" : "Jaune")
			<< " joue sur la colonne "
			<< move.column
			<< '\n';

		board.next();
	}

	const Box winner = board.getWinner();

	std::cout
		<< "Gagnant : "
		<< (winner == Box::RED
			? "Rouge"
			: winner == Box::YELLOW
				? "Jaune"
				: "Personne")
		<< '\n';

	return 0;
}
