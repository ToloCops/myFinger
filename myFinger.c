#include "myFinger.h"
#include <stdio.h>
#include <string.h>

#define MAX_USERS 128

void print_table_header() {
    printf("| ");
    print_centered("Username", 15);
    printf("| ");
    print_centered("Real Name", 15);
    printf("| ");
    print_centered("Tty", 10);
    printf("| ");
    print_centered("Idle", 11);
    printf("| ");
    print_centered("Login Time", 20);
    printf(" |\n");
    printf("-----------------------------------------------------------------------------------\n");
}

time_t get_idle_time(const char* tty) {
    struct stat st;
    char tty_path[64] = "/dev/";
    strncat(tty_path, tty, sizeof(tty_path) - strlen(tty_path) - 1);

    if (stat(tty_path, &st) == -1) {
        perror("stat");
        return -1;
    }

    time_t current_time = time(NULL);
    return current_time - st.st_mtime; // Calcola l'idle time come differenza in secondi
}

void remove_commas(const char* input, char* output) {
    while (*input) {
        if (*input != ',') {
            *output++ = *input; // Copia il carattere se non è una virgola
        }
        input++;
    }
    *output = '\0'; // Aggiungi terminatore nullo alla fine
}

UserInfo get_user_info(char *username)
{
    struct passwd *pwd = getpwnam(username);
    struct utmp *utmp_entry;
    UserInfo user;
    if (pwd == NULL) {
        strcpy(user.username, "Unknown");
        return user;
    }
    strncpy(user.username, pwd->pw_name, sizeof(user.username) - 1);
    remove_commas(pwd->pw_gecos, user.real_name);
    strncpy(user.home_dir, pwd->pw_dir, sizeof(user.home_dir) - 1);
    strncpy(user.shell, pwd->pw_shell, sizeof(user.shell) - 1);
    setutent();
    while ((utmp_entry = getutent()) != NULL) {
        if (utmp_entry->ut_type == USER_PROCESS && strcmp(utmp_entry->ut_user, username) == 0) {
            user.login_time = utmp_entry->ut_tv.tv_sec;
            strncpy(user.tty, utmp_entry->ut_line, sizeof(user.tty) - 1);
            user.idle_time = get_idle_time(user.tty);
            user.last_login_time = utmp_entry->ut_time;

            struct tm* tm_info = localtime(&user.login_time);
            strftime(user.formatted_login_time, sizeof(user.formatted_login_time), "%Y-%m-%d %H:%M:%S", tm_info);
            break;
        }
    }
    endutent();
    return user;
}

void print_centered(const char* text, int width) {
    int len = strlen(text);
    int padding = width - len;

    if (padding > 0) {
        int pad_left = padding / 2;
        int pad_right = padding - pad_left;

        // Stampa padding a sinistra
        for (int i = 0; i < pad_left; i++) {
            printf(" ");
        }

        // Stampa il testo
        printf("%s", text);

        // Stampa padding a destra
        for (int i = 0; i < pad_right; i++) {
            printf(" ");
        }
    } else {
        // Se il testo è più lungo della larghezza, stampalo direttamente
        printf("%s", text);
    }
}

void print_user_info(const UserInfo* user, char mode) {
    // Calcola ore e minuti dall'idle time
    int hours = user->idle_time / 3600;
    int minutes = (user->idle_time % 3600) / 60;

    if (mode == 's') {
        // Stampa le informazioni in formato tabellare
        printf("| ");
        print_centered(user->username, 15);
        printf("| ");
        print_centered(user->real_name, 15);
        printf("| ");
        print_centered(user->tty, 10);
        printf("| ");
        printf("   %02d:%02d   | ", hours, minutes);
        print_centered(user->formatted_login_time, 20);
        printf("|\n");
    } else if (mode == 'l') {
        // Stampa le informazioni in formato dettagliato
        printf("Username: %s\n", user->username);
        printf("Real Name: %s\n", user->real_name);
        printf("Home Dir: %s\n", user->home_dir);
        printf("Shell: %s\n", user->shell);
        printf("Tty: %s\n", user->tty);
        printf("Idle Time: %02d:%02d\n", hours, minutes);
        printf("Login Time: %s\n", user->formatted_login_time);
    } else {
        fprintf(stderr, "Modalità non valida: usa 's' o 'l'.\n");
    }
}

void print_all_logged_users(char mode) {
    if (mode == 's') {
        print_table_header();
    }
    struct utmp* u;
    char printed_users[MAX_USERS][32]; // Array per memorizzare i nomi degli utenti stampati
    int user_count = 0;

    setutent(); // Riporta il puntatore al primo record

    while ((u = getutent()) != NULL) {
        if (u->ut_type == USER_PROCESS) {
            int already_printed = 0;

            // Verifica se l'utente è già stato stampato
            for (int i = 0; i < user_count; i++) {
                if (strcmp(printed_users[i], u->ut_user) == 0) {
                    already_printed = 1;
                    break;
                }
            }

            // Se non è stato stampato, aggiungilo e stampa le informazioni
            if (!already_printed) {
                UserInfo user = get_user_info(u->ut_user);
                print_user_info(&user, mode);
                printf("\n"); // Separatore tra utenti

                // Aggiungi l'utente all'array di utenti stampati
                if (user_count < MAX_USERS) {
                    strncpy(printed_users[user_count], u->ut_user, sizeof(printed_users[user_count]) - 1);
                    user_count++;
                }
            }
        }
    }

    endutent(); // Chiude il file utmp
}