#include "Blackboard.hpp"
#include "CsvParser.hpp"
#include <DGM/classes/Builder.hpp>
#include <DGM/classes/Fsm.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[FSM]")
{
    auto handleSeparator = [](Blackboard& bb)
    {
        storeWord(bb);
        advanceChar(bb);
    };

    auto handleNewline = [](Blackboard& bb)
    {
        storeWord(bb);
        startLine(bb);
        advanceChar(bb);
    };

    // clang-format off
    auto&& fsm = fsm::Builder<Blackboard>()
        .withNoErrorMachine()
        .withSubmachine("Shifter")
            .withEntryState("Shift")
                .exec(advanceChar).andFinish()
            .done()
        .withMainMachine()
            .withEntryState("Start")
                .when(isEof).goToState("Error")
                .orWhen(isEscapeChar)
                    .goToMachine("Shifter").thenGoToState("Escaped")
                .orWhen(isSeparatorChar).goToState("SeparatorChar")
                .orWhen(isNewlineChar).goToState("NewlineChar")
                .otherwiseExec(advanceChar).andLoop()
            .withState("Escaped")
                .when(isEof).goToState("Error")
                .orWhen(isEscapeChar).goToMachine("Shifter").thenGoToState("CharAfterSecondEscaped")
                .otherwiseExec(advanceChar).andLoop()
            .withState("CharAfterSecondEscaped")
                // doubly escaped, returning to regularly quoted
                .when(isEof).goToState("Error")
                .orWhen(isEscapeChar).goToMachine("Shifter").thenGoToState("Escaped") 
                .orWhen(isSeparatorChar).goToState("SeparatorChar")
                .orWhen(isNewlineChar).goToState("NewlineChar")
                .otherwiseExec(nothing).andGoToState("Error")
            .withState("SeparatorChar")
                .exec(handleSeparator).andGoToState("Start")
            .withState("NewlineChar")
                .exec(handleNewline).andGoToState("CheckEof")
            .withState("CheckEof")
                .when(isEof).finish()
                .otherwiseExec(nothing).andGoToState("Start")
            .withState("Error")
                .exec(nothing).andLoop()
            .done()
        .build();
    // clang-format on

    auto&& bb = Blackboard {
        .data = "abc,cde,efg\n\"abc\",\"cd\"\"e\",efg\n",
    };

    fsm.start(bb);

    while (!fsm.isFinished(bb))
    {
        fsm.tick(bb);
    }

    // TODO: tick until finished or errored
}

TEST_CASE("[FSM2]")
{
    Blackboard bb;
    SECTION("Machine can finish")
    {
        // clang-format off
        auto&& machine = fsm::Builder<Blackboard>()
            .withNoErrorMachine()
            .withMainMachine()
                .withEntryState("Start")
                    .exec(nothing).andFinish()
                .done()
            .build();
        // clang-format on

        REQUIRE(machine.isFinished(
            bb)); // considered finished before being initialized
        machine.start(bb);
        machine.tick(bb);
        REQUIRE(machine.isFinished(bb));
    }

    SECTION("Can restart machine from error state")
    {
        // clang-format off
        auto&& machine = fsm::Builder<Blackboard>()
            .withErrorMachine()
                .noGlobalEntryCondition()
                .withEntryState("Start")
                    .exec(nothing).andRestart()
                .done()
            .withMainMachine()
                .withEntryState("Start")
                    .when(alwaysTrue).error()
                    .otherwiseExec(nothing).andLoop()
                .done()
            .build();
        // clang-format on

        machine.start(bb);

        REQUIRE(bb.__stateIdxs.back() == 1); // "__main__:Start"
        machine.tick(bb);
        REQUIRE(bb.__stateIdxs.back() == 0); // "__error__:Start"
        machine.tick(bb);
        REQUIRE(bb.__stateIdxs.back() == 1); // "__main__:Start"
        REQUIRE(bb.__stateIdxs.size() == 1);
    }

    SECTION("Can enter and return from a submachine")
    {
        // clang-format off
        auto&& machine = fsm::Builder<Blackboard>()
            .withNoErrorMachine()
            .withSubmachine("Sub")
                .withEntryState("A")
                    .exec(nothing).andFinish()
                .done()
            .withMainMachine()
                .withEntryState("A")
                    .exec(nothing).andGoToMachine("Sub").thenGoToState("B")
                .withState("B")
                    .exec(nothing).andLoop()
                .done()
            .build();
        // clang-format on

        // Ordering:
        // 0: Sub:A
        // 1: __main__:A
        // 2: __main__:B

        machine.start(bb);
        REQUIRE(bb.__stateIdxs.back() == 1);
        REQUIRE(bb.__stateIdxs.size() == 1);
        machine.tick(bb);
        REQUIRE(bb.__stateIdxs.back() == 0);
        REQUIRE(bb.__stateIdxs.size() == 2);
        machine.tick(bb);
        REQUIRE(bb.__stateIdxs.back() == 2);
        REQUIRE(bb.__stateIdxs.size() == 1);
    }

    SECTION("Error from submachines clears state stack")
    {
        // clang-format off
        auto&& machine = fsm::Builder<Blackboard>()
            .withErrorMachine()
                .noGlobalEntryCondition()
                .withEntryState("A")
                   .exec(nothing).andLoop()
                .done()
            .withSubmachine("1")
                .withEntryState("A")
                    .when(alwaysTrue).error()
                    .otherwiseExec(nothing).andLoop()
                .done()
            .withSubmachine("2")
                .withEntryState("A")
                    .exec(nothing).andGoToMachine("1").thenGoToState("B")
                .withState("B")
                    .exec(nothing).andFinish()
                .done()
            .withMainMachine()
                .withEntryState("A")
                    .exec(nothing).andGoToMachine("2").thenGoToState("B")
                .withState("B")
                    .exec(nothing).andLoop()
                .done()
            .build();
        // clang-format on

        machine.start(bb);
        machine.tick(bb); // going to 2
        REQUIRE(bb.__stateIdxs.size() == 2);
        machine.tick(bb); // going to 1
        REQUIRE(bb.__stateIdxs.size() == 3);
        machine.tick(bb); // going to error
        REQUIRE(bb.__stateIdxs.size() == 1);
    }

    SECTION(
        "Global error condition is taken into account when outside of error "
        "machine")
    {
        // clang-format off
        auto&& machine = fsm::Builder<Blackboard>()
            .withErrorMachine()
            .useGlobalEntryCondition(isExclamationMark)
                .withEntryState("A")
                    .exec(nothing).andGoToState("B")
                .withState("B")
                    .exec(nothing).andLoop()
                .done()
            .withSubmachine("HandleEscaped")
                .withEntryState("A")
                    .exec(advanceChar).andGoToState("MainLoop")
                .withState("MainLoop")
                    .when(isEscapeChar).finish()
                    .otherwiseExec(advanceChar).andLoop()
                .done()
            .withMainMachine()
                .withEntryState("A")
                    .when(isSeparatorChar).goToState("HandleSeparator")
                    .orWhen(isEscapeChar).goToMachine("HandleEscaped").thenGoToState("A")
                    .otherwiseExec(advanceChar).andLoop()
                .withState("HandleSeparator")
                    .exec([] (Blackboard& bb) { storeWord(bb); advanceChar(bb); }).andGoToState("PostSeparatorHandle")
                .withState("PostSeparatorHandle")
                    .exec(nothing).andGoToState("A")
                .done()
            .build();
        // clang-format on

        // All these cases are supposed to error out during 4th tick
        SECTION("Case A.I - Main machine")
        {
            bb.data = "abc!";
        }

        SECTION("Case A.II - Main machine, not entry state")
        {
            bb.data = "a,!";
        }

        SECTION("Case B - Sub machine")
        {
            bb.data = "\"a!";
        }

        machine.start(bb);
        machine.tick(bb);
        machine.tick(bb);
        machine.tick(bb);

        REQUIRE(!machine.isErrored(bb));
        machine.tick(bb);
        REQUIRE(machine.isErrored(bb));
    }
}