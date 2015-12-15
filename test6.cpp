#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"
#include "data.h"


using namespace Pipe;

struct Writer6: public pipe_t<pipe_data_t>
{
    Writer6() {}

    virtual ~Writer6()
    {
        printf("~Writer6\n");
    }

    void exec(const pipe_data_t &data)  override
        // void exec(const pipe_data_t &data)  override
    {
        // print the input data
        pipe_data_t::print("W",data);

        // call the next
        next(data);
    }
};

struct Classifier6: public pipe_t<pipe_data_t>
{
    Classifier6() {}

    virtual ~Classifier6()
    {
        printf("~Classifier6\n");
    }

    void exec(const pipe_data_t &data)  override
    {
        // print the input data
        pipe_data_t::print("C",data);

        // create a mutable copy
        pipe_data_t x = data;

        // update the copy
        x.cls = 1;

        // call the next
        next(x);
    }
};

struct Transform6: public pipe_t<pipe_data_t>
{
    Transform6() {}

    virtual ~Transform6()
    {
        printf("~Transform6\n");
    }

    void exec(const pipe_data_t &data)  override
    {
        // print the input data
        pipe_data_t::print("T",data);

        // create a mutable copy
        pipe_data_t x = data;

        // update the copy
        uint32_t    u32 = data.u32;
        x.lla.m_lon = static_cast<double>(u32) * 2.0;
        x.lla.m_lat = static_cast<double>(u32) * 2.0;
        x.lla.m_alt = static_cast<double>(u32) * 2.0;

        // call the next
        next(x);
    }
};

struct Reader6: public pipe_t<pipe_data_t>
{
    Reader6() {}

    virtual ~Reader6()
    {
        printf("~Reader6\n");
    }

    // void exec(const pipe_data_t &data)  override
    void exec(const pipe_data_t &data) override
    {
        // print the input data

        // create multiple mutable copies
        for(int i=0;i<10;++i) {
            pipe_data_t::print("R",data);
            // create mutable copy
            pipe_data_t x = data;
            // update it
            x.u32 = i;
            // call next
            next(x);
        }
    }
};

// null deleter for shared pointer to static objects
void test6()
{
    printf("TEST explicit pipe between statically allocated classes with an object as the piped data (pipe_data_t = latitude/longitude/altitude)\n");
    printf("only difference is that the shared_ptr instances need a null deleter\n");
    printf("uses inheritance of the pipe_t object\n");
    Reader6     r;
    Transform6  t;
    Classifier6 c;
    Writer6     w;

    // the static objects are destructed on exit from the function and are not 
    // dynamically allocated so the null_deleter is used to prevent a deletion
    // by the shared pointer implementation
    struct null_deleter
    {
        void operator()(void const *) const
        {
        }
    };

    // create shared pointers from statically allocated objects with null deleter
    std::shared_ptr<Reader6>     pr(&r,null_deleter());
    std::shared_ptr<Transform6>  pt(&t,null_deleter());
    std::shared_ptr<Classifier6> pc(&c,null_deleter());
    std::shared_ptr<Writer6>     pw(&w,null_deleter());

    pipe_data_t        v = pipe_data_t();

    // set up the pipeline
    // reader -> transform -> classifier -> writer
    auto x = compose<pipe_data_t>(pr,pt,pc,pw);

    // execute it
    x->exec(v);

    // alternative composition. this does the same thing as 'compose' above
    // pr->pipe(pt)->pipe(pc)->pipe(pw);
    // pr->exec(v);
}
