// This file is for test 
//
#include "all.h"
#include "strbuf.h"

int main(int argc, char **argv)
{
    struct strbuf test = STRBUF_INIT;
    strbuf_getwholeline(&test, stdin, '\n');
    printf("%s\n", test.buf);




    strbuf_release(&test);
    return 0;
}
