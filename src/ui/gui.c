#include "../../resource.h"
#include "../common.h"
#include "../utils/fonction.h"

void exit_button_clicked(GtkWidget *bouton, Application *app)
{
    g_application_quit(G_APPLICATION(app->application));
    gtk_main_quit();
}

void activateCMD(GtkApplication *application, Application *app)
{
    GtkWidget *box_main = NULL;
    GtkWidget *box_scan = NULL;
    GtkWidget *box_scan_bouton = NULL;
    GtkWidget *box_elements = NULL;
    GtkWidget *box_elements_bouton = NULL;

    /* Window */
    app->window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(app->window), UI_ANALYSE);
    gtk_window_set_position(GTK_WINDOW(app->window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(app->window, WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable (GTK_WINDOW(app->window), FALSE);

    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);
    
    /* Bouton */
    app->bouton_retour   = gtk_button_new_with_label(UI_BT_QUIT);
    gtk_widget_set_name(app->bouton_retour, "bouton_interface");
    
    /* Notebook */
    app->notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(app->notebook), FALSE);

    /* TexView */
    app->scrolled_text = gtk_scrolled_window_new(NULL, NULL);
    app->textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->textview), FALSE);
    gtk_text_view_set_justification(GTK_TEXT_VIEW(app->textview), GTK_JUSTIFY_CENTER);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app->textview), FALSE);

    /* TextBuffer */
    app->textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->textview));
    
    /* ProgressBar */
    app->progressbar = gtk_progress_bar_new();

    /* Elements */
    app->scrolled_analyse = gtk_scrolled_window_new(NULL, NULL);
    app->box_objets = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    /* Set CMD */
    app->cmd_satus = 1;

    /* CSS */ 
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "#bouton_retour {"
        "  font-size: 14px;"
        "  border-radius: 12px;"
        "  color: white;"
        "  padding: 12px 24px;"
        "}", -1, NULL);

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    /* Box */
    box_main                = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_scan                = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_scan_bouton         = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_elements            = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_elements_bouton     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_box_set_homogeneous(GTK_BOX(box_scan_bouton), TRUE);    
    
    gtk_container_add(GTK_CONTAINER(app->window), box_main);
    gtk_container_add(GTK_CONTAINER(app->scrolled_text), app->textview);
    gtk_container_add(GTK_CONTAINER(app->scrolled_analyse), app->box_objets);

    space_in_box(box_scan_bouton, 3);
    gtk_box_pack_start(GTK_BOX(box_scan_bouton), app->bouton_retour, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(box_scan), app->scrolled_text, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_scan), app->progressbar, FALSE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box_scan), box_scan_bouton, FALSE, TRUE, 10);

    gtk_box_pack_start(GTK_BOX(box_elements), app->scrolled_analyse, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_elements), box_elements_bouton, FALSE, TRUE, 10);

    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), box_scan, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), box_elements, NULL);

    gtk_box_pack_end(GTK_BOX(box_main), app->notebook, TRUE, TRUE, 0);

    /* Signal */
    g_signal_connect(G_OBJECT(app->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
    g_signal_connect(G_OBJECT(app->bouton_retour), "clicked", G_CALLBACK(exit_button_clicked), app);
    scan_cmd(app);
    
    gtk_widget_show_all(app->window);

    gtk_main();

}

void activate(GtkApplication *application, Application *app)
{
    GtkWidget *box_main = NULL;
    GtkWidget *box_default = NULL;
    GtkWidget *box_analyser = NULL;
    GtkWidget *box_analyser_btn = NULL;
    GtkWidget *box_right_button = NULL;
    GtkWidget *box_right_button_A = NULL;
    GtkWidget *box_right_button_B = NULL;
    GtkWidget *box_scan = NULL;
    GtkWidget *box_scan_bouton = NULL;
    GtkWidget *box_elements = NULL;
    GtkWidget *box_elements_bouton = NULL;
    GtkWidget *separateur = NULL;
    GtkWidget *iconFolder = NULL;
    GtkWidget *labelCible = NULL;
    GtkWidget *iconClamav = NULL;
    GtkWidget *labelClamav = NULL;
    GtkWidget *labelDB = NULL;
    GtkWidget *labelDate = NULL;
    char *version = NULL;
    gchar *db = NULL;
    gchar *date = NULL;

    /* Window */
    app->window = gtk_application_window_new(application);
    gtk_window_set_title(GTK_WINDOW(app->window), APP_NAME);
    gtk_window_set_position(GTK_WINDOW(app->window), GTK_WIN_POS_CENTER);
    gtk_widget_set_size_request(app->window, WINDOW_WIDTH, WINDOW_HEIGHT);
    gtk_window_set_resizable (GTK_WINDOW(app->window), FALSE);

    g_object_set(gtk_settings_get_default(), "gtk-application-prefer-dark-theme", TRUE, NULL);

    /* Image */
    iconClamav = gtk_image_new_from_icon_name("security-high-symbolic", GTK_ICON_SIZE_MENU);
    iconFolder = gtk_image_new_from_icon_name("folder-new-symbolic", GTK_ICON_SIZE_MENU);
    
    gtk_image_set_pixel_size(GTK_IMAGE(iconFolder), 128);
    gtk_image_set_pixel_size(GTK_IMAGE(iconClamav), 128);

    /* Bouton */
    app->bouton_retour   = gtk_button_new_with_label(UI_BT_RETOUR);
    app->bouton_analyser = gtk_button_new_with_icon("find-location-symbolic", UI_ANALYSER);
    app->bouton_update   = gtk_button_new_with_icon("view-refresh-symbolic", UI_UPDATE);
    app->bouton_file     = gtk_button_new_with_label(UI_SLT_FILE);
    app->bouton_folder   = gtk_button_new_with_label(UI_SLT_FOLDER);

    gtk_widget_set_name(app->bouton_retour, "bouton_retour");
    gtk_widget_set_name(app->bouton_analyser, "bouton_interface");
    gtk_widget_set_name(app->bouton_update, "bouton_interface");
    gtk_widget_set_name(app->bouton_file, "bouton_fichier");
    gtk_widget_set_name(app->bouton_folder, "bouton_fichier");

    GtkSizeGroup *group_1 = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
    gtk_size_group_add_widget(group_1, app->bouton_analyser);
    gtk_size_group_add_widget(group_1, app->bouton_update);

    GtkSizeGroup *group_2 = gtk_size_group_new(GTK_SIZE_GROUP_HORIZONTAL);
    gtk_size_group_add_widget(group_2, app->bouton_file);
    gtk_size_group_add_widget(group_2, app->bouton_folder);

    /* CSS */ 
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider,
        "#bouton_interface {"
        "  font-size: 20px;"
        "  border-radius: 12px;"
        "  color: white;"
        "  padding: 12px 24px;"
        "  font-weight: bold;"
        "}"
        "#bouton_fichier {"
        "  font-size: 14px;"
        "  border-radius: 12px;"
        "  color: white;"
        "  padding: 12px 24px;"
        "}"
        "#bouton_retour {"
        "  font-size: 14px;"
        "  border-radius: 12px;"
        "  color: white;"
        "  padding: 12px 24px;"
        "}", -1, NULL);

    GdkScreen *screen = gdk_screen_get_default();
    gtk_style_context_add_provider_for_screen(screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);

    /* Data */
    getDataValue(&version, &db, &date);

    /* Label */
    labelCible = gtk_label_new_with_markup(UI_SCAN_TXT, 0);
    labelClamav = gtk_label_new_with_markup(version, 4);
    labelDB = gtk_label_new_with_markup(db, 0);
    labelDate = gtk_label_new_with_markup(date, 0);

    /* Cleanup Data */
    cleanDataValue(&version, &db, &date);
    

    /* Notebook */
    app->notebook = gtk_notebook_new();
    gtk_notebook_set_show_tabs(GTK_NOTEBOOK(app->notebook), FALSE);

    /* Seprateur*/
    separateur = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);

    /* TexView */
    app->scrolled_text = gtk_scrolled_window_new(NULL, NULL);
    app->textview = gtk_text_view_new();
    gtk_text_view_set_editable(GTK_TEXT_VIEW(app->textview), FALSE);
    gtk_text_view_set_justification(GTK_TEXT_VIEW(app->textview), GTK_JUSTIFY_CENTER);
    gtk_text_view_set_cursor_visible(GTK_TEXT_VIEW(app->textview), FALSE);

    /* Elements */
    app->scrolled_analyse = gtk_scrolled_window_new(NULL, NULL);
    app->box_objets = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);

    /* TextBuffer */
    app->textBuffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(app->textview));
    
    /* ProgressBar */
    app->progressbar = gtk_progress_bar_new();

    /* Set CMD */
    app->cmd_satus = 0;

    /* Box */
    box_main                = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    
    box_default             = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

    box_right_button        = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    box_right_button_A      = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    box_right_button_B      = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    box_analyser            = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_analyser_btn        = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);

    box_scan                = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_scan_bouton         = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    box_elements            = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    box_elements_bouton     = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);

    gtk_box_set_homogeneous(GTK_BOX(box_scan_bouton), TRUE);
    gtk_box_set_homogeneous(GTK_BOX(box_elements_bouton), TRUE);

    gtk_container_add(GTK_CONTAINER(app->window), box_main);
    gtk_container_add(GTK_CONTAINER(app->scrolled_text), app->textview);
    gtk_container_add(GTK_CONTAINER(app->scrolled_analyse), app->box_objets);

    gtk_widget_set_valign(box_analyser, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(box_analyser_btn, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(box_analyser_btn), app->bouton_file, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_analyser_btn), app->bouton_folder, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_analyser), iconFolder, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box_analyser), labelCible, FALSE, FALSE, 20);
    gtk_box_pack_start(GTK_BOX(box_analyser), box_analyser_btn, FALSE, FALSE, 0);

    gtk_widget_set_valign(box_right_button, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(box_right_button_A, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(box_right_button_B, GTK_ALIGN_CENTER);
    
    space_in_box(box_right_button_A, 8);
    space_in_box(box_right_button_B, 8);
    gtk_box_pack_start(GTK_BOX(box_right_button_A), app->bouton_analyser, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_right_button_B), app->bouton_update, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(box_right_button), box_right_button_A, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_right_button), box_right_button_B, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(box_scan_bouton), app->bouton_retour, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(box_scan), app->scrolled_text, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_scan), app->progressbar, FALSE, TRUE, 10);
    gtk_box_pack_start(GTK_BOX(box_scan), box_scan_bouton, FALSE, TRUE, 10);

    gtk_box_pack_start(GTK_BOX(box_elements), app->scrolled_analyse, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_elements), box_elements_bouton, FALSE, TRUE, 10);

    move_down_in_box(box_default, 2);
    gtk_box_pack_start(GTK_BOX(box_default), iconClamav, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_default), labelClamav, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_default), labelDB, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box_default), labelDate, FALSE, FALSE, 0);

    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), box_default, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), box_analyser, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), box_scan, NULL);
    gtk_notebook_append_page(GTK_NOTEBOOK(app->notebook), box_elements, NULL);

    gtk_box_pack_start(GTK_BOX(box_main), box_right_button, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(box_main), separateur, FALSE, FALSE, 0);
    gtk_box_pack_end(GTK_BOX(box_main), app->notebook, TRUE, TRUE, 0);

    /* Signal */
    g_signal_connect(G_OBJECT(app->bouton_retour), "clicked", G_CALLBACK(notebook_set_page_default), app);
    g_signal_connect(G_OBJECT(app->bouton_analyser), "clicked", G_CALLBACK(notebook_set_page_analyser), app);
    g_signal_connect(G_OBJECT(app->bouton_update), "clicked", G_CALLBACK(update_function), app);
    g_signal_connect(G_OBJECT(app->bouton_file), "clicked", G_CALLBACK(selection_file_function), app);
    g_signal_connect(G_OBJECT(app->bouton_folder), "clicked", G_CALLBACK(selection_folder_function), app);
    
    gtk_widget_show_all(app->window);
}
