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

int getConsoleWidth()
{
	CONSOLE_SCREEN_BUFFER_INFO csbi{};

	if (GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&csbi))
	{
		return csbi.srWindow.Right - csbi.srWindow.Left + 1;
	}

	return 80;
}

int getUtf8VisualLength(const std::string& text)
{
	int length = 0;

	for (unsigned char c : text)
	{
		// Un caractère UTF-8 commence lorsque les deux bits de poids fort
		// ne sont pas "10".
		if ((c & 0xC0) != 0x80)
			++length;
	}

	return length;
}

void printCentered(const std::string& text)
{
	const int consoleWidth = getConsoleWidth();
	const int textWidth = getUtf8VisualLength(text);

	const int padding =
		std::max(0, (consoleWidth - textWidth) / 2);

	std::cout
		<< std::string(padding, ' ')
		<< text
		<< '\n';
}

void printTitle()
{
	constexpr const char* BLUE = "\033[38;2;45;120;255m";
	constexpr const char* BOLD = "\033[1m";
	constexpr const char* RESET = "\033[0m";

	std::cout << '\n';
	std::cout << BLUE << BOLD;

	printCentered(R"(██████╗ ██╗   ██╗██╗███████╗███████╗ █████╗ ███╗   ██╗ ██████╗███████╗    ██╗  ██╗)");
	printCentered(R"(██╔══██╗██║   ██║██║██╔════╝██╔════╝██╔══██╗████╗  ██║██╔════╝██╔════╝    ██║  ██║)");
	printCentered(R"(██████╔╝██║   ██║██║███████╗███████╗███████║██╔██╗ ██║██║     █████╗      ███████║)");
	printCentered(R"(██╔═══╝ ██║   ██║██║╚════██║╚════██║██╔══██║██║╚██╗██║██║     ██╔══╝      ╚════██║)");
	printCentered(R"(██║     ╚██████╔╝██║███████║███████║██║  ██║██║ ╚████║╚██████╗███████╗         ██║)");
	printCentered(R"(╚═╝      ╚═════╝ ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝ ╚═════╝╚══════╝         ╚═╝)");

	std::cout << RESET << '\n';

	printCentered("IA - By GregVido");

	std::cout << '\n';
}

int main()
{
	using namespace std::chrono_literals;
	bool loop = true;

	setIcon();

	SetConsoleOutputCP(CP_UTF8);
	SetConsoleCP(CP_UTF8);

	while (loop) {
		printTitle();

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

		std::cout << "Choisis le type d'IA pour le joueur Jaune (0: Facile, 1: Forte, 2: Imbattable, 3: Humain) : ";
		std::cin >> yellow;

		if (yellow == 1 || yellow == 2)
		{
			std::cout << "\nAttention, plus le chiffre est grand, plus le temps de calcul est long !\nChoisis la profondeur de recherche pour le joueur Jaune (1-15) : ";
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
