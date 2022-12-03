#include <catch.hpp>
#include <DGM/classes/FsmJsonLoader.hpp>

TEST_CASE("loadFromStream", "[FsmJsonLoader]")
{
	dgm::fsm::JsonLoader loader;

	SECTION("Can load empty JSON")
	{
		std::stringstream input(R"([])");

		auto states = loader.loadFromStream(input);

		REQUIRE(states.empty());
	}

	SECTION("Can load single looping state without transitions")
	{
		std::stringstream input(R"(
[
	{
		"name": "StateStart",
		"transitions": [],
		"behaviors": [
			"doNothing1",
			"doNothing2"
		],
		"defaultTransition": "StateStart"
	}
]
)");
		auto states = loader.loadFromStream(input);

		REQUIRE(states.size() == 1u);
		REQUIRE(states[0].transitions.empty());
		REQUIRE(states[0].behaviors.size() == 2u);
		REQUIRE(states[0].behaviors[0] == "doNothing1");
		REQUIRE(states[0].behaviors[1] == "doNothing2");
		REQUIRE(states[0].defaultTransition == 0u);
	}

	SECTION("Can load multiple states with some transitions")
	{
		std::stringstream input(R"(
[
	{
		"name": "StateStart",
		"transitions": [
			{
				"condition": "cond1",
				"target": "StateLogic"
			}
		],
		"behaviors": [
			"doNothing1",
			"doNothing2"
		],
		"defaultTransition": "StateStart"
	},
	{
		"name": "StateLogic",
		"transitions": [
			{
				"condition": "cond2",
				"target": "StateStart"
			},
			{
				"condition": "cond3",
				"target": "StateStart"
			}
		],
		"behaviors": [
			"logic"
		],
		"defaultTransition": "StateFinish"
	},
	{
		"name": "StateFinish",
		"transitions": [],
		"behaviors": [
			"doNothing"
		],
		"defaultTransition": "StateFinish"
	}
]
)");

		auto states = loader.loadFromStream(input);

		REQUIRE(states.size() == 3u);

		REQUIRE(states[0].transitions.size() == 1u);
		REQUIRE(states[0].transitions[0].first == "cond1");
		REQUIRE(states[0].transitions[0].second == 1);
		REQUIRE(states[0].behaviors.size() == 2u);
		REQUIRE(states[0].behaviors[0] == "doNothing1");
		REQUIRE(states[0].behaviors[1] == "doNothing2");
		REQUIRE(states[0].defaultTransition == 0u);
		REQUIRE(states[0].name == "StateStart");

		REQUIRE(states[1].transitions.size() == 2u);
		REQUIRE(states[1].transitions[0].first == "cond2");
		REQUIRE(states[1].transitions[0].second == 0u);
		REQUIRE(states[1].transitions[1].first == "cond3");
		REQUIRE(states[1].transitions[1].second == 0u);
		REQUIRE(states[1].behaviors[0] == "logic");
		REQUIRE(states[1].defaultTransition == 2u);
		REQUIRE(states[1].name == "StateLogic");

		REQUIRE(states[2].transitions.size() == 0u);
		REQUIRE(states[2].behaviors[0] == "doNothing");
		REQUIRE(states[2].defaultTransition == 2u);
		REQUIRE(states[2].name == "StateFinish");
	}

	SECTION("Throws if nonexistent state is referenced")
	{
		std::stringstream input(R"(
[
	{
		"name": "StateStart",
		"transitions": [
			{
				"condition": "cond1",
				"target": "StateLogic"
			}
		],
		"behaviors": [
			"doNothing1",
			"doNothing2"
		],
		"defaultTransition": "StateStart"
	}
]
)");

		REQUIRE_THROWS([&] ()
		{
			auto states = loader.loadFromStream(input);
		} ());
	}

	SECTION("Throws on invalid JSON")
	{
		std::stringstream input(R"(invalid)");

		REQUIRE_THROWS([&] ()
		{
			auto states = loader.loadFromStream(input);
		} ());
	}
}