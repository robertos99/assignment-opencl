#define CATCH_CONFIG_MAIN
#include <catch2/catch.hpp>

TEST_CASE("say_hello returns the correct string", "[hello_world]") {
  const char *expected = "Hello, World!";
  const char *actual = "Hello, World!";

  REQUIRE(std::string(actual) == std::string(expected));
}