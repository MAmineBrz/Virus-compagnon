#include <gtk/gtk.h>
#include <string.h>
#include <stdlib.h>

GtkWidget *entree_texte, *entree_cle, *label_resultat;

void calculer() {
    const char *message = gtk_entry_get_text(GTK_ENTRY(entree_texte));
    int cle = atoi(gtk_entry_get_text(GTK_ENTRY(entree_cle)));
    
    char resultat[1000];
    strcpy(resultat, message);

    for (int i = 0; i < strlen(resultat); i++) {
        if (resultat[i] != '\n') {
            resultat[i] = resultat[i] - cle; 
        }
    }

    gtk_label_set_text(GTK_LABEL(label_resultat), resultat);
}

int main(int argc, char **argv) {
    gtk_init(&argc, &argv);

    GtkWidget *fenetre = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    GtkWidget *boite = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    GtkWidget *l1 = gtk_label_new("Texte à chiffrer :");
    entree_texte = gtk_entry_new();
    GtkWidget *l2 = gtk_label_new("votre clé :");
    entree_cle = gtk_entry_new();
    GtkWidget *bouton = gtk_button_new_with_label("CHIFFRER");
    label_resultat = gtk_label_new("...");

    gtk_container_add(GTK_CONTAINER(fenetre), boite);
    gtk_box_pack_start(GTK_BOX(boite), l1, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(boite), entree_texte, 0, 0, 5);
    gtk_box_pack_start(GTK_BOX(boite), l2, 0, 0, 0);
    gtk_box_pack_start(GTK_BOX(boite), entree_cle, 0, 0, 5);
    gtk_box_pack_start(GTK_BOX(boite), bouton, 0, 0, 5);
    gtk_box_pack_start(GTK_BOX(boite), label_resultat, 0, 0, 5);

    g_signal_connect(bouton, "clicked", G_CALLBACK(calculer), NULL);
    g_signal_connect(fenetre, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_widget_show_all(fenetre);
    gtk_main();
    
    return 0;
}
