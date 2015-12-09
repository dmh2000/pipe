#include <cstdio>
#include "pipe.h"

struct lla_t {
    double m_lat;
    double m_lon;
    double m_alt;

    /**
     * default constructor
     */
    lla_t() : m_lat(0.0),m_lon(0.0),m_alt(0.0) {
    }

    /**
     * initializing constructor
     */
    lla_t(double lat,double lon,double alt): m_lat(lat),m_lon(lon),m_alt(alt)
    {
    }

    /**
     * destructor
     * virtual in case of inheritance
     */
    virtual ~lla_t()
    {
    }


    /** 
     * copy constructor
     */
    lla_t(const lla_t& b) {
        m_lat = b.m_lat;
        m_lon = b.m_lon;
        m_alt = b.m_alt;
    }

    /**
     * copy assignment operator
     */
    lla_t &operator=(const lla_t &b) {
        m_lat = b.m_lat;
        m_lon = b.m_lon;
        m_alt = b.m_alt;
        return *this;
    }

    void print(const char *s="") const {
        printf("%s : %.6f,%.6f,%.6f\n",s,m_lat,m_lon,m_alt);
    }
};


using namespace Pipe;

struct Writer: public pipe_t<lla_t>
{
    Writer() {}

    virtual ~Writer()
    {
        printf("~Writer\n");
    }

    void exec(const lla_t &pos) override
    {
        pos.print();
        next(pos);
    }
};

struct Classifier : public pipe_t<lla_t>
{
    Classifier() {}

    virtual ~Classifier()
    {
        printf("~Classifier\n");
    }

    void exec(const lla_t &pos)  override
    {
        lla_t x = pos;
        x.m_lon+=10.0;
        x.m_lat+=10.0;
        x.m_alt+=10.0;

        next(x);
    }
};

struct Transform : public pipe_t<lla_t>
{
    Transform() {}

    virtual ~Transform()
    {
        printf("~Transform\n");
    }

    void exec(const lla_t &pos)  override
    {
        lla_t x = pos;
        x.m_lon+=5.0;
        x.m_lat+=5.0;
        x.m_alt+=5.0;

        next(x);
    }
};

struct Reader  : public pipe_t<lla_t>
{
    Reader() {}

    virtual ~Reader() {
        printf("~Reader\n");
    }

    void exec(const lla_t &pos)  override
    {
        for(int i=0;i<10;++i) {
            lla_t x = pos;
            x.m_lon+=1.0;
            x.m_lat+=1.0;
            x.m_alt+=1.0;

            next(x);
        }
    }
};


void test1() 
{
    printf("TEST explicit pipe between dynamically allocated classes with an object as the piped data (lla_t = latitude/longitude/altitude)\n");
    printf("uses inheritance of the pipe_t object\n");
    std::shared_ptr<Reader>     r = std::make_shared<Reader>();
    std::shared_ptr<Transform>  t = std::make_shared<Transform>();
    std::shared_ptr<Classifier> c = std::make_shared<Classifier>();
    std::shared_ptr<Writer>     w = std::make_shared<Writer>();

    lla_t        v = lla_t(0.0,0.0,0.0);

    // set up the pipeline
    // reader -> transform -> classifier -> writer
    r->pipe(t)->pipe(c)->pipe(w);

    // execute it
    r->exec(v);
}

// null deleter for shared pointer to static objects


void test2()
{
    printf("TEST explicit pipe between statically allocated classes with an object as the piped data (lla_t = latitude/longitude/altitude)\n");
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

    lla_t        v = lla_t(0.0,0.0,0.0);

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
    bool operator()(lla_t &lla) {
        lla.print("1");
        lla.m_lat += 1.0;
        lla.m_lon += 2.0;
        count--;
        return (count == 0);
    }
};

// one to one transform
struct functor2
{
    bool operator()(lla_t &lla)
    {
        lla.print("2");
        lla.m_alt += 100;
        return true;
    }
};

// one to one transform
struct functor3
{
    bool operator()(lla_t &lla)
    {
        lla.print("3");
        return true;
    }
};

void test3() 
{
    printf("TEST composed pipeline with functor objects\n");
    printf("uses composition rather than inheritance\n");

    // create the piped functions
    auto a = pipe<lla_t>(functor1());
    auto b = pipe<lla_t>(functor2());
    auto c = pipe<lla_t>(functor3());

    // compose them into an a -> b -> c pipeline
    auto x = compose<lla_t>(a,b,c);

    // execute the pipeline
    lla_t lla;
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

int main(int argc,char *argv[])
{
    test1();
    test2();
    test3();
    test4();

    return 0;
}

