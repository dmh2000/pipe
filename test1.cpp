
#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"
#include "data.h"


using namespace Pipe;

bool f(int &i)
{
    printf("f : %d\n",i);
    i++;
    return true;
}

bool g(int &i)
{
    printf("g : %d\n",i);
    i++;
    return true;
}

bool h(int &i)
{
    printf("h : %d\n",i);
    return true;
}


void test1()
{
    printf("TEST composed pipeline with all plain old functions\n");

    // lambdas
    auto a = pipe<int>(f);
    auto b = pipe<int>(g);
    auto c = pipe<int>(h);


    // compose them into an a -> b -> c pipeline
    auto x = compose<int>(a,b,c);

    // execute the pipeline
    x->exec(1);
}
