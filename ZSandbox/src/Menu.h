#pragma once

#include <XYZ.h>


class Menu
{
public:
	bool GameRunning();
	bool GameStart();
	void SetEndGame();
	void Update();
private:
	bool m_GameStarted = false;
	bool m_GameRunning = false;
	bool m_GameOver = false;
};