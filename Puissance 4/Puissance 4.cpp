#include <iostream>
#include <memory>

#include <algorithm>
#include <cstdint>
#include <iomanip>
#include "Board.h"
#include "RandomIA.h"
#include "MinMaxIA.h"
#include "BestMinMax.h"
#include "User.h"
#include "IA.h"

#include <chrono>
#include <thread>

#include <limits>

#define NOMINMAX
#include <Windows.h>
#include "resource.h"

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
	case 3:
		return new User();
	default:
		return nullptr;
	}
}

void setIcon() {
	HWND console = GetConsoleWindow();

	HICON icon = LoadIcon(
		GetModuleHandle(nullptr),
		MAKEINTRESOURCE(IDI_ICON1)
	);

	SendMessage(console, WM_SETICON, ICON_BIG, reinterpret_cast<LPARAM>(icon));
	SendMessage(console, WM_SETICON, ICON_SMALL, reinterpret_cast<LPARAM>(icon));
}

int main()
{
	using namespace std::chrono_literals;
	bool loop = true;

	setIcon();

	while (loop) {
		std::cout << "Puissance 4 - IA - By GregVido" << "\n\n";

		int red = 0;
		int yellow = 0;
		int redDepth = 1;
		int yellowDepth = 1;

		std::cout << "Choisis le type d'IA pour le joueur Rouge (0: Facile, 1: Forte, 2: Imbattable, 3: Humain) : ";
		std::cin >> red;

		if (red == 1 || red == 2)
		{
			std::cout << "\nAttention, plus le chiffre est grand, plus le temps de calcul est long !\nChoisis la profondeur de recherche pour le joueur Rouge (1-15) : ";
			std::cin >> redDepth;

			redDepth = std::clamp(redDepth, 1, 15);
		}

		std::cout << "Choisis le type d'IA pour le joueur Jaune (0: Facile, 1: Forte, 2: Imbattable) : ";
		std::cin >> yellow;

		if (yellow == 1 || yellow == 2)
		{
			std::cout << "\nAttention, plus le chiffre est grand, plus le temps de calcul est long !\nChoisis la profondeur de recherche pour le joueur Rouge (1-15) : ";
			std::cin >> yellowDepth;

			yellowDepth = std::clamp(yellowDepth, 1, 15);
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

		int continueChoice = 0;

		std::cout << "\nVoulez-vous recommencer ? (1: Oui, 0: Non) : ";
		std::cin >> continueChoice;

		if (!continueChoice)
		{
			loop = false;
		}
		else
			system("cls");

	}

	std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
	std::cin.get();

	return 0;
}
