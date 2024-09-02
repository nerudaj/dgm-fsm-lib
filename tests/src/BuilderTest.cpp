#include "Blackboard.hpp"
#include "CsvParser.hpp"
#include <DGM/classes/Builder.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[Builder]")
{
    SECTION("Cannot call __main__ machine from submachine")
    {
        // clang-format off
        REQUIRE_THROWS(dgm::fsm::Builder<Blackboard>()
            .withNoGlobalErrorCondition()
            .withSubmachine("A")
                .withEntryState("S")
                    .exec(nothing).andGoToMachineThenReturnToState("__main__", "S")
                .done()
            .withMainMachine()
                .withEntryState("S")
                    .exec(nothing).andLoop()
                .done()
            .build());
        // clang-format on
    }
}