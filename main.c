#include "myFinger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
    int opt;
    int s_flag = 0, l_flag = 0;
    int p_flag = 0;
    int m_flag = 0;

    char *username = NULL;

    // Analizza le opzioni con getopt
    while ((opt = getopt(argc, argv, "sl")) != -1) {
        switch (opt) {
            case 's':
                s_flag = 1;
                break;
            case 'l':
                l_flag = 1;
                break;
            case 'p':
                p_flag = 1;
                break;
            case 'm':
                m_flag = 1;
                break;
            default:
                fprintf(stderr, "Usage: %s [-s] [-l] [-p] [-m] [username]\n", argv[0]);
                exit(EXIT_FAILURE);
        }
    }

    // Controlla se c'è un nome utente dopo le opzioni
    if (optind < argc) {
        username = argv[optind];
    }

    // Logica in base alle opzioni passate
    if (l_flag) {
        if (username) {
            UserInfo user = get_user_info(username);
            print_user_info(&user, 'l');
        } else {
            print_all_logged_users('l');
        }
    } else if (s_flag) {
        if (username) {
            UserInfo user = get_user_info(username);
            print_table_header();
            print_user_info(&user, 's');
        } else {
            print_all_logged_users('s');
        }
    } else if (username) {
        UserInfo user = get_user_info(username);
        print_user_info(&user, 'l');
    } else {
        print_all_logged_users('s');
    }

    return 0;
}

