#include <crtdbg.h>

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

void test()
{
    /*
    test1();
    test2();
    test3();
    test4();
    */
    test5();
    test6();
}

int main(int argc,char *argv[])
{
    // Get current flag
    int tmpFlag = _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG);
    tmpFlag |= _CRTDBG_LEAK_CHECK_DF;
    tmpFlag |= _CRTDBG_CHECK_CRT_DF;
    tmpFlag |= _CRTDBG_CHECK_ALWAYS_DF;
    tmpFlag |= _CRTDBG_ALLOC_MEM_DF;

    // Set flag to the new value
    _CrtSetDbgFlag(tmpFlag);


    test();
    return 0;
}

