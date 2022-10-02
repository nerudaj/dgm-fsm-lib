#pragma once

#include <string>
#include <map>
#include <numeric>

#include <Fsm.hpp>
#include <FsmLoader.hpp>
#include <FsmDecorators.hpp>

namespace dgm
{

	namespace fsm
	{

		template<class BlackboardType>
		class Factory final
		{
			LoaderInterface& loader;
			std::map<std::string, Condition<BlackboardType>> predicates;
			std::map<std::string, Logic<BlackboardType>> logics;

		private:
			template<class T>
			std::string keysToString(
				const std::map<std::string, T>& map,
				const std::string& delimiter) const
			{
				return std::accumulate(
					++(map.begin()),
					map.end(),
					map.begin()->first,
					[&delimiter](
						const std::string& out,
						const decltype(*(map.begin())) item) -> std::string
					{
						return out
							+ delimiter
							+ item.first;
					});
			}

		public:
			void registerPredicate(
				const std::string& name,
				Condition<BlackboardType> condition)
			{
				predicates[name] = condition;
			}

			void registerLogic(
				const std::string& name,
				Logic<BlackboardType> logic)
			{
				logics[name] = logic;
			}

			[[nodiscard]]
			std::string getAnnotations() const
			{
				const auto serializedPredicates = keysToString(predicates, ",\n");
				const auto seralizedLogics = keysToString(logics, ",\n");

				return std::vformat(
					"{{\n"
					"\"predicates\": [\n{}\n]\n"
					"\"behaviors\": [\n{}\n]\n"
					"}}", std::make_format_args(
						serializedPredicates,
						seralizedLogics));
			}

			[[nodiscard]]
			Fsm<BlackboardType, unsigned> loadFromFile(const std::string& path) const
			{
				auto states = loader.loadFromFile(path);

				std::map<unsigned, State<BlackboardType, unsigned>> finalStates;
				auto getCheckedStateId = [&] (unsigned id) -> unsigned
				{
					if (id >= states.size())
					{
						throw std::runtime_error("Referencing state that does not exist");
					}
					return id;
				};

				for (auto&& [stateId, state] : states)
				{
					auto& fstate = finalStates[stateId];

					for (auto&& transition : state.transitions)
					{
						fstate.transitions.push_back({
							predicates.at(transition.first),
							getCheckedStateId(transition.second) });
					}

					if (state.behaviors.empty())
					{
						throw std::runtime_error("Behaviors must not be empty");
					}

					Logic<BlackboardType> finalLogic = logics.at(state.behaviors[0]);

					for (unsigned i = 1; i < state.behaviors.size(); i++)
					{
						finalLogic = dgm::fsm::decorator::Merge<BlackboardType>(
							finalLogic,
							logics.at(state.behaviors[i]));
					}

					fstate.logic = finalLogic;
					fstate.targetState = getCheckedStateId(state.defaultTransition);
				}

				return dgm::fsm::Fsm(std::move(finalStates));
			}

			Factory(LoaderInterface& loader)
				: loader(loader)
			{}
		};

	}

}