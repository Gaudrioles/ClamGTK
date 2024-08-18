#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <sys/stat.h>

#include "main.h"
#include "fonction.h"
#include "stack.h"

struct DispatchData
{
    GtkTextBuffer *buffer;
    gchar *virus_name;
    char *output_str;
    st_widgets *pst;
};

void SetSensitiveButton(GtkWidget *widget)
{
    if(gtk_widget_get_sensitive(widget) == TRUE)
    {
        gtk_widget_set_sensitive(widget, FALSE);
    }
    else
    {
        gtk_widget_set_sensitive(widget, TRUE);
    }
}

void ActivationButton(st_widgets *st)
{
    switch(st->cmd_satus)
    {
        case 0:            
            SetSensitiveButton(st->bouton_retour);
            SetSensitiveButton(st->bouton_analyser);
            SetSensitiveButton(st->bouton_update);
            break;
        
        case 1:
            SetSensitiveButton(st->bouton_retour);
            break;
        default:
            break;
    }

    return;
}

void strcut(char *source, const char *supp) 
{
    if (!*supp || !strstr(source, supp))
    {
        return;
    }    

    size_t len_supp = strlen(supp);
    if (len_supp < 2)
    {
        return;
    }

    char caractereA = supp[0];
    char caractereB = supp[1];
    size_t len_source = strlen(source);

    for (size_t i = 0; i < len_source - 1; i++) {
        if (source[i] == caractereA && source[i + 1] == caractereB)
        {
            source[i] = '\0';
            return;
        }
    }
}


char *getVirusName(const char *buffer)
{
    if (!buffer)
    {
        return NULL;
    }

    const char *result = strstr(buffer, ": ");
    if (!result)
    {
        return NULL;
    }

    result += 2; // Skip past ": "

    // Calculate the length of the virus name
    const char *end = strchr(result, ' ');
    if (!end)
    {
        return NULL;
    }

    size_t name_len = end - result;

    char *virus_name = (char*)malloc(name_len + 1);
    if (!virus_name)
    {
        fprintf(stderr, "Memory allocation error\n");
        return NULL;
    }

    strncpy(virus_name, result, name_len);
    virus_name[name_len] = '\0';

    return virus_name;
}

static gboolean display_status_textbuffer(struct DispatchData *data)
{
    GtkTextIter start, end;
    gchar *output = NULL;
    char *buffer = NULL;

    gtk_text_buffer_get_bounds(data->buffer, &start, &end);

    if (!gtk_text_iter_equal(&start, &end))
    {
        gchar *tampon = gtk_text_buffer_get_text(data->buffer, &start, &end, TRUE);
        if (tampon)
        {
            char *result_OK = strstr(data->output_str, ": OK");/*File: OK*/
            char *result_KO = strstr(data->output_str, "FOUND");/*File: Win.Test.EICAR_HDB-1 FOUND*/

            if (result_OK)
            {
                strcut(data->output_str, ": ");
                output = g_strdup_printf("%s : <span color=\"green\">%s\n</span>", data->output_str, "CLEAN");
            } else if (result_KO)
            {
                buffer = getVirusName(data->output_str);
                if (!buffer)
                {
                    exit(EXIT_FAILURE);
                }

                strcut(data->output_str, ": ");
                output = g_strdup_printf("%s : <span color=\"orange\">WARNING</span> <span color=\"red\">%s\n</span>", data->output_str, buffer);
                data->pst->virusNb++;
                Virus v = add_virus(buffer, data->output_str);
                data->pst->st_virus = push_stack(data->pst->st_virus, v);
                free(buffer);
            } else
            {
                output = g_strdup_printf("<span color=\"white\">%s</span>", data->output_str);
            }

            if (!output)
            {
                exit(EXIT_FAILURE);
            }

            gtk_text_buffer_insert_markup(data->buffer, &end, output, -1);
            g_free(tampon);
            g_free(output);
            g_free(data->output_str);
        }
    }
    else
    {
        gtk_text_buffer_set_text(data->buffer, data->output_str, -1);
        g_free(data->output_str);
    }

    g_free(data);
    return G_SOURCE_REMOVE;
}

char *textFormated(const char *text)
{
    if (!text)
    {
        return NULL;
    }

    size_t len = strlen(text);

    if (len <= 28) 
    {
        return NULL;
    }

    char *buffer = malloc((len - 28 + 1) * sizeof(char));
    if (!buffer)
    {
        fprintf(stderr, "Erreur allocation memoire\n");
        return NULL;
    }

    strcpy(buffer, text + 28);

    return buffer;
}

gboolean pulse_progress_bar(GtkWidget *progressBar)
{
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressBar));
    return G_SOURCE_CONTINUE;
}

void cleanup_progress_bar(GtkWidget *progressBar)
{
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0);
    return;
}

void notebook_set_page_default(GtkWidget *bouton, st_widgets *st)
{
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 0);
}

void notebook_set_page_analyser(GtkWidget *bouton, st_widgets *st)
{
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 1);
}

void clear_textView(GtkWidget *text_view)
{
    GtkTextIter start;
    GtkTextIter end;

    GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    
    if(gtk_text_iter_equal(&start, &end) != TRUE)
    {
        gtk_text_buffer_delete(buffer, &start, &end);
    }
        
    return;
}

GtkWidget *gtk_label_new_with_markup(const char *text, int color)
{
    GtkWidget *label   = NULL;
    gchar *buffer = NULL;

    switch (color)
    {
        case 0:
            buffer = g_strdup_printf("<span color=\"white\" font=\"20\" font_family=\"ubuntu\"><b>%s</b></span>", text);
            break;
        case 1:
            buffer = g_strdup_printf("<span color=\"green\" font=\"20\" font_family=\"ubuntu\"><b>%s</b></span>", text);
            break;
        case 2:
            buffer = g_strdup_printf("<span color=\"red\" font=\"20\" font_family=\"ubuntu\"><b>%s</b></span>", text);
            break;
        case 3:
            buffer = g_strdup_printf("<span color=\"orange\">%s</span>", text);
            break;
        case 4:
            buffer = g_strdup_printf("<span color=\"red\">%s</span>", text);
            break;
        
        default:
            buffer = g_strdup_printf("<span font=\"20\" font_family=\"ubuntu\"><b>%s</b></span>", text);
            break;
    }

    label = gtk_label_new("");
    
    gtk_label_set_markup(GTK_LABEL(label), buffer);

    g_free(buffer);
    
    return label;
}

void add_text_textview(const gchar *text, st_widgets *st)
{
    struct DispatchData *data = g_new0(struct DispatchData, 1);
    
    data->output_str = g_strdup_printf("%s\n", text);
    data->buffer = st->textBuffer;
    data->pst = st;
    
    gdk_threads_add_idle((GSourceFunc)display_status_textbuffer, data);
}

void move_down_in_box(GtkWidget *box, int nb)
{
    for(int compteur = 0; compteur < nb; compteur++)
    {
        GtkWidget *label    = NULL;

        label = gtk_label_new("");
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    }
}

void space_in_box(GtkWidget *box, int nb)
{
    for(int compteur = 0; compteur < nb; compteur++)
    {
        GtkWidget *start  = NULL;
        GtkWidget *end  = NULL;

        start = gtk_label_new("");
        end = gtk_label_new("");
        gtk_box_pack_start(GTK_BOX(box), start, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX(box), end, FALSE, FALSE, 0);
    }
}

GtkWidget *gtk_button_new_with_image(const char *imageName)
{
    GtkWidget *bouton   = NULL;
    GtkWidget *image    = NULL;

    image = gtk_image_new_from_file(imageName);

    bouton = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(bouton), image);
    
    return bouton;
}

void *worker_scan(void *user_data)
{
    st_widgets *st = (st_widgets*)user_data;
    FILE *fichier = NULL;
    gchar *buffer = NULL;
    char output[1024];

    clear_textView(st->textview);
    ActivationButton(user_data);
    
    buffer = g_strdup_printf("clamscan -r \"%s\"", st->scanPath);
    if (!buffer)
    {
        add_text_textview("Allocation mémoire Impossible !!", user_data);
        return NULL;
    }

    if (st->scanPath)
    {
        add_text_textview(MSG_SCAN, user_data);
    }

    g_free(st->scanPath);
    
    fichier = popen(buffer, "r");
    g_free(buffer);

    if (!fichier)
    {
        add_text_textview("ClamScan Erreur", user_data);
        return NULL;
    }

    while (fgets(output, sizeof(output), fichier) != NULL)
    {
        output[strcspn(output, "\n")] = '\0';
        add_text_textview(output, user_data);
    }

    pclose(fichier);

    g_source_remove(st->threadID);
    cleanup_progress_bar(st->progressbar);
    check_virus_detected(st);
    
    return NULL;
}

void *worker_update(void *user_data)
{
    st_widgets *st = (st_widgets*)user_data;
    FILE *fichier = NULL;
    gchar *buffer = NULL;
    char output[1024];
    int compteur = 0;

    clear_textView(st->textview);
    ActivationButton(user_data);
    
    buffer = g_strdup_printf(CMD_FRESHCLAN);
    if (!buffer)
    {
        add_text_textview("Allocation mémoire Impossible !!", user_data);
        return NULL;
    }
    
    fichier = popen(buffer, "r");
    g_free(buffer);
    if (!fichier)
    {
        add_text_textview("freshclam Erreur", user_data);
        return NULL;
    }

    while (fgets(output, sizeof(output), fichier) != NULL)
    {
        
        char *message = NULL;
        
        if(compteur == 0)
        {
            message = strdup(MSG_UPDATE);
        }
        else
        {
            message = textFormated(output);            
        }

        if(!message)
        {
            break;
        } 

        add_text_textview(message, user_data);
        compteur++;

        free(message);
    }

    pclose(fichier);

    g_source_remove(st->threadID);
    cleanup_progress_bar(st->progressbar);
    ActivationButton(user_data);
    
    return NULL;
}

gchar *selection_fichier(st_widgets *st)
{
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    char *buffer = NULL;

    native = gtk_file_chooser_native_new("Selection du fichier", GTK_WINDOW(st->window), action, "_Analyser", "_Retour");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GSList *liste = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(native));

        if (g_list_length((GList*)liste) > 0)
        {
            buffer = strdup((char*)liste->data);
        }
        else
        {
            return NULL;
        }

        if (liste != NULL)
        {
            g_slist_free(liste);
        }
    }

    g_object_unref(native);

    return buffer;
}

gchar *selection_repertoire(st_widgets *st)
{
    GtkFileChooserNative *native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    gint res;

    char *buffer = NULL;

    native = gtk_file_chooser_native_new("Selection du Repertoire", GTK_WINDOW(st->window), action, "_Analyser", "_Retour");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GSList *liste = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(native));

        if (g_list_length((GList*)liste) > 0)
        {
            buffer = strdup((char*)liste->data);
        }
        else
        {
            return NULL;
        }

        if (liste != NULL)
        {
            g_slist_free(liste);
        }
    }

    g_object_unref(native);

    return buffer;
} 
void selection_file_function(GtkWidget *bouton, st_widgets *st)
{
    pthread_t thread;

    st->scanPath = selection_fichier(st);
    if(st->scanPath == NULL)
    {
        return;
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);    
    pthread_create(&thread, NULL, worker_scan, (void*) st);

    return;
}

void selection_folder_function(GtkWidget *button, st_widgets *st)
{
    pthread_t thread;

    st->scanPath = selection_repertoire(st);
    if(st->scanPath == NULL)
    {
        return;
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);
    
    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    pthread_create(&thread, NULL, worker_scan, (void*) st);

    return;
}

void update_function(GtkWidget *bouton,st_widgets *st)
{
    pthread_t thread;

    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);
        
    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    pthread_create(&thread, NULL, worker_update, (void*) st);

    return;
}

void scan_cmd(st_widgets *st)
{
    pthread_t thread;
    
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);
    
    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    pthread_create(&thread, NULL, worker_scan, (void*) st);
    
    return;
}

int check_remove(const char *filePath)
{
    if((remove(filePath)) == 0)
    {
        return 0;
    }
    else
    {
        gchar *tmp = NULL;
        
        if(!(tmp = g_strdup_printf("%s rm \"%s\"", CMD_DEL, filePath)))
        {
            return -1;
        }
        
        if(system(tmp) == -1)
        {
            g_free(tmp);
            return -1;
        }

        g_free(tmp);
    }

    return 0;

}

void supression_fonction(GtkWidget *widget, st_widgets *st)
{
    const gchar *tmp = NULL;
    char buffer[BUFFER_SIZE];

    GtkWidget *parent = gtk_widget_get_parent(widget);
    if(parent)
    {
        if(!(tmp = gtk_widget_get_name(widget)))
        {
            return;
        }
        gtk_widget_destroy(parent);
    }
    
    if((get_virus_data(buffer, tmp)) != 0)
    {
        return;
    }
    
    check_remove(buffer);

    if(st->virusNb != 0)
    {
        st->virusNb--;
    }

    if(st->virusNb == 0)
    {
        ActivationButton(st);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 0);
    }
}

void validation_fonction(GtkWidget *widget, st_widgets *st)
{
    const gchar *tmp = NULL;
    char buffer[BUFFER_SIZE];

    GtkWidget *parent = gtk_widget_get_parent(widget);
    if(parent)
    {
        if(!(tmp = gtk_widget_get_name(widget)))
        {
            return;
        }
        gtk_widget_destroy(parent);
    }
    
    if((get_virus_data(buffer, tmp)) != 0)
    {
        return;
    }

    if(st->virusNb != 0)
    {
        st->virusNb--;
    }

    if(st->virusNb == 0)
    {
        ActivationButton(st);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 0);
    }
}

int store_virus_data(const char *filepath, const char *filename)
{
    gchar *tmp = NULL;
    
    if (!(tmp = g_strdup_printf("%s/%s/TMP_%s.ZxvirusxZ", getenv("HOME"), CLAMGTK_CONF, filename)))
    {
        return -1;
    }

    FILE *fichier = NULL;

    fichier = fopen(tmp, "w+");

    if(!fichier)
    {
        g_free(tmp);
        return -1;
    }

    fprintf(fichier, "%s", filepath);

    fclose(fichier);

    g_free(tmp);

    return 0;
}

int get_virus_data(char *dest, const char *filename)
{
    gchar *tmp = NULL;
    char buffer[BUFFER_SIZE];
    
    if (!(tmp = g_strdup_printf("%s/%s/TMP_%s.ZxvirusxZ", getenv("HOME"), CLAMGTK_CONF, filename)))
    {
        return -1;
    }

    FILE *fichier = NULL;

    fichier = fopen(tmp, "r");

    if(!fichier)
    {
        g_free(tmp);
        return -1;
    }
    
    if(!(fgets(buffer, BUFFER_SIZE, fichier)))
    {
        g_free(tmp);
        fclose(fichier);
        return -1;
    }

    fclose(fichier);
    if((remove(tmp) != 0))
    {
        fprintf(stderr, "Suppression impossible %s\n", tmp);
    }
    g_free(tmp);

    strncpy(dest, buffer, BUFFER_SIZE);

    return 0;
}

void add_virus_elements(st_widgets *st, int nombre)
{
    gchar *base = NULL;
    gchar *tmp = NULL;

    if(!(base = g_path_get_basename(st->st_virus->pv.VirusPath)))
    {
        return;
    }

    if(!(tmp = g_strdup_printf("%d", nombre)))
    {
        g_free(base);
        return;
    }
    

    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label_fichier    = gtk_label_new_with_markup(base, 3);
    GtkWidget *label_virus      = gtk_label_new_with_markup(st->st_virus->pv.virusName, 4);
    
    GtkWidget *button_del   = gtk_button_new_with_image(IMAGE_BT_DEL);
    GtkWidget *button_valid = gtk_button_new_with_image(IMAGE_BT_VALID);

    gtk_widget_set_name(button_del, tmp);
    gtk_widget_set_name(button_valid, tmp);
    if((store_virus_data(st->st_virus->pv.VirusPath, tmp) != 0))
    {
       return;
    }
    
    gtk_box_pack_start(GTK_BOX(box), label_fichier, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label_virus, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), button_del, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), button_valid, FALSE, FALSE, 0);

    gtk_box_pack_start(GTK_BOX(st->box_objets), box, FALSE, FALSE, 0);

    g_signal_connect(button_del, "clicked", G_CALLBACK(supression_fonction), st);
    g_signal_connect(button_valid, "clicked", G_CALLBACK(validation_fonction), st);

    gtk_widget_show_all(box);
    g_free(base);
    g_free(tmp);
}

void add_virus_titre(st_widgets *st)
{
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);
    GtkWidget *label = gtk_label_new_with_markup(MSG_VIRUS, 0);
    
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(st->box_objets), box, FALSE, FALSE, 0);

    gtk_widget_show_all(box);
}

void check_virus_detected(st_widgets *st)
{
    if(st->virusNb == 0)
    {
        ActivationButton(st);
        return;
    }

    int compteur = 0;
    if(st->cmd_satus == 1)
    {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 1);
    }
    else
    {
        gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 3);
    }
    add_virus_titre(st);

    while(compteur != st->virusNb)
    {
        add_virus_elements(st, compteur);
        st->st_virus = pop_stack(st->st_virus);
        compteur++;
    }
}

int directory_exists(const char *path)
{
    struct stat info;

    if (stat(path, &info) != 0)
    {
        return -1;
    }
    else if (info.st_mode & S_IFDIR)
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

int check_conf_folder(void)
{
    gchar *buffer = NULL;

    if (!(buffer = g_strdup_printf("%s/%s", getenv("HOME"), CLAMGTK_CONF)))
    {
        return -1;
    }

    if(directory_exists(buffer) != 0)
    {
        if((mkdir(buffer, 0755)) != 0)
        {
            return -1;
        }
    }

    g_free(buffer);

    return 0;
}

int check_if_already_running(const char *lock_file)
{
    FILE *fichier = NULL;
    gchar *buffer = NULL;

    buffer = g_strdup_printf("%s/%s", getenv("HOME"), lock_file);
    if(!buffer)
    {
        return -1;
    }

    fichier = fopen(buffer, "r");
    if(!fichier)
    {
        g_free(buffer);
        return -1;
    }

    fclose(fichier);
    g_free(buffer);

    return 0;
}

int create_lock_file(const char *lock_file)
{
    FILE *fichier = NULL;
    gchar *buffer = NULL;

    buffer = g_strdup_printf("%s/%s", getenv("HOME"), lock_file);
    if(!buffer)
    {
        return -1;
    }

    fichier = fopen(buffer, "w+");
    if(fichier == NULL)
    {
        g_free(buffer);
        return -1;
    }    

    fprintf(fichier, "Running");
    fclose(fichier);
    g_free(buffer);

    return 0;
}

int remove_lock_file(const char *lock_file)
{
    if(check_if_already_running(lock_file) != 0)
    {
        return -1;
    }
    gchar *buffer = NULL;

    buffer = g_strdup_printf("%s/%s", getenv("HOME"), lock_file);
    if(!buffer)
    {
        return -1;
    }

    if((remove(buffer)) != 0)
    {
        g_free(buffer);
        return -1;
    }
    g_free(buffer);

    return 0;
}