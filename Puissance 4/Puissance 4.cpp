#include <iostream>
#include <memory>

#include "Board.h"
#include "RandomIA.h"
#include "MinMaxIA.h"
#include "BestMinMax.h"
#include "IA.h"

#include <chrono>
#include <thread>

IA* getIABYType(int type)
{
	switch (type)
	{
	case 0:
		return new RandomIA();
	case 1:
		return new MinMaxIA();
	case 2:
		return new BestMinMax();
	default:
		return nullptr;
	}
}

int main()
{
	using namespace std::chrono_literals;

	std::cout << "Puissance 4 - IA - By GregVido" << "\n\n";

	int red, yellow;

	std::cout << "Choisis le type d'IA pour le joueur Rouge (0: Random, 1: MinMax, 2: BestMinMax) : ";
	std::cin >> red;
	std::cout << "Choisis le type d'IA pour le joueur Jaune (0: Random, 1: MinMax, 2: BestMinMax) : ";
	std::cin >> yellow;

	const auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(1s);
	const auto end = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double, std::milli> elapsed = end - start;

	Board board;

	while (board.getWinner() == Box::EMPTY && !board.isFull())
	{
		const Box player = board.currentPlayer;

		IA* ia =
			(player == Box::YELLOW)
			? static_cast<IA*>(getIABYType(yellow))
			: static_cast<IA*>(getIABYType(red));

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
