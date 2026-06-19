#include <catch2/catch_test_macros.hpp>
#include "Localization.hpp"
#include <string>

TEST_CASE("tr returns English for lang=0", "[localization]")
{
    REQUIRE(std::string(game::tr("Hello", "Привет", 0)) == "Hello");
}

TEST_CASE("tr returns Russian for lang=1", "[localization]")
{
    REQUIRE(std::string(game::tr("Hello", "Привет", 1)) == "Привет");
}

TEST_CASE("tr returns English for unknown language code", "[localization]")
{
    REQUIRE(std::string(game::tr("Settings", "Настройки", 42)) == "Settings");
}

TEST_CASE("tr handles empty strings", "[localization]")
{
    REQUIRE(std::string(game::tr("", "", 0)) == "");
    REQUIRE(std::string(game::tr("", "", 1)) == "");
}
