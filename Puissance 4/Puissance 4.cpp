#include <iostream>
#include <memory>

#include "Board.h"
#include "RandomIA.h"
#include "MinMaxIA.h"
#include "BestMinMax.h"
#include "IA.h"

#include <chrono>
#include <thread>

#include <limits>

IA* getIABYType(int type, int depth)
{
	switch (type)
	{
	case 0:
		return new RandomIA();
	case 1:
		return new MinMaxIA(depth);
	case 2:
		return new BestMinMax(depth);
	default:
		return nullptr;
	}
}

int main()
{
	using namespace std::chrono_literals;

	std::cout << "Puissance 4 - IA - By GregVido" << "\n\n";

	int red = 0;
	int yellow = 0;
	int redDepth = 1;
	int yellowDepth = 1;

	std::cout << "Choisis le type d'IA pour le joueur Rouge (0: Facile, 1: Forte, 2: Imbattable) : ";
	std::cin >> red;

	if (red == 1 || red == 2)
	{
		std::cout << "Choisis la profondeur de recherche pour le joueur Rouge (1-10) : ";
		std::cin >> redDepth;
	}

	std::cout << "Choisis le type d'IA pour le joueur Jaune (0: Facile, 1: Forte, 2: Imbattable) : ";
	std::cin >> yellow;

	if (yellow == 1 || yellow == 2)
	{
		std::cout << "Choisis la profondeur de recherche pour le joueur Jaune (1-10) : ";
		std::cin >> yellowDepth;
	}

	const auto start = std::chrono::high_resolution_clock::now();
	std::this_thread::sleep_for(1s);
	const auto end = std::chrono::high_resolution_clock::now();
	const std::chrono::duration<double, std::milli> elapsed = end - start;

	Board board;

	IA* redIA = getIABYType(red, redDepth);
	IA* yellowIA = getIABYType(yellow, yellowDepth);

	while (board.getWinner() == Box::EMPTY && !board.isFull())
	{
		const Box player = board.currentPlayer;

		IA* ia = player == Box::YELLOW ? yellowIA : redIA;

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

	delete redIA;
	delete yellowIA;

	const Box winner = board.getWinner();

	std::cout
		<< "Gagnant : "
		<< (winner == Box::RED
			? "Rouge"
			: winner == Box::YELLOW
			? "Jaune"
			: "Personne")
		<< '\n';

	std::cout << "Appuie sur Entree pour quitter...";

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin.get();

	return 0;
}
