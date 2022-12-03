#pragma once

#include <vector>
#include <string>
#include <utility>
#include <map>
#include <istream>

namespace dgm
{
	namespace fsm
	{
		namespace loader
		{
			/**
			 *  Helper structure for representing data
			 *  retrieved via LoaderInterface
			 *
			 *  This structure makes sense to FsmFactory
			 *  which can map string names to registered
			 *  predicates and behaviors.
			 */
			struct State
			{
				std::string name = "";
				std::vector<std::pair<std::string, unsigned>> transitions = {};
				std::vector<std::string> behaviors = {};
				unsigned defaultTransition = 0;
			};

			using StateMap = std::map<unsigned, State>;
		}

		/**
		 *  Interface used by fsm::Factory for loading
		 *  FSM definitions from a storage.
		 *
		 *  Either implement your own loader or you can
		 *  use dgm::fsm::JsonLoader to use JSON format
		 *  favoured by this library.
		 */
		class LoaderInterface
		{
		public:
			[[nodiscard]]
			virtual loader::StateMap loadFromFile(const std::string&) const = 0;

			virtual ~LoaderInterface() = default;
		};
	}
}