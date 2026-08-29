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
#include "Console.h"

#include <chrono>
#include <thread>

#define NOMINMAX
#include <Windows.h>
#include "resource.h"
#include <conio.h>
#include <array>

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

int selectDepth(const std::string& playerName)
{
	system("cls");
	printTitle();

	constexpr const char* WARNING = "\033[38;2;255;190;70m";
	constexpr const char* NORMAL = "\033[38;2;180;195;220m";
	constexpr const char* RESET = "\033[0m";

	printCenteredColored(
		"Attention, plus le chiffre est grand, plus le temps de calcul est long !",
		WARNING,
		RESET
	);

	std::cout << '\n';

	printCenteredColored(
		"Choisis la profondeur de recherche pour le joueur " +
		playerName + " (1-15)",
		NORMAL,
		RESET
	);

	std::cout << "\n\n";

	// Récupère la largeur de la console
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(
		GetStdHandle(STD_OUTPUT_HANDLE),
		&csbi
	);

	const int consoleWidth =
		csbi.srWindow.Right -
		csbi.srWindow.Left + 1;

	// On place le curseur approximativement au centre
	COORD pos;
	pos.X = static_cast<SHORT>(consoleWidth / 2 - 1);
	pos.Y = csbi.dwCursorPosition.Y;

	SetConsoleCursorPosition(
		GetStdHandle(STD_OUTPUT_HANDLE),
		pos
	);

	int depth = 1;
	std::cin >> depth;

	return std::clamp(depth, 1, 15);
}

int selectIA(const std::string& playerName)
{
	const std::array<std::string, 4> choices =
	{
		"Facile",
		"Forte",
		"Imbattable",
		"Humain"
	};

	int selected = 0;

	constexpr const char* RESET = "\033[0m";

	constexpr const char* NORMAL_TEXT =
		"\033[38;2;180;195;220m";

	// Style de la sélection :
	// gras + fond bleu + texte blanc
	constexpr const char* SELECT_STYLE =
		"\033[1m"
		"\033[48;2;45;120;255m"
		"\033[38;2;255;255;255m";

	while (true)
	{
		system("cls");

		printTitle();

		printCentered(
			"Choisis le joueur " + playerName
		);

		std::cout << '\n';

		for (int i = 0; i < static_cast<int>(choices.size()); ++i)
		{
			if (i == selected)
			{
				// Le rectangle entier est centré
				const std::string text =
					"  > " + choices[i] + " <  ";

				printCenteredColored(
					text,
					SELECT_STYLE,
					RESET
				);
			}
			else
			{
				// On centre uniquement le vrai texte,
				// sans compter les codes ANSI
				printCenteredColored(
					choices[i],
					NORMAL_TEXT,
					RESET
				);
			}
		}

		std::cout << '\n';

		printCentered(
			"↑ / ↓ pour naviguer - Entrée pour sélectionner"
		);

		const int key = _getch();

		if (key == 0 || key == 224)
		{
			const int arrow = _getch();

			// Flèche haut
			if (arrow == 72)
			{
				--selected;

				if (selected < 0)
				{
					selected =
						static_cast<int>(choices.size()) - 1;
				}
			}

			// Flèche bas
			else if (arrow == 80)
			{
				++selected;

				if (selected >= static_cast<int>(choices.size()))
				{
					selected = 0;
				}
			}
		}
		// Entrée
		else if (key == 13)
		{
			return selected;
		}
	}
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

		red = selectIA("Rouge");

		if (red == 1 || red == 2)
		{
			std::cout << "\nAttention, plus le chiffre est grand, plus le temps de calcul est long !\nChoisis la profondeur de recherche pour le joueur Rouge (1-15) : ";
			std::cin >> redDepth;

			redDepth = std::clamp(redDepth, 1, 15);
		}

		yellow = selectIA("Jaune");

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
