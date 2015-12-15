#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"
#include "data.h"


using namespace Pipe;

struct Writer5
{
    Writer5() 
    {
        printf("Writer\n");
    }

    Writer5(const Writer5 &b) {
        printf("Writer cc\n");
    }

    virtual ~Writer5()
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

struct Classifier5: public pipe_t<pipe_data_t>
{
    Classifier5() 
    {
        printf("Classifier\n");
    }


    virtual ~Classifier5()
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

struct Transform5: public pipe_t<pipe_data_t>
{
    Transform5()
    {
        printf("Transform\n");
    }


    virtual ~Transform5()
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

class Reader5: public pipe_t<pipe_data_t>
{
private:
    int count;
public:
    Reader5()
    {
        count = 0;
        printf("Reader\n");
    }


    virtual ~Reader5()
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


void test5()
{
    printf("TEST explicit pipe between dynamically allocated classes with an object as the piped data (pipe_data_t = latitude/longitude/altitude)\n");
    printf("uses inheritance of the pipe_t object\n");

    // use objects for Reader,Transform and Classifier
    std::shared_ptr<Reader5>     r = std::make_shared<Reader5>();
    std::shared_ptr<Transform5>  t = std::make_shared<Transform5>();
    std::shared_ptr<Classifier5> c = std::make_shared<Classifier5>();

    // use a functor for writing instead of object
    auto w  = pipe<pipe_data_t>(Writer5());

    pipe_data_t  v = pipe_data_t();

    // set up the pipeline
    // reader -> transform -> classifier -> writer
    auto x = compose<pipe_data_t>(r,t,c,w);

    // execute it
    x->exec(v);
}

