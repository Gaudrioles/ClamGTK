#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <ctype.h>

#include "main.h"
#include "fonction.h"

struct DispatchData
{
    GtkTextBuffer *buffer;
    gchar *virus_name;
    char *output_str;
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
        case 1:
            buffer = g_strdup_printf("<span color=\"green\" font=\"20\" font_family=\"ubuntu\"><b>%s</b></span>", text);
            break;
        case 2:
            buffer = g_strdup_printf("<span color=\"red\" font=\"20\" font_family=\"ubuntu\"><b>%s</b></span>", text);
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

    if (st->scanPath != NULL)
    {
        add_text_textview(MSG_SCAN, user_data);
    }

    g_free(st->scanPath);
    
    fichier = popen(buffer, "r");
    if (fichier == NULL)
    {
        add_text_textview("ClamScan Erreur", user_data);
        g_free(buffer);
        return NULL;
    }

    g_free(buffer);

    while (fgets(output, sizeof(output), fichier) != NULL)
    {
        size_t len = strlen(output) - 1;
        output[len] = '\0';
        add_text_textview(output, user_data);
    }

    pclose(fichier);

    g_source_remove(st->threadID);
    cleanup_progress_bar(st->progressbar);

    ActivationButton(user_data);
    
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
    if (buffer == NULL)
    {
        add_text_textview("Allocation mémoire Impossible !!", user_data);
        return NULL;
    }
    
    fichier = popen(buffer, "r");
    if (fichier == NULL)
    {
        add_text_textview("freshclam Erreur", user_data);
        g_free(buffer);
        return NULL;
    }

    g_free(buffer);

    while (fgets(output, sizeof(output), fichier) != NULL)
    {
        
        char *zbuffer = NULL;
        
        if(compteur == 0)
        {
            zbuffer = strdup(MSG_UPDATE);
        }
        else
        {
            zbuffer = textFormated(output);            
        }

        if(!zbuffer)
        {
            break;
        } 

        add_text_textview(zbuffer, user_data);
        compteur++;

        free(zbuffer);
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
