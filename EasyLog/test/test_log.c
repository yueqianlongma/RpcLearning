#include"log.h"
int main()
{
    trace("test %s\n", "this is main");
    trace("test %s: %d\n", "his", 5);
    trace("\n");
    errsys("\n");
}