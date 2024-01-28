#ifndef GTK_FONCTION_H_INCLUDED
#define GTK_FONCTION_H_INCLUDED

void selection_file_function(GtkWidget* button, gpointer user_data);
void selection_folder_function(GtkWidget* button, gpointer user_data);
void update_function(GtkWidget* button, gpointer user_data);
void scan_cmd(gpointer user_data);
void move_down_in_box(GtkWidget* box, int nb);
void space_in_box(GtkWidget* box, int nb);
void execute_with_root_privileges(const char *command);
void notebook_set_page_default(GtkWidget* bouton, gpointer user_data);
void notebook_set_page_analyser(GtkWidget* bouton, gpointer user_data);
void add_text_textview(const gchar* text, gpointer user_data);
void clear_textView(GtkWidget* text_view);
void cleanup_progress_bar(GtkWidget* progressBar);

GtkWidget* gtk_button_new_with_image(const char *imageName);
GtkWidget* gtk_label_new_with_markup(const char *text, int color);

void* worker_update(void* user_data);
void* worker_scan(void* user_data);
gpointer worker_pulse(gpointer user_data);

gchar* selection_repertoire(st_widgets* st);
gchar* selection_fichier(st_widgets* st);

char* textFormated(const char* text);

#endif /* !GTK_FONCTION_H_INCLUDED */