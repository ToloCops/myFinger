#ifndef FINGER_H
#define FINGER_H

#include <pwd.h>
#include <utmp.h>
#include <time.h>
#include <sys/stat.h>

typedef struct
{
    char username[32];
    char real_name[128];
    char home_dir[256];
    char shell[256];
    time_t login_time;
    char tty[32];
    time_t idle_time;
    time_t last_login_time;
    char shortf_log_time[20]; // Formato: "Jan 3 16:01"
    char longf_log_time[40];  // Formato: "Fri Jan 3 16:01 (CET)"
} UserInfo;

void print_table_header();

void print_all_logged_users(char mode);

void print_centered(const char *text, int width);

time_t get_idle_time(const char *tty);

struct utmp *get_user_utmp(const char *username);

UserInfo get_user_info(struct utmp *user);

void print_user_info(const UserInfo *user, char mode);

#endif