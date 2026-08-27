#pragma once
#include "IA.h"
class User :
    public IA
{
public:
    Move getMove(Board board, Box player) override;
};

