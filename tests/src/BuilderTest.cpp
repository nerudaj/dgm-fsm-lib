#include "Blackboard.hpp"
#include "CsvParser.hpp"
#include <DGM/classes/Builder.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[Builder]")
{
    SECTION("Cannot call __main__ machine from submachine")
    {
        // clang-format off
        REQUIRE_THROWS(fsm::Builder<Blackboard>()
            .withNoErrorMachine()
            .withSubmachine("A")
                .withEntryState("S")
                    .exec(nothing)
                        .andGoToMachine("__main__").thenGoToState("S")
                .done()
            .withMainMachine()
                .withEntryState("S")
                    .exec(nothing).andLoop()
                .done()
            .build());
        // clang-format on
    }

    /*
    .withErrorMachine() / withNoErrorMachine()
        .useGlobalEntryCondition()/.noGlobalEntryCondition()
        .withEntryState()
            .when().goToState() / restart()
            .orWhen()
            .otherwiseExec().andLoop() / andGoToState() / andRestart()
    */
}