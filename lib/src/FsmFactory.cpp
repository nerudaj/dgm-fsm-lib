#include <DGM/classes/FsmFactory.hpp>

#include <algorithm>
#include <numeric>

std::string serializeVector(const std::vector<std::string>& words, std::string delimiter)
{
	return std::accumulate(
		(++words.begin()),
		words.end(),
		*(words.begin()),
		[&] (const std::string& aggreg, const std::string& word) -> std::string
		{
			return aggreg + delimiter + word;
		});
}

std::string dgm::fsm::priv::getAnnotations(
	const std::vector<std::string>& predicateNames,
	const std::vector<std::string>& behaviorNames)
{
	return std::format(
		"{{\n"
		"    \"predicates\": [{}]\n"
		"    \"behaviors\": [{}]\n"
		"}}",
		serializeVector(predicateNames, ", "),
		serializeVector(behaviorNames, ", ")
	);
}