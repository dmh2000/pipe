#include <cstdio>
#include <cstring>
#include <cstdint>
#include "pipe.h"


/** define a more complex type */
struct lla_t {
    double m_lat;
    double m_lon;
    double m_alt;

    /** be sure to initialize the data */
    lla_t() : m_lat(0.0),m_lon(0.0),m_alt(0.0) 
    {
    }
};


/** create a struct with all data needed through the pipeline */
struct pipe_data_t {
    uint32_t u32;
    lla_t    lla;
    int      cls;

    /** be sure to initialize the data */
    pipe_data_t() : u32(0),cls(0) {
    }

    /** print the data */
    static void print(const char *s,const pipe_data_t &data)
    {
        printf("%s : %u %d %.3f %.3f %.3f\n",s,data.u32,data.cls,data.lla.m_lat,data.lla.m_lon,data.lla.m_alt);
    }
};



using namespace Pipe;

struct Writer: public pipe_t<pipe_data_t>
{
    Writer() {}

    virtual ~Writer()
    {
        printf("~Writer\n");
    }

    void exec(const pipe_data_t &data) override
    {
        // print the input data
        pipe_data_t::print("W",data);
        // call next in pipeline
        next(data);
    }
};

struct Classifier : public pipe_t<pipe_data_t>
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

struct Transform : public pipe_t<pipe_data_t>
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

struct Reader  : public pipe_t<pipe_data_t>
{
    Reader() {}

    virtual ~Reader() {
        printf("~Reader\n");
    }

    void exec(const pipe_data_t &data)  override
    {
        // print the input data
        pipe_data_t::print("R",data);

        // create multiple mutable copies
        for(int i=0;i<10;++i) {
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
    std::shared_ptr<Reader>     r = std::make_shared<Reader>();
    std::shared_ptr<Transform>  t = std::make_shared<Transform>();
    std::shared_ptr<Classifier> c = std::make_shared<Classifier>();
    std::shared_ptr<Writer>     w = std::make_shared<Writer>();

    pipe_data_t        v = pipe_data_t();

    // set up the pipeline
    // reader -> transform -> classifier -> writer
    r->pipe(t)->pipe(c)->pipe(w);

    // execute it
    r->exec(v);
}

// null deleter for shared pointer to static objects


void test2()
{
    printf("TEST explicit pipe between statically allocated classes with an object as the piped data (pipe_data_t = latitude/longitude/altitude)\n");
    printf("only difference is that the shared_ptr instances need a null deleter\n");
    printf("uses inheritance of the pipe_t object\n");
    Reader     r;
    Transform  t;
    Classifier c;
    Writer     w;

    struct null_deleter
    {
        void operator()(void const *) const
        {
        }
    };

    std::shared_ptr<Reader>     pr(&r,null_deleter());
    std::shared_ptr<Transform>  pt(&t,null_deleter());
    std::shared_ptr<Classifier> pc(&c,null_deleter());
    std::shared_ptr<Writer>     pw(&w,null_deleter());

    pipe_data_t        v = pipe_data_t();

    // set up the pipeline
    // reader -> transform -> classifier -> writer
    pr->pipe(pt)->pipe(pc)->pipe(pw);

    // execute it
    pr->exec(v);
}

// =======================================================================================
// functor classes for test 3
// =======================================================================================

// one to many transform
struct functor1 {
    int count;
    functor1() : count(10) {}

    // generate multiple output, return done == 'true' with last output
    bool operator()(pipe_data_t &data) {
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

    // compose them into an a -> b -> c pipeline
    auto x = compose<pipe_data_t>(a,b,c,d);

    // execute the pipeline
    pipe_data_t lla;
    x->exec(lla);
}

void test4()
{
    printf("TEST composed pipeline with lambda functions\n");
    printf("uses composition rather than inheritance\n");

    // create the piped functions
    auto a = pipe<int>([](int &i) -> bool {printf("a: %d\n",i); i++; return true;});
    auto b = pipe<int>([](int &i) -> bool {printf("b: %d\n",i); i++; return true;});
    auto c = pipe<int>([](int &i) -> bool {printf("c: %d\n",i); i++; return true;});

    // compose them into an a -> b -> c pipeline
    auto x = compose<int>(a,b,c);

    // execute the pipeline
    x->exec(1);
}

struct F4: public pipe_t<int>
{
    F4() {
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

void test5()
{
    printf("TEST composed pipeline with mix of objects,functors and lambda functions\n");
    printf("objects and/or functors when state is needed, functors/lambdas when code can be pure\n");
    
    std::shared_ptr<F4>     r = std::make_shared<F4>();

    // create the piped functions
    auto a = pipe<int>([](int &i) -> bool { printf("a: %d\n",i); i++; return true; });
    auto b = pipe<int>([](int &i) -> bool { printf("b: %d\n",i); i++; return true; });
    auto c = pipe<int>([](int &i) -> bool { printf("c: %d\n",i); i++; return true; });
    auto d = pipe<int>(functor5());

    // compose them into an a -> b -> c pipeline
    auto x = compose<int>(r,a,b,c,d);

    // execute the pipeline
    x->exec(10);
}
int main(int argc,char *argv[])
{
    // test1();
    // test2();
    // test3();
    // test4();
    test5();
    return 0;
}

