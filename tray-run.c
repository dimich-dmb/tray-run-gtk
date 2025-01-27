#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/wait.h>

#include <gtk/gtk.h>

static void on_tray_icon_click(GtkStatusIcon *, gpointer data)
{
    char * const *cmd = data;

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return;
    }
    if (pid != 0) {
        if (waitpid(pid, NULL, 0) == -1) {
            perror("waitpid");
        }
        return;
    }
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid != 0) {
        exit(EXIT_SUCCESS);
    }
    execvp(cmd[0], cmd);
    fprintf(stderr, "execve: '%s': %s\n", cmd[0], strerror(errno));
    exit(EXIT_FAILURE);
}

static void on_tray_icon_menu(GtkStatusIcon *, guint, guint, gpointer data)
{
    gtk_menu_popup_at_pointer(GTK_MENU(data), NULL);
}

int main(int argc, char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s icon command [ args... ]\n", argv[0]);
        return 1;
    }

    gtk_init(NULL, NULL);

    GtkStatusIcon *tray_icon;

    if (argv[1][0] == '/' || (argv[1][0] == '.' && argv[1][1] == '/')) {
        tray_icon = gtk_status_icon_new_from_file(argv[1]);
    } else {
        tray_icon = gtk_status_icon_new_from_icon_name(argv[1]);
    }
    if (tray_icon == NULL) {
        fprintf(stderr, "Icon load error\n");
        return 2;
    }

    gtk_status_icon_set_visible(tray_icon, TRUE);
    gtk_status_icon_set_tooltip_text(tray_icon, argv[2]);

    GtkWidget *menu = gtk_menu_new();
    GtkWidget *item = gtk_menu_item_new_with_mnemonic("E_xit");
    gtk_widget_show(item);
    gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
    g_signal_connect(G_OBJECT(item), "activate", G_CALLBACK(gtk_main_quit), NULL);

    g_signal_connect(G_OBJECT(tray_icon), "activate", G_CALLBACK(on_tray_icon_click), argv + 2);
    g_signal_connect(G_OBJECT(tray_icon), "popup-menu", G_CALLBACK(on_tray_icon_menu), menu);

    gtk_main();

    gtk_widget_destroy(item);
    gtk_widget_destroy(menu);

    g_object_unref(tray_icon);

    return 0;
}
