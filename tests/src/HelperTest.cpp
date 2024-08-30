#include "Blackboard.hpp"
#include <DGM/classes/Helper.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[Helper]")
{
    using namespace dgm::fsm::detail;

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