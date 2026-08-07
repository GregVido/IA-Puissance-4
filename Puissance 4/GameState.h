#include "Move.h"
#include "Box.h"

struct GameState
{
	Box winner;
	int nbWin;
	int nb3Token;
	int nb2Token;
	Box currentPlayer;
};
