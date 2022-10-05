#pragma once

#include <functional>
#include <vector>
#include <map>
#include <utility>
#include <string>
#include <format>
#include <ostream>
#include <iostream>

namespace dgm
{
	namespace fsm
	{
		template<typename T>
		concept EnumConcept = true;//std::is_enum_v<T>;

		template<class BlackboardType>
		using Condition = std::function<bool(const BlackboardType&)>;

		template<class BlackboardType, EnumConcept StateType>
		using Transition = std::pair<Condition<BlackboardType>, StateType>;

		template<class BlackboardType>
		using Logic = std::function<void(BlackboardType&)>;

		template<class BlackboardType, EnumConcept StateType>
		struct State
		{
			std::vector<Transition<BlackboardType, StateType>> transitions;
			Logic<BlackboardType> logic;
			StateType targetState = StateType();
		};

		/**
		 * \brief Final state machine implementation
		 *
		 * This FSM is tailored for game AI and as such,
		 * it has some rules on how the states and transitions look like.
		 *
		 * It even has restrictions on what can be passed down to each
		 * behaviour or predicate function.
		 *
		 * Namely there are two template parameters:
		 * Ctx - stands for Context. This is supposed to be const at all
		 * times and can contain like player position, map, obstacle placement, etc.
		 *
		 * BB - stands for BlackboardType. This is mutable context of the AI,
		 * this should contain currently selected target, timers, etc.
		 *
		 * Last template StateType is the type of the State enumeration. It has to be
		 * an enum.
		 *
		 *
		 * Rules that apply to this FSM:
		 * 1) Every state has a set of conditions. Each condition has associated state code.
		 *    If condition evaluates to true, it transitions to associated state.
		 * 2) Conditions cannot mutate state, not even BlackboardType.
		 * 3) If all conditions are evaluated to false, default logic runs. Default logic
		 *    is permitted to mutate BlackboardType state.
		 * 4) After default logic executes, then FSM is permitted to transition to another state
		 *    or it keeps the current one.
		 * 4) Each update tick of FSM ends with a transition. So either a condition evaluates to
		 *    true and transitions, or default logic executes and then transitions (keeping the current state
		 *    is also transition). This prevents FSM from freezing the app by constantly looping.
		 */
		template<class BlackboardType, EnumConcept StateType>
		class Fsm final
		{
		private:
			std::map<StateType, State<BlackboardType, StateType>> states;
			StateType currentState = StateType();

			// Logging
			bool loggingEnabled = false;
			std::reference_wrapper<std::ostream> log = std::cout;
			std::map<StateType, std::string> stateToString;

		private:
			[[nodiscard]]
			std::string getStateName(StateType state) const noexcept
			{
				return stateToString.count(state)
					? stateToString.at(state)
					: "UNDEFINED(" + std::to_string(static_cast<int>(state)) + ")";
			}

			void logCurrentState(const BlackboardType& b)
			{
				if (!loggingEnabled) return;

				log.get() << std::format(
					"FSM::update(State = {}):\n",
					getStateName(currentState));
			}

			void logTransitionTaken(bool defaultTransition = false, bool loop = false)
			{
				if (!loggingEnabled)
					return;

				log.get() << std::format("  {}, {}{}\n",
					defaultTransition ? "behavior executed" : "condition hit",
					loop ? "looping" : "jumping to ",
					loop ? "" : getStateName(currentState));
			}

		public:
			Fsm(std::map<StateType, State<BlackboardType, StateType>>&& states)
				: states(states)
			{}

			void update(BlackboardType& blackboard)
			{
				logCurrentState(blackboard);

				for (auto&& transition : states[currentState].transitions)
				{
					if (transition.first(blackboard))
					{
						currentState = transition.second;
						logTransitionTaken();
						return;
					}
				}

				states[currentState].logic(blackboard);
				const bool looping = currentState == states[currentState].targetState;
				currentState = states[currentState].targetState;
				logTransitionTaken(true, looping);
			}

			void reset(StateType state) noexcept
			{
				currentState = state;
			}

			void setStateToStringHelper(
				std::map<StateType, std::string>&& stateToString)
			{
				this->stateToString = stateToString;
			}

			void setLogging(
				bool enabled,
				std::ostream& logger = std::cout)
			{
				loggingEnabled = enabled;
				log = logger;
			}

			[[nodiscard]] constexpr auto getState() const noexcept -> StateType
			{
				return currentState;
			}
		};
	}
}
