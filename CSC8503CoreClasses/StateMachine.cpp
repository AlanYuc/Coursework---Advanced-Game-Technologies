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
	//�������״ָ̬�� s ��ӵ� allStates ������ĩβ
	//û��ʹ��push_back�����Ա��ⲻ��Ҫ�Ĺ���Ϳ�������
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

		//ʹ�� equal_range ������ȡ�ӵ�ǰ�״̬����������ת���ķ�Χ��
		//���� range ��һ�� std::pair������ first ��Ա��ָ��Χ��ʼ�ĵ�������
		//�� second ��Ա��ָ��Χ�����ĵ�������
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