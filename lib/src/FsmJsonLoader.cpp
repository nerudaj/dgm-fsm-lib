#include <DGM/classes/FsmJsonLoader.hpp>
#include <fstream>
#include <nlohmann/json.hpp>

using StateMap = dgm::fsm::loader::StateMap;

StateMap dgm::fsm::JsonLoader::loadFromFile(const std::string& path) const
{
	std::ifstream load(path);
	return loadFromStream(load);
}

StateMap dgm::fsm::JsonLoader::loadFromStream(std::istream& stream) const
{
	nlohmann::json json;
	stream >> json;

	StateMap result;

	// Preparse state names
	std::map<std::string, unsigned> stateToIndex;
	unsigned currentIndex = 0;
	for (auto&& state : json)
	{
		stateToIndex[state["name"]] = currentIndex;
		++currentIndex;
	}

	currentIndex = 0;
	for (auto&& state : json)
	{
		result[currentIndex].name = state["name"];

		for (auto&& transition : state["transitions"])
		{
			result[currentIndex].transitions.push_back(
				{
					transition["condition"].get<std::string>(),
					stateToIndex.at(transition["target"])
				}
			);
		}

		for (auto&& logic : state["behaviors"])
		{
			result[currentIndex].behaviors.push_back(logic.get<std::string>());
		}

		result[currentIndex].defaultTransition = stateToIndex.at(state["defaultTransition"]);

		++currentIndex;
	}

	return result;
}
