#include <gtk/gtk.h>
#include <stdlib.h>
#include <stdio.h>

GtkWidget *entry_a, *entry_b, *combo, *result;

void calculer() {
    int a = atoi(gtk_entry_get_text(GTK_ENTRY(entry_a)));
    int b = atoi(gtk_entry_get_text(GTK_ENTRY(entry_b)));
    char op = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(combo))[0];

    char res[100];

    switch(op){
        case '+': sprintf(res, "%d + %d = %d", a, b, a+b); break;
        case '-': sprintf(res, "%d - %d = %d", a, b, a-b); break;
        case '*': sprintf(res, "%d * %d = %d", a, b, a*b); break;
        case '/': 
            if(b==0) sprintf(res, "Erreur division");
            else sprintf(res, "%d / %d = %d", a, b, a/b);
            break;
        default: sprintf(res, "Erreur");
    }

    gtk_label_set_text(GTK_LABEL(result), res);
}

int main(int argc, char *argv[]) {
    gtk_init(&argc, &argv);

    GtkWidget *win = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(win), "Calculatrice");
    gtk_container_set_border_width(GTK_CONTAINER(win), 10);

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_add(GTK_CONTAINER(win), box);

    entry_a = gtk_entry_new();
    entry_b = gtk_entry_new();

    combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "+");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "-");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "*");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "/");
    gtk_combo_box_set_active(GTK_COMBO_BOX(combo), 0);

    GtkWidget *btn = gtk_button_new_with_label("Calculer");
    result = gtk_label_new("Resultat");

    g_signal_connect(btn, "clicked", G_CALLBACK(calculer), NULL);
    g_signal_connect(win, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_box_pack_start(GTK_BOX(box), entry_a, 0,0,0);
    gtk_box_pack_start(GTK_BOX(box), entry_b, 0,0,0);
    gtk_box_pack_start(GTK_BOX(box), combo, 0,0,0);
    gtk_box_pack_start(GTK_BOX(box), btn, 0,0,0);
    gtk_box_pack_start(GTK_BOX(box), result, 0,0,0);

    gtk_widget_show_all(win);
    gtk_main();
}
