#include "helloworld.h"
#include <stdio.h>

const char *say_hello()
{
    return "Hello, World!";
}

int main()
{
    puts(say_hello());
    return 0;
}
