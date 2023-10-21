#include "unity.h"
#include "helloworld.h"

void setUp(void)
{
    // This function runs before EACH TEST
}

void tearDown(void)
{
    // This function runs after EACH TEST
}

void test_say_hello_returns_correct_string(void)
{
    const char *expected = "Hello, World!";
    const char *actual = say_hello();
    TEST_ASSERT_EQUAL_STRING(expected, actual);
}

int main(void)
{
    UNITY_BEGIN();
    RUN_TEST(test_say_hello_returns_correct_string);
    return UNITY_END();
}
