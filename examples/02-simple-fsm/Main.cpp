#include <CsvBlackboard.hpp>
#include <CsvFunctions.hpp>
#include <fsm/Builder.hpp>
#include <fsm/logging/CsvLogger.hpp>
#include <print>

int main()
{
    auto doNothing = [](CsvBlackboard&) {};

    // clang-format off
    auto&& machine = fsm::Builder<CsvBlackboard>()
        .withErrorMachine()
        .noGlobalEntryCondition()
            .withEntryState("Start")
                .exec(doNothing).andLoop()
            .done()
        .withMainMachine()
            .withEntryState("Start")
                .when(isEof).error()
                .orWhen(isSeparator).goToState("HandleSeparator")
                .orWhen(isNewline).goToState("HandleNewline")
                .otherwiseExec(advanceChar).andLoop()
            .withState("HandleSeparator")
                .exec(handleSeparator).andGoToState("Start")
            .withState("HandleNewline")
                .exec([] (CsvBlackboard& bb) {
                        handleSeparator(bb);
                        handleNewline(bb);
                    }).andGoToState("PostNewline")
            .withState("PostNewline")
                .when(isEof).finish()
                .otherwiseExec(doNothing).andGoToState("Start")
            .done()
        .build();
    // clang-format on

    // NOTE: To enable logging, uncomment the next line
    /*
    auto&& logger = fsm::CsvLogger();
    machine.setLogger(logger);
    */

    auto&& runMachine = [&machine](CsvBlackboard&& blackboard)
    {
        while (!machine.isErrored(blackboard)
               && !machine.isFinished(blackboard))
        {
            machine.tick(blackboard);
        }

        std::println(
            "Fsm\n  finished: {}\n  errored: {}\n",
            machine.isFinished(blackboard),
            machine.isErrored(blackboard));
    };

    // This one should succeed
    runMachine(CsvBlackboard {
        .data = "abc,bcd,cde\nabc,bcd,cde\n",
    });

    // This one should fail
    runMachine(CsvBlackboard {
        .data = "abc,bcd,cde\nabc,b",
    });
}