#include <catch2/catch_all.hpp>
#include <fsm/detail/Compiler.hpp>

TEST_CASE("[Compiler]")
{
    using namespace fsm::detail;

    SECTION("compileDestination")
    {
        auto&& index = StateIndex();
        index.addNameToIndex("a");
        index.addNameToIndex("b");

        SECTION("When no destination is present")
        {
            REQUIRE(Compiler::compileTransition(TransitionContext {}, index)
                        .isEmpty());
        }

        SECTION("When only one destination is present")
        {
            auto&& dest = Compiler::compileTransition(
                TransitionContext { .primary = "a" }, index);

            REQUIRE(dest.getSize() == 1u);
            REQUIRE(dest[0] == 0u);
        }

        SECTION("When both destinations are present")
        {
            auto&& dest = Compiler::compileTransition(
                TransitionContext { .primary = "a", .secondary = "b" }, index);

            REQUIRE(dest.getSize() == 2u);
            REQUIRE(dest[0] == 0u);
            REQUIRE(dest[1] == 1u);
        }

        SECTION("Throws if only second destination is present")
        {
            REQUIRE_THROWS(Compiler::compileTransition(
                TransitionContext { .secondary = "b" }, index));
        }
    }
}