#include <gtk/gtk.h>

#include "main.h"
#include "fonction.h"

void exit_button_clicked(GtkWidget *bouton, st_widgets *st)
{
    g_application_quit(G_APPLICATION(st->application));
    gtk_main_quit();
}

void activateCMD(GtkApplication *application, st_widgets *st)
{
    GtkWidget *box_main = NULL;
    GtkWidget *box_scan = NULL;
    GtkWidget *box_scan_bouton = NULL;

    /* Window */
    st->window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(st->window), "Analyse");
    gtk_window_set_position(GTK_WINDOW(st->window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(st->window, WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable (GTK_WINDOW(st->window), FALSE);
    gtk_window_set_icon_name(GTK_WINDOW(st->window), IMAGE_ICON);

    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
    
    /* Bouton */
    st->bouton_retour   = gtk_button_new_with_label("Quitter");
    
    /* Notebook */
    st->notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(st->notebook), FALSE);

    /* TexView */
    st->scrolled_text = gtk_scrolled_window_new(NULL, NULL);
    st->textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(st->textview), FALSE);
    gtk_text_view_set_justification(GTK_TEXT_VIEW(st->textview), GTK_JUSTIFY_CENTER);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(st->textview), FALSE);

    /* TextBuffer */
    st->textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(st->textview));
    
    /* ProgressBar */
    st->progressbar = gtk_progress_bar_new();

    /* Set CMD */
    st->cmd_satus = 1;

    /* Box */
    box_main            = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_scan            = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_scan_bouton     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_box_set_homogeneous(GTK_BOX(box_scan_bouton), TRUE);    
    
    gtk_container_add(GTK_CONTAINER(st->window), box_main);
    gtk_container_add(GTK_CONTAINER(st->scrolled_text), st->textview); 

    space_in_box(box_scan_bouton, 3);
    gtk_box_pack_start(GTK_BOX(box_scan_bouton), st->bouton_retour, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(box_scan), st->scrolled_text, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_scan), st->progressbar, FALSE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box_scan), box_scan_bouton, FALSE, TRUE, 10);

    gtk_notebook_append_page(GTK_NOTEBOOK(st->notebook), box_scan, NULL);

    gtk_box_pack_end(GTK_BOX(box_main), st->notebook, TRUE, TRUE, 0);

    /* Signal */
    g_signal_connect(G_OBJECT(st->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(st->bouton_retour), "clicked", G_CALLBACK(exit_button_clicked), st);
    scan_cmd(st);
    
    gtk_widget_show_all(st->window);

    gtk_main();

}

void activate(GtkApplication *application, st_widgets *st)
{
    GtkWidget *box_main = NULL;
    GtkWidget *box_analyser = NULL;
    GtkWidget *box_analyser_file = NULL;
    GtkWidget *box_analyser_folder = NULL;
    GtkWidget *box_right_button = NULL;
    GtkWidget *box_right_h_b_a = NULL;
    GtkWidget *box_right_h_b_u = NULL;
    GtkWidget *box_scan = NULL;
    GtkWidget *box_scan_bouton = NULL;
    GtkWidget *imageClamav = NULL;
    GtkWidget *label_analyser = NULL;
    GtkWidget *label_update = NULL;
    GtkWidget *separateur = NULL;
    GtkWidget *label_analyser_file = NULL;
    GtkWidget *label_analyser_folder = NULL;

    /* Window */
    st->window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(st->window), "ClamGTK");
    gtk_window_set_position(GTK_WINDOW(st->window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(st->window, WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable (GTK_WINDOW(st->window), FALSE);
    gtk_window_set_icon_name(GTK_WINDOW(st->window), IMAGE_ICON);

    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    /* Image */
    imageClamav = gtk_image_new_from_file(IMAGE_CLAMAV);

    /* Bouton */
    st->bouton_retour   = gtk_button_new_with_label("Retour");
    st->bouton_analyser = gtk_button_new_with_image(IMAGE_BT_SEARCH);
    st->bouton_update   = gtk_button_new_with_image(IMAGE_BT_UPDATE);
    st->bouton_file     = gtk_button_new_with_image(IMAGE_BT_FILE);
    st->bouton_folder   = gtk_button_new_with_image(IMAGE_BT_FOLDER);

    /* Label */
    label_analyser        = gtk_label_new_with_markup("Analyser", 0);
    label_update          = gtk_label_new_with_markup("Update", 0);
    label_analyser_file   = gtk_label_new_with_markup("Fichier", 0);
    label_analyser_folder = gtk_label_new_with_markup("Repertoire", 0);

    /* Notebook */
    st->notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(st->notebook), FALSE);

    /* Seprateur*/
    separateur = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

    /* TexView */
    st->scrolled_text = gtk_scrolled_window_new(NULL, NULL);
    st->textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(st->textview), FALSE);
    gtk_text_view_set_justification(GTK_TEXT_VIEW(st->textview), GTK_JUSTIFY_CENTER);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(st->textview), FALSE);

    /* TextBuffer */
    st->textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(st->textview));
    
    /* ProgressBar */
    st->progressbar = gtk_progress_bar_new();

    /* Set CMD */
    st->cmd_satus = 0;

    /* Box */
    box_main            = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_right_button    = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_right_h_b_a     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    box_right_h_b_u     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    box_analyser        = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_analyser_file   = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_analyser_folder = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);

    box_scan            = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_scan_bouton     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_box_set_homogeneous(GTK_BOX(box_scan_bouton), TRUE);

    gtk_container_add(GTK_CONTAINER(st->window), box_main);
    gtk_container_add(GTK_CONTAINER(st->scrolled_text), st->textview); 

    gtk_box_pack_start(GTK_BOX(box_analyser), box_analyser_file, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_analyser), box_analyser_folder, TRUE, FALSE, 0);

    move_down_in_box(box_analyser_file, 8);
    gtk_box_pack_start(GTK_BOX(box_analyser_file), st->bouton_file, FALSE, FALSE, 0);
    move_down_in_box(box_analyser_file, 2);
    gtk_box_pack_start(GTK_BOX(box_analyser_file), label_analyser_file, FALSE, FALSE, 0);
    
    move_down_in_box(box_analyser_folder, 8);
    gtk_box_pack_start(GTK_BOX(box_analyser_folder), st->bouton_folder, FALSE, FALSE, 0);
    move_down_in_box(box_analyser_folder, 2);
    gtk_box_pack_start(GTK_BOX(box_analyser_folder), label_analyser_folder, FALSE, FALSE, 0);

    space_in_box(box_right_h_b_a, 4);
    space_in_box(box_right_h_b_u, 4);

    gtk_box_pack_start(GTK_BOX(box_right_h_b_a), st->bouton_analyser, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_right_h_b_a), label_analyser, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_right_h_b_u), st->bouton_update, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_right_h_b_u), label_update, FALSE, FALSE, 0);

    move_down_in_box(box_right_button, 8);

    gtk_box_pack_start(GTK_BOX(box_right_button), box_right_h_b_a, FALSE, FALSE, 0);
    move_down_in_box(box_right_button, 1);
    gtk_box_pack_start(GTK_BOX(box_right_button), box_right_h_b_u, FALSE, FALSE, 0);
    
    space_in_box(box_scan_bouton, 3);
    gtk_box_pack_start(GTK_BOX(box_scan_bouton), st->bouton_retour, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(box_scan), st->scrolled_text, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_scan), st->progressbar, FALSE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box_scan), box_scan_bouton, FALSE, TRUE, 10);

    gtk_notebook_append_page(GTK_NOTEBOOK(st->notebook), imageClamav, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(st->notebook), box_analyser, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(st->notebook), box_scan, NULL);

    gtk_box_pack_start(GTK_BOX(box_main), box_right_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_main), separateur, TRUE, TRUE, 0);
    gtk_box_pack_end(GTK_BOX(box_main), st->notebook, TRUE, TRUE, 0);

    /* Signal */
    g_signal_connect(G_OBJECT(st->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(st->bouton_retour), "clicked", G_CALLBACK(notebook_set_page_default), st);
    g_signal_connect(G_OBJECT(st->bouton_analyser), "clicked", G_CALLBACK(notebook_set_page_analyser), st);
    g_signal_connect(G_OBJECT(st->bouton_update), "clicked", G_CALLBACK(update_function), st);
    g_signal_connect(G_OBJECT(st->bouton_file), "clicked", G_CALLBACK(selection_file_function), st);
    g_signal_connect(G_OBJECT(st->bouton_folder), "clicked", G_CALLBACK(selection_folder_function), st);
    
    gtk_widget_show_all(st->window);

    gtk_main();
}
