#include "Blackboard.hpp"> "
#include "CsvParser.hpp"
#include <DGM/classes/Builder.hpp>
#include <DGM/classes/Fsm.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[FSM]")
{
    // clang-format off
    auto&& fsm = dgm::fsm::Builder<Blackboard>()
        .withNoGlobalErrorCondition()
        .withSubmachine("Shifter")
            .withEntryState("Shift")
                .exec(advanceChar).andFinish()
            .done()
        .withMainMachine()
            .withEntryState("Start")
                .when(isEscapeChar).goToMachineAndThenToState("Shifter", "Escaped")
                .orWhen(isSeparatorChar).goToState("SeparatorChar")
                .orWhen(isNewlineChar).goToState("NewlineChar")
                .otherwiseExec(advanceChar).andLoop()
            .withState("Escaped")
                .when(isEscapeChar).goToMachineAndThenToState("Shifter", "CharAfterSecondEscaped")
                .otherwiseExec(advanceChar).andLoop()
            .withState("CharAfterSecondEscaped")
                // doubly escaped, returning to regularly quoted
                .when(isEscapeChar).goToMachineAndThenToState("Shifter", "Escaped") 
                .orWhen(isSeparatorChar).goToState("SeparatorChar")
                .orWhen(isNewlineChar).goToState("NewlineChar")
                .otherwiseExec(nothing).andGoToState("Error")
            .withState("SeparatorChar")
                .exec([](Blackboard& bb) { storeWord(bb); advanceChar(bb); }).andGoToState("Start")
            .withState("NewlineChar")
                .exec([](Blackboard& bb) { storeWord(bb); startLine(bb); advanceChar(bb); }).andGoToState("Start")
            .withState("Error")
                .exec(nothing).andLoop()
            .done()
        .build();
    // clang-format on
}