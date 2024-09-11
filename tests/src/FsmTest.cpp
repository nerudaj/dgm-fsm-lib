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
    auto&& fsm = dgm::fsm::Builder<Blackboard>()
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