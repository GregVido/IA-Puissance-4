#pragma once
#include "IA.h"
class User :
    public IA
{
    Move getMove(Board board, Box player) override;
};

