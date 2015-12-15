#include <cstdio>
#include <functional>
#include <memory>

using namespace std::placeholders;

void reader(int i,int j)
{
    printf("%d %d\n",i,j);
}

struct X {
    X(int j) : i(j) {}

    int i;
    void log() {
        printf("%d\n",i);
    }
};

X x(1);

struct Z {
    int a;
    int b;
    int c;
};

Z z = {1,2,3};

void logz(Z m) {
    printf("%d %d %d\n",m.a,m.b,m.c);
}

struct functor {
    int operator()(int i,int j) {
        printf("functor : %d %d\n",i,j);
        return i+j;
    }
};

void test() 
{
    auto f = std::bind(reader,_1,2);
    f(1);

    auto g = std::bind(&X::log,x);
    g();

    auto h = std::bind(logz,z);
    h();

    auto a = std::function<void(int,int)>(reader);
    a(1,2);
    std::function<void(int,int)> d = std::function<void(int,int)>(reader);
    d(1,2);

    auto b = std::function<void(int,int)>([](int i,int j) -> void {printf("b: %d %d\n",i,j);});
    b(1,2);

    auto c = std::function<int(int,int)>(functor());
    c(1,2);
}

int main(int argc,char *argv[])
{
    test();
    return 0;
}
