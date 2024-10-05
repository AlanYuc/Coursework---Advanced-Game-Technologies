#pragma once

#include "MenuPushDownMachine.h"
#include "Debug.h"

class GameMenu
{
public:
	GameMenu();
	~GameMenu();

	void DrawPlayerInfo(int score, int hp);
	void DrawItemsLeft(int itemsNum);
	void DrawTime(float time);
	void DrawFuncSelection(float dt);
	void DrawPlayerModeInfo(bool inPlayerMode);
	void DrawAttackMode(int playerShootType);

	void DrawStartMenu();
	//void SelectPage(int page);
	

protected:
	//PushdownMachine funcSelectionMachine;
	//PushdownMachine* funcSelectionMachine(new StartScreen());
	PushdownMachine* funcSelectionMachine;
	//int page;//表示当前显示第几页
};
		



