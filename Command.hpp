#ifndef COMMAND_H
#define COMMAND_H

#include "Actor.hpp"
#include "Game.h"

class Command {
public:
	virtual ~Command() {};
	void execute(Actor& a) {};
protected:
	Command() {};
};

class MoveCommand : public Command {
public:
	void execute(Actor& a, int x, int y) {
		a.move(x, y);
	}
};


#endif