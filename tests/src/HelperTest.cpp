#include "Blackboard.hpp"
#include <catch2/catch_all.hpp>
#include <fsm/detail/Helper.hpp>

TEST_CASE("[Helper]")
{
    using namespace fsm::detail;

    SECTION("createStateIndexFromBuilderContext")
    {
        SECTION("Without error states")
        {
            auto&& index =
                createStateIndexFromBuilderContext(BuilderContext<Blackboard> {
                    .machines = {
                        { "__main__",
                          MachineBuilderContext<Blackboard> {
                              .entryState = "Start",
                              .states = { { "Start", {} }, { "End", {} } } } },
                        { "machine2",
                          MachineBuilderContext<Blackboard> {
                              .states = { { "Start", {} },
                                          { "End", {} } } } } } });

            auto&& indexedNames = index.getIndexedStateNames();

            REQUIRE(indexedNames.size() == 4u);
            REQUIRE(indexedNames[0] == "__main__:Start");
            REQUIRE(indexedNames[1] == "__main__:End");
            REQUIRE(indexedNames[2] == "machine2:End");
            REQUIRE(indexedNames[3] == "machine2:Start");
        }

        SECTION("With error states")
        {
            auto&& index =
                createStateIndexFromBuilderContext(BuilderContext<Blackboard> {
                    .machines = {
                        { "__main__",
                          MachineBuilderContext<Blackboard> {
                              .entryState = "Start",
                              .states = { { "Start", {} }, { "End", {} } } } },
                        { "machine2",
                          MachineBuilderContext<Blackboard> {
                              .states = { { "Start", {} }, { "End", {} } } } },
                        { "__error__",
                          MachineBuilderContext<Blackboard> {
                              .states = { { "Start", {} }, { "End", {} } } } },
                    } });

            auto&& indexedNames = index.getIndexedStateNames();

            REQUIRE(indexedNames.size() == 6u);
            REQUIRE(indexedNames[0] == "__main__:Start");
            REQUIRE(indexedNames[1] == "__error__:End");
            REQUIRE(indexedNames[2] == "__error__:Start");
            REQUIRE(indexedNames[3] == "__main__:End");
            REQUIRE(indexedNames[4] == "machine2:End");
            REQUIRE(indexedNames[5] == "machine2:Start");
        }
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