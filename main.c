#include "myFinger.h"
#include <stdio.h>

int main(int argc, char *argv[])
{
    if (argc == 2) {
        UserInfo user = get_user_info(argv[1]);
        print_user_info(&user);
    } else {
        print_table_header();
        get_all_logged_users();
    }
    return 0;
}

