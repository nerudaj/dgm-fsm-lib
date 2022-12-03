#pragma once

#include <DGM/classes/Fsm.hpp>

namespace dgm
{
	namespace fsm
	{
		template<class BlackboardType, StateTypeConcept StateType>
		class Builder;

		namespace builders
		{
			template<class BlackboardType, StateTypeConcept StateType>
			class DefaultTransitionBuilder final
			{
				Builder<BlackboardType, StateType> origBuilder;
				std::vector<Transition<BlackboardType, StateType>> transitions;
				Logic<BlackboardType> logic;

			public:
				DefaultTransitionBuilder(
					Builder<BlackboardType, StateType>&& origBuilder,
					std::vector<Transition<BlackboardType, StateType>> transitions,
					Logic<BlackboardType> logic)
					: origBuilder(origBuilder)
					, transitions(transitions)
					, logic(logic)
				{}

				auto andGoTo(StateType state)
				{
					origBuilder.addState(
						origBuilder.getCurrentlyAddedState(),
						State<BlackboardType, StateType>({
							.transitions = transitions,
							.logic = logic,
							.targetState = state
							}));
					return std::move(origBuilder);
				}

				auto andLoop()
				{
					origBuilder.addState(
						origBuilder.getCurrentlyAddedState(),
						State<BlackboardType, StateType>({
							.transitions = transitions,
							.logic = logic,
							.targetState = origBuilder.getCurrentlyAddedState()
							}));
					return std::move(origBuilder);
				}
			};

			// Forward declaring transition builder because there is cyclic reference between it and StateBuilder
			template<class BlackboardType, typename StateType>
			class TransitionBuilder;

			template<class BlackboardType, StateTypeConcept StateType>
			class StateBuilder final
			{
				Builder<BlackboardType, StateType> origBuilder;
				std::vector<Transition<BlackboardType, StateType>> transitions;

			public:
				StateBuilder(
					Builder<BlackboardType, StateType>&& origBuilder,
					std::vector<Transition<BlackboardType, StateType>> transitions)
					: origBuilder(origBuilder)
					, transitions(transitions)
				{}

				auto orWhen(Condition<BlackboardType> condition)
				{
					return TransitionBuilder<BlackboardType, StateType>(
						std::move(origBuilder),
						transitions,
						condition);
				}

				auto otherwiseExec(Logic<BlackboardType> logic)
				{
					return DefaultTransitionBuilder<BlackboardType, StateType>(
						std::move(origBuilder),
						transitions,
						logic);
				}
			};

			template<class BlackboardType, typename StateType>
			class TransitionBuilder final
			{
				Builder<BlackboardType, StateType> origBuilder;
				std::vector<Transition<BlackboardType, StateType>> transitions;
				Condition<BlackboardType> condition;

			public:
				TransitionBuilder(
					Builder<BlackboardType, StateType>&& origBuilder,
					std::vector<Transition<BlackboardType, StateType>> transitions,
					Condition<BlackboardType> condition)
					: origBuilder(origBuilder)
					, transitions(transitions)
					, condition(condition)
				{}

				auto goTo(StateType state)
				{
					transitions.push_back(Transition<BlackboardType, StateType>(condition, state));
					return StateBuilder<BlackboardType, StateType>(
						std::move(origBuilder),
						transitions);
				}
			};

			template<class BlackboardType, StateTypeConcept StateType>
			class EmptyStateBuilder final
			{
				Builder<BlackboardType, StateType> origBuilder;

			public:
				EmptyStateBuilder(Builder<BlackboardType, StateType>&& origBuilder)
					: origBuilder(origBuilder)
				{}

				auto when(Condition<BlackboardType> condition)
				{
					return TransitionBuilder<BlackboardType, StateType>(
						std::move(origBuilder),
						{},
						condition);
				}

				auto exec(Logic<BlackboardType> logic)
				{
					return DefaultTransitionBuilder<BlackboardType, StateType>(
						std::move(origBuilder),
						{},
						logic);
				}
			};
		}

		template<class BlackboardType, StateTypeConcept StateType>
		class Builder final
		{
		private:
			std::map<StateType, State<BlackboardType, StateType>> states;
			StateType currentlyAddedState;

		public:
			auto with(StateType state)
			{
				currentlyAddedState = state;
				return builders::EmptyStateBuilder<BlackboardType, StateType>(std::move(*this));
			}

			StateType getCurrentlyAddedState() const
			{
				return currentlyAddedState;
			}

			void addState(StateType code, State<BlackboardType, StateType> state)
			{
				states[code] = state;
			}

			auto build()
			{
				return Fsm<BlackboardType, StateType>(std::move(states));
			}
		};
	}
}
