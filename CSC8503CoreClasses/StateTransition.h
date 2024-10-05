#pragma once

namespace NCL {
	namespace CSC8503 {
		class State;
		//表示一个函数对象，该函数没有参数，返回一个布尔值
		typedef std::function<bool()> StateTransitionFunction;
		class StateTransition	{
		public:
			StateTransition(State* source, State* dest, StateTransitionFunction f) {
				sourceState		 = source;
				destinationState = dest;
				function		 = f;
			}

			bool CanTransition() const {
				return function();
			}

			State* GetDestinationState()  const {
				return destinationState;
			}

			State* GetSourceState() const {
				return sourceState;
			}

		protected:
			State * sourceState;//源状态指针，表示状态过渡的起始状态
			State * destinationState;//目标状态指针，表示状态过渡的目标状态
			StateTransitionFunction function;//状态过渡条件检查函数
		};
	}
}

