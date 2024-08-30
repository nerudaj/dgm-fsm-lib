#include "Blackboard.hpp"
#include <DGM/classes/Helper.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[Helper]")
{
    using namespace dgm::fsm::detail;

    SECTION("compileDestination")
    {
        auto&& index = StateIndex();
        index.addNameToIndex("a");
        index.addNameToIndex("b");

        SECTION("When no destination is present")
        {
            REQUIRE(compileTransition(TransitionContext {}, index).isEmpty());
        }

        SECTION("When only one destination is present")
        {
            auto&& dest =
                compileTransition(TransitionContext { .primary = "a" }, index);

            REQUIRE(dest.getSize() == 1u);
            REQUIRE(dest[0] == 0u);
        }

        SECTION("When both destinations are present")
        {
            auto&& dest = compileTransition(
                TransitionContext { .primary = "a", .secondary = "b" }, index);

            REQUIRE(dest.getSize() == 2u);
            REQUIRE(dest[0] == 0u);
            REQUIRE(dest[1] == 1u);
        }

        SECTION("Throws if only second destination is present")
        {
            REQUIRE_THROWS(compileTransition(
                TransitionContext { .secondary = "b" }, index));
        }
    }

    SECTION("createStateIndexFromBuilderContext")
    {
        auto&& index =
            createStateIndexFromBuilderContext(BuilderContext<Blackboard> {
                .machines = {
                    { "machine1",
                      MachineBuilderContext<Blackboard> {
                          .states = { { "Start", {} }, { "End", {} } } } },
                    { "machine2",
                      MachineBuilderContext<Blackboard> {
                          .states = { { "Start", {} }, { "End", {} } } } } } });

        auto&& indexedNames = index.getIndexedStateNames();

        REQUIRE(indexedNames.size() == 4u);
        REQUIRE(indexedNames[0] == "machine1:End");
        REQUIRE(indexedNames[1] == "machine1:Start");
        REQUIRE(indexedNames[2] == "machine2:End");
        REQUIRE(indexedNames[3] == "machine2:Start");
    }

    SECTION("Full state name")
    {
        SECTION("Throws on invalid full name")
        {
            REQUIRE_THROWS(getMachineAndStateNameFromFullName("abc"));
        }

        SECTION("Properly constructs and deconstructs")
        {
            auto&& machineName = "machine";
            auto&& stateName = "state";
            auto&& fullName = createFullStateName(machineName, stateName);
            auto&& split = getMachineAndStateNameFromFullName(fullName);

            REQUIRE(machineName == split.first);
            REQUIRE(stateName == split.second);
        }
    }
}