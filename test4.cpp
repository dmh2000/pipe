#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"
#include "data.h"


using namespace Pipe;

struct F4: public pipe_t<int>
{
    F4()
    {
    }

    virtual ~F4()
    {
        printf("~F4\n");
    }

    void exec(const int &count)  override
    {
        printf("F4 : %d\n",count);
        // create multiple mutable copies
        for(int i=0;i<count;++i) {
            // create mutable copy
            // call next
            next(i);
        }
    }
};

// one to one transform
struct functor5
{
    bool operator()(int i)
    {
        printf("functor 5 : %d\n",i);
        return true;
    }
};

void test4()
{
    printf("TEST composed pipeline with mix of objects,functors and lambda functions\n");
    printf("objects and/or functors when state is needed, functors/lambdas when code can be pure\n");

    // object
    std::shared_ptr<F4>     r = std::make_shared<F4>();

    // lambdas
    auto a = pipe<int>([](int &i) -> bool { printf("a: %d\n",i); i++; return true; });
    auto b = pipe<int>([](int &i) -> bool { printf("b: %d\n",i); i++; return true; });
    auto c = pipe<int>([](int &i) -> bool { printf("c: %d\n",i); i++; return true; });

    // functor
    auto d = pipe<int>(functor5());

    // compose them into an a -> b -> c pipeline
    auto x = compose<int>(r,a,b,c,d);

    // execute the pipeline
    x->exec(10);
}
