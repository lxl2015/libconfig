
#include "lxl_misc.h"
#include "lxl_cfg.h"


int test1 = 1;
char *test2 = NULL;

int main(int argc, const char *argv[])
{
    static struct cfg_line cfg[] = 
    {

        {"TEST1", &test1, TYPE_INT, PARM_OPT, 1, 999999},
        {"TEST2", &test2, TYPE_STRING, PARM_OPT, 0, 0}
    };



    lxl_parse_cfg_file("../conf/configure.conf", cfg, LXL_CFG_FILE_REQUIRED, LXL_CFG_STRICT);



    printf("test1 = %d\n, test2= %s\n",test1, test2);

    free(test2);
    return 0;
}

