#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"
#include "data.h"


using namespace Pipe;

struct Writer
{
    Writer() {}

    virtual ~Writer()
    {
        printf("~Writer\n");
    }

    // Writer is a functor without state so just implement operator()
    bool operator()(pipe_data_t &data)
    {
        // print the input data
        pipe_data_t::print("W",data);
        return true;
    }
};

struct Classifier: public pipe_t<pipe_data_t>
{
    Classifier() {}

    virtual ~Classifier()
    {
        printf("~Classifier\n");
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

struct Transform: public pipe_t<pipe_data_t>
{
    Transform() {}

    virtual ~Transform()
    {
        printf("~Transform\n");
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

struct Reader: public pipe_t<pipe_data_t>
{
    Reader() {}

    virtual ~Reader()
    {
        printf("~Reader\n");
    }

    void exec(const pipe_data_t &data) override
    {
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


void test1()
{
    printf("TEST explicit pipe between dynamically allocated classes with an object as the piped data (pipe_data_t = latitude/longitude/altitude)\n");
    printf("uses inheritance of the pipe_t object\n");

    // use objects for Reader,Transform and Classifier
    std::shared_ptr<Reader>     r = std::make_shared<Reader>();
    std::shared_ptr<Transform>  t = std::make_shared<Transform>();
    std::shared_ptr<Classifier> c = std::make_shared<Classifier>();
    
    // use a functor for writing instead of object
    auto w  = pipe<pipe_data_t>(Writer());

    pipe_data_t  v = pipe_data_t();

    // set up the pipeline
    // reader -> transform -> classifier -> writer
    auto x = compose<pipe_data_t>(r,t,c,w);

    // execute it
    x->exec(v);
}
