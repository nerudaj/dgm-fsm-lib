#include <DGM/classes/StateIndex.hpp>
#include <catch2/catch_all.hpp>

TEST_CASE("[StateIndex]")
{
    auto&& index = dgm::fsm::detail::StateIndex();
    SECTION("Throws exception when trying to read id not inserted to index")
    {
        REQUIRE_THROWS(index.getStateIndex("not-inserted"));
    }

    SECTION(
        "Throws exception when trying to insert state into index multiple "
        "times")
    {
        index.addNameToIndex("name");
        REQUIRE_THROWS(index.addNameToIndex("name"));
    }

    SECTION("Returns indices according to order of insertion")
    {
        index.addNameToIndex("helo");
        index.addNameToIndex("ehlo");
        index.addNameToIndex("name");

        SECTION("getStateIndex")
        {
            REQUIRE(index.getStateIndex("helo") == 0u);
            REQUIRE(index.getStateIndex("ehlo") == 1u);
            REQUIRE(index.getStateIndex("name") == 2u);
        }

        SECTION("getImdexedStateNames")
        {
            auto&& names = index.getIndexedStateNames();

            REQUIRE(names.size() == 3u);
            REQUIRE(names[0] == "helo");
            REQUIRE(names[1] == "ehlo");
            REQUIRE(names[2] == "name");
        }
    }
}