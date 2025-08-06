#ifndef GTK_FONCTION_H_INCLUDED
#define GTK_FONCTION_H_INCLUDED

void selection_file_function(GtkWidget *bouton, Application *app);
void selection_folder_function(GtkWidget *bouton, Application *app);
void update_function(GtkWidget *bouton, Application *app);
void scan_cmd(Application *app);
void move_down_in_box(GtkWidget *box, int nb);
void space_in_box(GtkWidget *box, int nb);
void execute_with_root_privileges(const char *command);
void notebook_set_page_default(GtkWidget *bouton, Application *app);
void notebook_set_page_analyser(GtkWidget *bouton, Application *app);
void clear_textView(GtkWidget *text_view);
void cleanup_progress_bar(GtkWidget *progressBar);
void check_virus_detected(Application *app);
void ActivationButton(Application *app);
void SetSensitiveButton(GtkWidget *widget);
void add_text_textview(GtkTextBuffer* buffer, const gchar *text, int color);
void cleanDataValue(char **version, char **db, char **date);

GtkWidget *gtk_button_new_with_icon(const char *iconName, const char *label);
GtkWidget *gtk_label_new_with_markup(const char *text, int color);

void *worker_update(void *user_data);
void *worker_scan(void *user_data);

gchar *selection_repertoire(Application *app);
gchar *selection_fichier(Application *app);

gboolean pulse_progress_bar(GtkWidget *progressBar);
gboolean fraction_progressbar(GtkWidget *progressBar);
gboolean getDataValue(char **out_version, char **out_db, char **out_date);

#endif /* !GTK_FONCTION_H_INCLUDED */