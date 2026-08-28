#include "User.h"
#include "Console.h"

#include <conio.h>
#include <iostream>



Move User::getMove(Board board, Box player)
{
	// Commence au milieu
	int selectedColumn = board.getWidth() / 2;

	// Si la colonne centrale est pleine,
	// cherche une colonne disponible
	if (board.isColumnFull(selectedColumn))
	{
		for (int column = 0; column < board.getWidth(); ++column)
		{
			if (!board.isColumnFull(column))
			{
				selectedColumn = column;
				break;
			}
		}
	}

	while (true)
	{
		// Efface l'écran et remet le curseur en haut
		std::cout << "\033[2J\033[H";

		// Affiche la grille avec la colonne sélectionnée
		board.draw(selectedColumn);

		printCentered("← → : choisir une colonne    Entrée : jouer\n");

		const int key = _getch();

		// Touche spéciale (flèches)
		if (key == 0 || key == 224)
		{
			const int arrow = _getch();

			// ==========================
			// Flèche gauche
			// ==========================
			if (arrow == 75)
			{
				int nextColumn = selectedColumn;

				do
				{
					--nextColumn;

					if (nextColumn < 0)
						nextColumn = board.getWidth() - 1;

				} while (
					board.isColumnFull(nextColumn) &&
					nextColumn != selectedColumn
					);

				if (!board.isColumnFull(nextColumn))
					selectedColumn = nextColumn;
			}

			// ==========================
			// Flèche droite
			// ==========================
			else if (arrow == 77)
			{
				int nextColumn = selectedColumn;

				do
				{
					++nextColumn;

					if (nextColumn >= board.getWidth())
						nextColumn = 0;

				} while (
					board.isColumnFull(nextColumn) &&
					nextColumn != selectedColumn
					);

				if (!board.isColumnFull(nextColumn))
					selectedColumn = nextColumn;
			}
		}

		// ==========================
		// Entrée
		// ==========================
		else if (key == 13)
		{
			if (!board.isColumnFull(selectedColumn))
			{
				Move move;
				move.box = player;
				move.column = selectedColumn;

				return move;
			}
		}
	}
}