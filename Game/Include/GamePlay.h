#ifndef GAMEPLAY_H
#define GAMEPLAY_H

#include "Direct3D.h"

class GamePlay
{
private:

public:
	GamePlay();
	virtual ~GamePlay();

	void Update();
	void Render(Direct3D*& direct3D);
};

#endif