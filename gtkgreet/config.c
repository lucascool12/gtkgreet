#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <gtk/gtk.h>
#include <limits.h>

#include "gtkgreet.h"
#include "config.h"


const char* strip_sort_prefix(const char* str) {
    int prefix = 0;
    for (int i = 0; i < MAX_COMMAND_LEN; i++) {
        if (str[i] == '-' && i > 0) {
            prefix = i + 1;
            break;
        } else if (!(str[i] >= '0' && str[i] <= '9') || str[i] == '\0') {
            break;
        } else {
            continue;
        }
    }
    return &str[prefix];
}

int cmd_strcmp(const void* str1, const void* str2) {
    const char* prefix1 = strip_sort_prefix(str1);
    const char* prefix2 = strip_sort_prefix(str2);
    int len1 = prefix1 - (char*)str1;
    int len2 = prefix2 - (char*)str2;

    if (len1 < len2) {
        return 1;
    } else if (len1 > len2) {
        return -1;
    } else if (len1 > 0 && len2 > 0){
        return strncmp(str1, str2, len1);
    } else if (len1 > 0) {
        return 1;
    } else if (len2 > 0) {
        return -1;
    } else {
        return 0;
    }
}


int config_update_command_selector(GtkWidget *combobox) {
    int entries = 0;
    if (gtkgreet->command != NULL) {
        gtk_combo_box_text_append((GtkComboBoxText*)combobox, NULL, gtkgreet->command);
    	entries++;
    }

    const char* file_name;

    GError *error = NULL;
    GDir *d_envs = g_dir_open(ENVIRONMENTS_DIR, 0, &error);

    if (error != NULL) {
        return entries;
    }
    int files = 0;
    while ((file_name = g_dir_read_name(d_envs))) {
        if (file_name == NULL) {
            break;
        }
        strncpy(gtkgreet->commands[files], file_name, MAX_COMMAND_LEN - 1);
        files++;
        entries++;
        if (files >= MAX_COMMANDS) {
            break;
        }

    }
    g_dir_close(d_envs);

    qsort(gtkgreet->commands, files, MAX_COMMAND_LEN, cmd_strcmp);

    for (int i = 0; i < files; i++) {
        gtk_combo_box_text_append((GtkComboBoxText*)combobox, NULL, strip_sort_prefix(gtkgreet->commands[i]));
    }

    return entries;
}
