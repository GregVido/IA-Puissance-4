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
	constexpr const char* RESET = "\033[0m";

	constexpr const char* RED = "\033[38;2;255;65;65m";
	constexpr const char* YELLOW = "\033[38;2;255;215;40m";
	constexpr const char* EMPTY = "\033[38;2;220;230;245m";

	constexpr const char* NORMAL_TEXT =
		"\033[38;2;180;195;220m";

	constexpr const char* CATEGORY_TEXT =
		"\033[38;2;100;160;255m";

	constexpr const char* SELECT_STYLE =
		"\033[1m"
		"\033[48;2;45;120;255m"
		"\033[38;2;255;255;255m";

	// ==============================
	// MENU PRINCIPAL : IA / MANUEL
	// ==============================

	const std::array<std::string, 2> categories =
	{
		"IA",
		"Manuel"
	};

	int categorySelected = 0;

	while (true)
	{
		system("cls");

		printTitle();

		if (playerName == "Rouge")
			printCenteredColored(
				"Joueur Rouge",
				RED,
				RESET
			);
		else if (playerName == "Jaune")
			printCenteredColored(
				"Joueur Jaune",
				YELLOW,
				RESET
			);

		std::cout << "\n\n";

		for (int i = 0; i < static_cast<int>(categories.size()); ++i)
		{
			if (i == categorySelected)
			{
				const std::string text =
					"  > " + categories[i] + " <  ";

				printCenteredColored(
					text,
					SELECT_STYLE,
					RESET
				);
			}
			else
			{
				printCenteredColored(
					categories[i],
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

			if (arrow == 72) // Haut
			{
				--categorySelected;

				if (categorySelected < 0)
					categorySelected =
					static_cast<int>(categories.size()) - 1;
			}
			else if (arrow == 80) // Bas
			{
				++categorySelected;

				if (categorySelected >= static_cast<int>(categories.size()))
					categorySelected = 0;
			}
		}
		else if (key == 13) // Entrée
		{
			// =========================
			// MANUEL
			// =========================

			if (categorySelected == 1)
			{
				return 3; // User()
			}

			// =========================
			// SOUS-MENU IA
			// =========================

			const std::array<std::string, 3> iaChoices =
			{
				"Facile",
				"Forte",
				"Imbattable"
			};

			int iaSelected = 0;

			while (true)
			{
				system("cls");

				printTitle();

				printCenteredColored(
					"IA",
					CATEGORY_TEXT,
					RESET
				);

				std::cout << '\n';

				printCentered(
					"Choisis la difficulté pour " + playerName
				);

				std::cout << "\n\n";

				for (int i = 0;
					i < static_cast<int>(iaChoices.size());
					++i)
				{
					if (i == iaSelected)
					{
						const std::string text =
							"  > " + iaChoices[i] + " <  ";

						printCenteredColored(
							text,
							SELECT_STYLE,
							RESET
						);
					}
					else
					{
						printCenteredColored(
							iaChoices[i],
							NORMAL_TEXT,
							RESET
						);
					}
				}

				std::cout << '\n';

				printCentered(
					"↑ / ↓ pour naviguer - Entrée pour sélectionner - Échap pour revenir"
				);

				const int iaKey = _getch();

				if (iaKey == 0 || iaKey == 224)
				{
					const int arrow = _getch();

					if (arrow == 72) // Haut
					{
						--iaSelected;

						if (iaSelected < 0)
							iaSelected =
							static_cast<int>(iaChoices.size()) - 1;
					}
					else if (arrow == 80) // Bas
					{
						++iaSelected;

						if (iaSelected >= static_cast<int>(iaChoices.size()))
							iaSelected = 0;
					}
				}
				else if (iaKey == 13) // Entrée
				{
					// 0 = Facile
					// 1 = Forte
					// 2 = Imbattable
					return iaSelected;
				}
				else if (iaKey == 27) // Échap
				{
					break;
				}
			}
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
			redDepth = selectDepth("Rouge");

		yellow = selectIA("Jaune");

		if (yellow == 1 || yellow == 2)
			yellowDepth = selectDepth("Jaune");

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

			const std::string moveText =
				"Joueur : " +
				std::string(player == Box::RED ? "Rouge" : "Jaune") +
				" joue sur la colonne " +
				std::to_string(move.column);

			printCentered(moveText);

			board.next();
		}

		delete redIA;
		delete yellowIA;

		const Box winner = board.getWinner();

		const std::string winnerText =
			"Gagnant : " +
			std::string(
				winner == Box::RED
				? "Rouge"
				: winner == Box::YELLOW
				? "Jaune"
				: "Personne"
			);

		printCentered(winnerText);

		std::cout << "\n\n";

		int continueChoice = 1; // 1 = Oui, 0 = Non

		CONSOLE_SCREEN_BUFFER_INFO csbi;
		GetConsoleScreenBufferInfo(
			GetStdHandle(STD_OUTPUT_HANDLE),
			&csbi
		);

		const int selectionY = csbi.dwCursorPosition.Y;

		while (true)
		{
			GetConsoleScreenBufferInfo(
				GetStdHandle(STD_OUTPUT_HANDLE),
				&csbi
			);

			const int consoleWidth =
				csbi.srWindow.Right -
				csbi.srWindow.Left + 1;

			const std::string question =
				"Voulez-vous recommencer ?  ";

			const std::string yesText = "  OUI  ";
			const std::string noText = "  NON  ";

			const std::string fullText =
				question + yesText + "    " + noText;

			const int startX = std::max(
				0,
				(consoleWidth - static_cast<int>(fullText.size())) / 2
			);

			COORD lineStart;
			lineStart.X = 0;
			lineStart.Y = static_cast<SHORT>(selectionY);

			// Efface uniquement la ligne de sélection
			SetConsoleCursorPosition(
				GetStdHandle(STD_OUTPUT_HANDLE),
				lineStart
			);

			std::cout << std::string(consoleWidth, ' ');

			// Replace le curseur au début du texte centré
			COORD textPos;
			textPos.X = static_cast<SHORT>(startX);
			textPos.Y = static_cast<SHORT>(selectionY);

			SetConsoleCursorPosition(
				GetStdHandle(STD_OUTPUT_HANDLE),
				textPos
			);

			std::cout << question;

			if (continueChoice == 1)
			{
				std::cout
					<< "\033[1m"
					<< "\033[48;2;45;120;255m"
					<< "\033[38;2;255;255;255m"
					<< yesText
					<< "\033[0m"
					<< "    "
					<< noText;
			}
			else
			{
				std::cout
					<< yesText
					<< "    "
					<< "\033[1m"
					<< "\033[48;2;45;120;255m"
					<< "\033[38;2;255;255;255m"
					<< noText
					<< "\033[0m";
			}

			const int key = _getch();

			if (key == 0 || key == 224)
			{
				const int arrow = _getch();

				if (arrow == 75) // Gauche
					continueChoice = 1;

				else if (arrow == 77) // Droite
					continueChoice = 0;
			}
			else if (key == 13) // Entrée
			{
				break;
			}
		}

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
