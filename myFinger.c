#include "myFinger.h"
#include <stdio.h>
#include <string.h>

#define MAX_USERS 128

void print_table_header()
{
    printf("| ");
    print_centered("Login", 15);
    printf("| ");
    print_centered("Name", 15);
    printf("| ");
    print_centered("Tty", 10);
    printf("| ");
    print_centered("Idle", 11);
    printf("| ");
    print_centered("Login Time", 20);
    printf(" |\n");
    printf("-----------------------------------------------------------------------------------\n");
}

time_t get_idle_time(const char *tty)
{
    struct stat st;
    char tty_path[64] = "/dev/";
    strncat(tty_path, tty, sizeof(tty_path) - strlen(tty_path) - 1);

    if (stat(tty_path, &st) == -1)
    {
        perror("stat");
        return -1;
    }

    time_t current_time = time(NULL);
    return current_time - st.st_mtime; // Calcola l'idle time come differenza in secondi
}

void remove_commas(const char *input, char *output)
{
    while (*input)
    {
        if (*input != ',')
        {
            *output++ = *input; // Copia il carattere se non è una virgola
        }
        input++;
    }
    *output = '\0'; // Aggiungi terminatore nullo alla fine
}

struct utmp *get_user_utmp(const char *username)
{
    struct utmp *utmp_entry;
    setutent();
    while ((utmp_entry = getutent()) != NULL)
    {
        if (utmp_entry->ut_type == USER_PROCESS && strcmp(utmp_entry->ut_user, username) == 0)
        {
            endutent();
            return utmp_entry;
        }
    }
    endutent();
    return NULL;
}

UserInfo get_user_info(struct utmp *user)
{
    UserInfo info;
    struct passwd *pwd = getpwnam(user->ut_user);
    if (pwd == NULL)
    {
        strcpy(info.username, "Unknown");
        return info;
    }
    strncpy(info.username, pwd->pw_name, sizeof(info.username) - 1);
    remove_commas(pwd->pw_gecos, info.real_name);
    strncpy(info.home_dir, pwd->pw_dir, sizeof(info.home_dir) - 1);
    strncpy(info.shell, pwd->pw_shell, sizeof(info.shell) - 1);
    info.login_time = user->ut_tv.tv_sec;
    strncpy(info.tty, user->ut_line, sizeof(info.tty) - 1);
    info.idle_time = get_idle_time(info.tty);
    info.last_login_time = user->ut_time;

    struct tm *tm_info = localtime(&info.login_time);
    strftime(info.shortf_log_time, sizeof(info.shortf_log_time), "%b %d %H:%M", tm_info);
    strftime(info.longf_log_time, sizeof(info.longf_log_time), "%a %b %d %H:%M (%Z)", tm_info);

    return info;
}

void print_centered(const char *text, int width)
{
    int len = strlen(text);
    int padding = width - len;

    if (padding > 0)
    {
        int pad_left = padding / 2;
        int pad_right = padding - pad_left;

        // Stampa padding a sinistra
        for (int i = 0; i < pad_left; i++)
        {
            printf(" ");
        }

        // Stampa il testo
        printf("%s", text);

        // Stampa padding a destra
        for (int i = 0; i < pad_right; i++)
        {
            printf(" ");
        }
    }
    else
    {
        // Se il testo è più lungo della larghezza, stampalo direttamente
        printf("%s", text);
    }
}

void print_user_info(const UserInfo *user, char mode)
{
    static UserInfo last_user;

    // Calcola ore e minuti dall'idle time
    int hours = user->idle_time / 3600;
    int minutes = (user->idle_time % 3600) / 60;
    int seconds = user->idle_time % 60;

    if (mode == 's')
    {
        // Stampa le informazioni in formato tabellare
        printf("| ");
        print_centered(user->username, 15);
        printf("| ");
        print_centered(user->real_name, 15);
        printf("| ");
        print_centered(user->tty, 10);
        printf("| ");
        printf("   %02d   | ", minutes);
        print_centered(user->shortf_log_time, 20);
        printf("|\n");
    }
    else if (mode == 'l')
    {
        if (strcmp(last_user.username, user->username) != 0)
        {
            printf("Login: %s\n", user->username);
            printf("Name: %s\n", user->real_name);
            printf("Home Dir: %s\n", user->home_dir);
            printf("Shell: %s\n", user->shell);
        }

        printf("On since %s on ", user->longf_log_time);
        printf("%s\n", user->tty);
        printf("   %02d minutes %02d seconds idle\n", minutes, seconds);

        last_user = *user;
    }
    else
    {
        fprintf(stderr, "Modalità non valida: usa 's' o 'l'.\n");
    }
}

void print_all_logged_users(char mode)
{
    if (mode == 's')
    {
        print_table_header();
    }
    struct utmp *u;

    setutent(); // Riporta il puntatore al primo record

    while ((u = getutent()) != NULL)
    {
        if (u->ut_type == USER_PROCESS)
        {
            UserInfo user = get_user_info(u);
            print_user_info(&user, mode);
            printf("\n"); // Separatore tra utenti
        }
    }

    endutent(); // Chiude il file utmp
}