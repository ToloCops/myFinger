#ifndef FINGER_H
#define FINGER_H

#include <pwd.h>
#include <utmp.h>
#include <time.h>
#include <sys/stat.h>

typedef struct {
    char username[32];
    char real_name[128];
    char home_dir[256];
    char shell[256];
    time_t login_time;
    char tty[32];
    time_t idle_time;
    time_t last_login_time;
    char formatted_login_time[20];
} UserInfo;

void print_table_header();

void get_all_logged_users();

void print_centered(const char* text, int width);

time_t get_idle_time(const char* tty);

UserInfo get_user_info(char *username);

void print_user_info_row(const UserInfo* user);

void print_user_info(const UserInfo* user);

#endif