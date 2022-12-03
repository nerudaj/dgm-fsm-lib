#include <catch.hpp>
#include <FsmFactory.hpp>

using dgm::fsm::Factory;

struct Blackboard {};

class LoaderMock final : public dgm::fsm::LoaderInterface
{
public:
	dgm::fsm::loader::StateMap states;

	[[nodiscard]]
	virtual dgm::fsm::loader::StateMap loadFromFile(const std::string&) const override
	{
		return states;
	}
};

TEST_CASE("[FsmFactory]", "[FsmFactory]")
{
	auto loader = LoaderMock();
	auto factory = Factory<Blackboard>(loader);
	auto blackboard = Blackboard();

	unsigned cond1_callCnt = 0;
	unsigned cond2_callCnt = 0;
	unsigned cond3_callCnt = 0;

	unsigned logic1_callCnt = 0;
	unsigned logic2_callCnt = 0;
	unsigned logic3_callCnt = 0;

	factory.registerPredicate("cond1", [&] (const Blackboard&) -> bool
	{
		cond1_callCnt++;
		return false;
	});
	factory.registerPredicate("cond2", [&] (const Blackboard&) -> bool
	{
		cond2_callCnt++;
		return false;
	});
	factory.registerPredicate("cond3", [&] (const Blackboard&) -> bool
	{
		cond3_callCnt++;
		return false;
	});

	factory.registerLogic("logic1", [&] (Blackboard&) { logic1_callCnt++; });
	factory.registerLogic("logic2", [&] (Blackboard&) { logic2_callCnt++; });
	factory.registerLogic("logic3", [&] (Blackboard&) { logic3_callCnt++; });

	SECTION("Can construct minimal FSM with "
			"just one state and no transitions")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.behaviors = { "logic1" },
				.defaultTransition = 0
			}}
		};

		dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		fsm.update(blackboard);

		REQUIRE(logic1_callCnt == 1u);
	}

	SECTION("Can construct minimal FSM with some transitions")
	{
		loader.states = {
			{0, {
				.transitions = {
					{ "cond1", 1 },
					{ "cond2", 1 }
				},
				.behaviors = { "logic1" },
				.defaultTransition = 1
			}},
			{1, {
				.transitions = {},
				.behaviors = { "logic2" },
				.defaultTransition = 0
			}}
		};

		dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		fsm.update(blackboard);
		fsm.update(blackboard);

		REQUIRE(cond1_callCnt == 1u);
		REQUIRE(cond2_callCnt == 1u);
		REQUIRE(logic1_callCnt == 1u);
		REQUIRE(logic2_callCnt == 1u);
	}

	SECTION("Can merge multiple logics")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.behaviors = {
					"logic1",
					"logic2",
					"logic3"
				},
				.defaultTransition = 0
			}}
		};

		dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		fsm.update(blackboard);

		REQUIRE(logic1_callCnt == 1u);
		REQUIRE(logic2_callCnt == 1u);
		REQUIRE(logic3_callCnt == 1u);
	}

	SECTION("Can deal with empty behavior")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.behaviors = {},
				.defaultTransition = 0
			}}
		};

		REQUIRE_NOTHROW([&] ()
		{
			dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		} ());
	}

	SECTION("Properly sets log helpers for state names")
	{
		loader.states = {
			{ 0, {
				.name = "Start",
				.transitions = {},
				.behaviors = {},
				.defaultTransition = 1
			}},
			{ 1, {
				.name = "End",
				.transitions = {},
				.behaviors = {},
				.defaultTransition = 0
			}}
		};

		dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");

		std::stringstream log;
		fsm.setLogging(true, log);
		fsm.update(blackboard);
		fsm.update(blackboard);

		REQUIRE(log.str() == R"(FSM::update(State = Start):
  behavior executed, jumping to End
FSM::update(State = End):
  behavior executed, jumping to Start
)");
	}

	SECTION("Throws if referencing not-registered logic")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.behaviors = { "notRegisteredLogic" },
				.defaultTransition = 0
			}}
		};

		REQUIRE_THROWS([&] ()
		{
			dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		} ());
	}

	SECTION("Throws if referencing not-registered predicate")
	{
		loader.states = {
			{0, {
				.transitions = { { "notRegisteredLogic", 0 } },
				.behaviors = { "logic1" },
				.defaultTransition = 0
			}}
		};

		REQUIRE_THROWS([&] ()
		{
			dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		} ());
	}

	SECTION("Throws if out-of-bound state is referenced")
	{
		loader.states = {
			{0, {
				.transitions = {},
				.behaviors = { "logic1" },
				.defaultTransition = 1
			}}
		};

		REQUIRE_THROWS([&] ()
		{
			dgm::fsm::Fsm<Blackboard, unsigned> fsm = factory.loadFromFile("unused");
		} ());
	}

	SECTION("Provides annotations")
	{
		const std::string refAnnotations = R"({
    "predicates": [cond1, cond2, cond3]
    "behaviors": [logic1, logic2, logic3]
})";

		REQUIRE(factory.getAnnotations() == refAnnotations);
	}
}