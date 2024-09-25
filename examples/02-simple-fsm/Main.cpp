#include <CsvBlackboard.hpp>
#include <CsvFunctions.hpp>
#include <StdoutLogger.hpp>
#include <fsm/Builder.hpp>
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

    auto&& logger = StdoutLogger();
    // NOTE: To enable logging, uncomment the next line
    // machine.setLogger(logger);

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