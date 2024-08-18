#ifndef GTK_FONCTION_H_INCLUDED
#define GTK_FONCTION_H_INCLUDED

void selection_file_function(GtkWidget *bouton, st_widgets *st);
void selection_folder_function(GtkWidget *bouton, st_widgets *st);
void update_function(GtkWidget *bouton, st_widgets *st);
void scan_cmd(st_widgets *st);
void move_down_in_box(GtkWidget *box, int nb);
void space_in_box(GtkWidget *box, int nb);
void execute_with_root_privileges(const char *command);
void notebook_set_page_default(GtkWidget *bouton, st_widgets *st);
void notebook_set_page_analyser(GtkWidget *bouton, st_widgets *st);
void add_text_textview(const gchar *text, st_widgets *st);
void clear_textView(GtkWidget *text_view);
void cleanup_progress_bar(GtkWidget *progressBar);
void check_virus_detected(st_widgets *st);
void ActivationButton(st_widgets *st);
void SetSensitiveButton(GtkWidget *widget);

GtkWidget *gtk_button_new_with_image(const char *imageName);
GtkWidget *gtk_label_new_with_markup(const char *text, int color);

void *worker_update(void *user_data);
void *worker_scan(void *user_data);

gchar *selection_repertoire(st_widgets *st);
gchar *selection_fichier(st_widgets *st);

char *textFormated(const char *text);
char *getVirusName(const char *buffer);

int get_virus_data(char *dest, const char *filename);
int check_conf_folder(void);
int check_if_already_running(const char *lock_file);
int create_lock_file(const char *lock_file);
int remove_lock_file(const char *lock_file);

#endif /* !GTK_FONCTION_H_INCLUDED */