#include "GameMenu.h"

GameMenu::GameMenu() {
	//funcSelectionMachine->SetinitialState(new StartScreen());
	funcSelectionMachine = new PushdownMachine(new StartScreen());
	//page = 0;
}

GameMenu::~GameMenu()
{
}

void GameMenu::DrawPlayerInfo(int score, int hp) {
	Debug::Print("Player    HP: " + std::to_string(hp), Vector2(65, 5), Debug::GREEN);
	Debug::Print("Player Score: " + std::to_string(score), Vector2(65, 10), Debug::RED);
}

void GameMenu::DrawItemsLeft(int itemsNum) {
	Debug::Print("Items   Left: " + std::to_string(itemsNum), Vector2(65, 15));
}

void GameMenu::DrawTime(float time) {
	//dt近似为一个固定值，还需要一个累加器
	Debug::Print("Time Left:" + std::to_string(time), Vector2(5, 5));
}

void GameMenu::DrawFuncSelection(float dt) {
	//PushdownMachine funcSelectionMachine(new StartScreen());
	/*if (!funcSelectionMachine->Update(dt)) {
		return;
	}*/
	//funcSelectionMachine = new PushdownMachine(new StartScreen());
	//Debug::Print("Press R to open the menu!\n", Vector2(5, 30));
	funcSelectionMachine->Update(dt);

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM0)) {
		std::cout << "reset!";
		funcSelectionMachine->Reset(new StartScreen());
	}

}

void GameMenu::DrawPlayerModeInfo(bool inPlayerMode)
{
	if (inPlayerMode) {
		Debug::Print("Now in Player mode", Vector2(65, 90));
	}
	else {
		Debug::Print("Player mode exit", Vector2(65, 90));
	}
}

void GameMenu::DrawStartMenu() {
	Debug::Print("YUC: TRY TO GET 2 OR MORE COINS!", Vector2(20, 35));
	Debug::Print("Press F to start the game!", Vector2(25, 50));
	Debug::Print("Press ESC to quit the game!", Vector2(25, 55));
	Debug::Print("Press E to back to this menu!", Vector2(25, 60));
}

void GameMenu::DrawAttackMode(int playerShootType) {
	switch (playerShootType) {
	case(0):Debug::Print("PUSH MODE", Vector2(70, 85), Vector4(1, 0, 0, 1)); break;
	case(1):Debug::Print("PULL MODE", Vector2(70, 85), Vector4(1, 0, 0, 1)); break;
	case(2):Debug::Print("SWING MODE", Vector2(70, 85), Vector4(1, 0, 0, 1)); break;
	case(3):Debug::Print("DESTROY MODE", Vector2(70, 85), Vector4(1, 0, 0, 1)); break;
	}
}
//void GameMenu::SelectPage(int page) {
//	switch (page) {
//	case (0):
//		Debug::Print("Press R to open the menu!\n", Vector2(5, 30)); 
//		break;
//	case (1):
//		Debug::Print("Please select!\n", Vector2(5, 30));
//		Debug::Print("1.Add a force!\n", Vector2(5, 35));
//		Debug::Print("2.Use an item!\n", Vector2(5, 40));
//		Debug::Print("9.Go Back!\n", Vector2(5, 45));
//		Debug::Print("0.Quit!\n", Vector2(5, 50));
//		break;
//	case (2):
//		Debug::Print("Please select!\n", Vector2(5, 30));
//		Debug::Print("1.Add a force!\n", Vector2(5, 35));
//		Debug::Print("9.Go Back!\n", Vector2(5, 40));
//		Debug::Print("0.Quit!\n", Vector2(5, 45));
//		break;
//	case (3):
//		Debug::Print("Please select!\n", Vector2(5, 30));
//		Debug::Print("1.Speed up!\n", Vector2(5, 35));
//		Debug::Print("9.Go Back!\n", Vector2(5, 40));
//		Debug::Print("0.Quit!\n", Vector2(5, 45));
//		break;
//	}
//}