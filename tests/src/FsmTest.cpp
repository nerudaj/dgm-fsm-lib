#include <DGM/classes/Builder.hpp>
#include <DGM/classes/Fsm.hpp>
#include <catch2/catch_all.hpp>

struct Blackboard
{
    std::string data = "";
    size_t charIdx = 0;
};

bool isEscapeChar(const Blackboard& bb)
{
    return bb.data[bb.charIdx] == '"';
}

bool isSeparatorChar(const Blackboard& bb)
{
    return bb.data[bb.charIdx] == ',';
}

bool isNewlineChar(const Blackboard& bb)
{
    return bb.data[bb.charIdx] == '\n';
}

void advanceChar(Blackboard& bb)
{
    ++bb.charIdx;
}

void nothing(Blackboard& bb) {}

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
            .when(isEscapeChar).goToMachineAndThenToState("Shifter", "CharAfterEscaped")
            .orWhen(isSeparatorChar).goToState("SeparatorChar")
            .orWhen(isNewlineChar).goToState("NewlineChar")
            .otherwiseExec(advanceChar).andLoop()
        .withState("CharAfterEscaped")
            .when(isEscapeChar).goToMachineAndThenToState("Shifter", "CharAfterSecondEscaped")
            .otherwiseExec(advanceChar).andLoop()
        .withState("CharAfterSecondEscaped")
            .when(isEscapeChar).goToState("EscapedChar") // doubly escaped, returning to regularly quoted
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