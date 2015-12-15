#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"
#include "data.h"


using namespace Pipe;

void test4()
{
    printf("TEST composed pipeline with lambda functions\n");
    printf("uses composition rather than inheritance\n");

    // create the piped functions
    auto a = pipe<int>([](int &i) -> bool { printf("a: %d\n",i); i++; return true; });
    auto b = pipe<int>([](int &i) -> bool { printf("b: %d\n",i); i++; return true; });
    auto c = pipe<int>([](int &i) -> bool { printf("c: %d\n",i); i++; return true; });

    // compose them into an a -> b -> c pipeline
    auto x = compose<int>(a,b,c);

    // execute the pipeline
    x->exec(1);
}