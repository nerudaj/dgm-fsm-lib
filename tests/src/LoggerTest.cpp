#include "Blackboard.hpp"
#include "TestableLogger.hpp"
#include <catch2/catch_all.hpp>

TEST_CASE("[Logger]")
{
    auto&& loggerInstance = TestableLogger();
    fsm::LoggerInterface& logger = loggerInstance;

    SECTION("Can log loggable blackboard")
    {
        logger.log("", Blackboard {}, "", "");
        REQUIRE(loggerInstance.lastLogBlackboard == "Blackboard: [ charIdx: 0; wordStartIdx: 0; |csv| = 1; |csv.back()| = 0 ]");
    }

    SECTION("Can log unloggable blackboard")
    {
        logger.log("", NonLoggableBlackboard {}, "", "");
        REQUIRE(loggerInstance.lastLogBlackboard.starts_with(
            "Blackboard at address 0x"));
    }
}