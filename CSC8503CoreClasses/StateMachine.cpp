#include "StateMachine.h"
#include "State.h"
#include "StateTransition.h"

using namespace NCL::CSC8503;

StateMachine::StateMachine()	{
	activeState = nullptr;
}

StateMachine::~StateMachine()	{
	for (auto& i : allStates) {
		delete i;
	}
	for (auto& i : allTransitions) {
		delete i.second;
	}
}

void StateMachine::AddState(State* s) {
	//将传入的状态指针 s 添加到 allStates 向量的末尾
	//没有使用push_back，可以避免不必要的构造和拷贝操作
	allStates.emplace_back(s);

	if (activeState == nullptr) {
		activeState = s;
	}
}

void StateMachine::AddTransition(StateTransition* t) {
	allTransitions.insert(std::make_pair(t->GetSourceState(), t));
}

void StateMachine::Update(float dt) {
	if (activeState) {
		activeState->Update(dt);

		//使用 equal_range 函数获取从当前活动状态出发的所有转换的范围。
		//这里 range 是一个 std::pair，其中 first 成员是指向范围起始的迭代器，
		//而 second 成员是指向范围结束的迭代器。
		//Get the transition set starting from this state node;
		std::pair<TransitionIterator, TransitionIterator> range = allTransitions.equal_range(activeState);

		for (auto& i = range.first; i != range.second; ++i) {
			if (i->second->CanTransition()) {
				State* newState = i->second->GetDestinationState();
				activeState = newState;
			}
		}
	}
}