#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"
#include "data.h"

using namespace Pipe;

// =======================================================================================
// functor classes for test 3
// =======================================================================================

// one to many transform using a generator pattern
struct functor1
{
    int count;
    functor1(): count(10) {}

    // generate multiple output, return done == 'true' with last output
    bool operator()(pipe_data_t &data)
    {
        pipe_data_t::print("1",data);

        data.lla.m_lat += 1.0;
        data.lla.m_lon += 2.0;
        data.lla.m_alt  = 99.0;
        count--;
        return (count == 0);
    }
};

// one to one transform
struct functor2
{
    bool operator()(pipe_data_t &data)
    {
        pipe_data_t::print("2",data);

        data.u32 = 2;
        return true;
    }
};

// one to one transform
struct functor3
{
    bool operator()(pipe_data_t &data)
    {
        pipe_data_t::print("3",data);
        data.cls = 3;
        return true;
    }
};

// one to one transform
struct functor4
{
    bool operator()(pipe_data_t &data)
    {
        pipe_data_t::print("4",data);
        return true;
    }
};

void test3()
{
    printf("TEST composed pipeline with functor objects\n");
    printf("uses composition rather than inheritance\n");

    // create the piped functions
    auto a = pipe<pipe_data_t>(functor1());
    auto b = pipe<pipe_data_t>(functor2());
    auto c = pipe<pipe_data_t>(functor3());
    auto d = pipe<pipe_data_t>(functor4());

    // compose them into an (a -> b -> c -> d) pipeline
    auto x = compose<pipe_data_t>(a,b,c,d);

    // execute the pipeline
    pipe_data_t lla;
    x->exec(lla);
}
