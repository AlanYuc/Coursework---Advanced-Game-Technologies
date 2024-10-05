#pragma once
#include "Window.h"
#include "PushdownState.h"
#include "PushdownMachine.h"
#include <iostream>
#include "Debug.h"
using namespace NCL;
using namespace CSC8503;
using namespace std;


class ItemScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM1)) {
			//添加相应的功能
			//return PushdownResult::Pop;
		}
		else if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM9)) {
			return PushdownResult::Pop;
		}



		Debug::Print("Please select!\n", Vector2(5, 30));
		Debug::Print("1.To be continueA!\n", Vector2(5, 35));
		Debug::Print("9.Go Back!\n", Vector2(5, 40));
		Debug::Print("0.Quit!\n", Vector2(5, 45));

		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		//std::cout << "Press U to unpause game!\n";
		//Debug::Print("Please select!\n", Vector2(5, 30));
		//Debug::Print("1.Speed up!\n", Vector2(5, 35));
		//Debug::Print("9.Go Back!\n", Vector2(5, 40));
		//Debug::Print("0.Quit!\n", Vector2(5, 45));
	}
};
class PhysicsScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM1)) {
			//添加相应的功能
			//return PushdownResult::Pop;
		}
		else if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM9)) {
			return PushdownResult::Pop;
		}

		

		Debug::Print("Please select!\n", Vector2(5, 30));
		Debug::Print("1.To be continueB!\n", Vector2(5, 35));
		Debug::Print("9.Go Back!\n", Vector2(5, 40));
		Debug::Print("0.Quit!\n", Vector2(5, 45));

		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		//std::cout << "Press U to unpause game!\n";
		//Debug::Print("Please select!\n", Vector2(5, 30));
		//Debug::Print("1.Add a force!\n", Vector2(5, 35));
		//Debug::Print("9.Go Back!\n", Vector2(5, 40));
		//Debug::Print("0.Quit!\n", Vector2(5, 45));

	}
};
class SecondScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		pauseReminder -= dt;
		if (pauseReminder < 0) {
			//std::cout << "Press P to pause game, or F1 to return to main menu!\n";
			pauseReminder += 1.0f;
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM1)) {
			*newState = new PhysicsScreen();
			return PushdownResult::Push;
		}
		else if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM2)) {
			*newState = new ItemScreen();
			return PushdownResult::Push;
		}
		else if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM9)) {
			//std::cout << "Returning to main menu!\n";
			return PushdownResult::Pop;
		}

		

		

		Debug::Print("Please select!\n", Vector2(5, 30));
		Debug::Print("1.To be continueC!\n", Vector2(5, 35));
		Debug::Print("2.To be continueD!\n", Vector2(5, 40));
		Debug::Print("9.Go Back!\n", Vector2(5, 45));
		Debug::Print("0.Quit!\n", Vector2(5, 50));

		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		//std::cout << "Preparing to mine coins!\n";
		/*Debug::Print("Please select!\n", Vector2(5, 30));
		Debug::Print("1.Add a force!\n", Vector2(5, 35));
		Debug::Print("2.Use an item!\n", Vector2(5, 40));
		Debug::Print("9.Go Back!\n", Vector2(5, 45));
		Debug::Print("0.Quit!\n", Vector2(5, 50));*/
	}

protected:
	float pauseReminder = 1;
};
class StartScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyDown(KeyCodes::R)) {
			*newState = new SecondScreen();
			return PushdownResult::Push;
		}
		/*if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM9)) {
			return PushdownResult::Pop;
		}*/

		Debug::Print("Press R to open the menu!\n", Vector2(5, 30));

		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		//std::cout << "Welcome to a really awesome game!\n";
		//std::cout << "Press Space To Begin or escape to quit!\n";
		//Debug::Print("Press R to open the menu!\n", Vector2(5, 30));
		
	}
};