#include <StdoutLogger.hpp>
#include <fsm/Types.hpp>
#include <print>

struct Blackboard : fsm::BlackboardBase
{
    bool someValue = false;
};

template<class CharT>
struct std::formatter<Blackboard, CharT>
{
    template<class ParseContext>
    constexpr auto parse(ParseContext& ctx)
    {
        // You can process format options here
        // Look at std documentation for more info
        return ctx.begin();
    }

    template<class FormatContext>
    constexpr auto format(const Blackboard& bb, FormatContext& ctx) const
    {
        return std::format_to(
            ctx.out(), "Blackboard: [ someValue: {} ]", bb.someValue);
    }
};

// This one is not loggable
struct Blackboard2 : fsm::BlackboardBase
{
};

int main()

{
    // fsm::LoggerInterface uses std::format
    // so if this line works then fsm::LoggerInterface can
    // log it too
    std::println("Blackboard: {}", Blackboard {});

    auto&& logger = StdoutLogger();

    // Thus, this will print a pretty log of a loggable blackboard
    logger.log("", Blackboard {}, "", "");

    // This one compiles and runs, but provides only blackboard address
    logger.log("", Blackboard2 {}, "", "");
}
