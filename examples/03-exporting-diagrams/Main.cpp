#include <CsvBlackboard.hpp>
#include <CsvFunctions.hpp>
#include <fsm/Builder.hpp>
#include <fsm/exports/MermaidExporter.hpp>
#include <print>
#include <sstream>

int main()
{
    auto doNothing = [](CsvBlackboard&) {};

    auto fstream = std::ostringstream();

    // clang-format off
    std::ignore = fsm::Builder<CsvBlackboard>()
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
        // NOTE: Export here to stdout
        .exportDiagram(fsm::MermaidExporter(std::cout))
        // or to any ostream handle
        .exportDiagram(fsm::MermaidExporter(fstream))
        .build();
    // clang-format on

    // Paste resulting diagram to https://mermaid.live/
}