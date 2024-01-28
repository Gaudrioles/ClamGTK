#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "main.h"
#include "fonction.h"

struct DispatchData {
    GtkTextBuffer *buffer;
    char *output_str;
};

static gboolean display_status_textbuffer(struct DispatchData *data)
{
    gchar* output = NULL;
    GtkTextIter start;
    GtkTextIter end;

    gtk_text_buffer_get_bounds(data->buffer, &start, &end);
    
    if(gtk_text_iter_equal(&start, &end) != TRUE)
    {
        gchar* tampon = gtk_text_buffer_get_text(data->buffer, &start, &end, TRUE);
        if(tampon != NULL)
        {
            output = g_strdup_printf("%s%s", tampon, data->output_str);
            if(output == NULL)
            {
                g_free(tampon);
                g_free(data);
                return G_SOURCE_REMOVE;
            }
        }

        gtk_text_buffer_set_text(data->buffer, output, strlen(output));
        g_free(tampon);
        g_free(output);
    }
    else
    {
        gtk_text_buffer_set_text(data->buffer, data->output_str, strlen(data->output_str));
    }

    g_free(data);
    return G_SOURCE_REMOVE;
}

char* textFormated(const char* text)
{
    int compteur = 28;
    if(text == NULL)
    {
        return NULL;
    }

    char* buffer = NULL;

    size_t len = strlen(text);

    buffer = malloc(len * sizeof(char));
    if(buffer == NULL)
    {
        fprintf(stderr, "Erreur allocation memoire\n");
        return NULL;
    }

    for(int i = 0; i < (int)len; i++)
    {
        buffer[i] = text[compteur];
        compteur++;
    }

    return buffer;
}

gboolean pulse_progress_bar(GtkWidget* progressBar)
{
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressBar));
    return G_SOURCE_CONTINUE;
}

void cleanup_progress_bar(GtkWidget* progressBar)
{
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), 0.0);
    return;
}

void notebook_set_page_default(GtkWidget* bouton, gpointer user_data)
{
    UNUSED(bouton);
    st_widgets* st = (st_widgets*) user_data;
    if(st->lock == TRUE)
    {
        return;
    }
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 0);
}

void notebook_set_page_analyser(GtkWidget* bouton, gpointer user_data)
{
    UNUSED(bouton);
    st_widgets* st = (st_widgets*) user_data;
    if(st->lock == TRUE)
    {
        return;
    }
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 1);
}

void clear_textView(GtkWidget* text_view)
{
    GtkTextIter start;
    GtkTextIter end;

    GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(text_view));
    
    gtk_text_buffer_get_start_iter(buffer, &start);
    gtk_text_buffer_get_end_iter(buffer, &end);
    
    if(gtk_text_iter_equal(&start, &end) != TRUE)
    {
        gtk_text_buffer_delete(buffer, &start, &end);
    }
        
    return;
}

GtkWidget* gtk_label_new_with_markup(const char* text, int color)
{
    GtkWidget* label   = NULL;
    gchar* buffer = NULL;

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

void add_text_textview(const gchar* text, gpointer user_data)
{
    st_widgets* st = (st_widgets*) user_data;
    struct DispatchData *data = g_new0(struct DispatchData, 1);
    data->output_str = g_strdup_printf("%s\n", text);
    data->buffer = st->TextBuffer;
    gdk_threads_add_idle((GSourceFunc)display_status_textbuffer, data);
}

void move_down_in_box(GtkWidget* box, int nb)
{
    for(int compteur = 0; compteur < nb; compteur++)
    {
        GtkWidget* label    = NULL;

        label = gtk_label_new("");
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);
    }
}

void space_in_box(GtkWidget* box, int nb)
{
    for(int compteur = 0; compteur < nb; compteur++)
    {
        GtkWidget* start  = NULL;
        GtkWidget* end  = NULL;

        start = gtk_label_new("");
        end = gtk_label_new("");
        gtk_box_pack_start(GTK_BOX(box), start, FALSE, FALSE, 0);
        gtk_box_pack_end(GTK_BOX(box), end, FALSE, FALSE, 0);
    }
}

GtkWidget* gtk_button_new_with_image(const char* imageName)
{
    GtkWidget* bouton   = NULL;
    GtkWidget* image    = NULL;

    image = gtk_image_new_from_file(imageName);

    bouton = gtk_button_new();
    gtk_button_set_image(GTK_BUTTON(bouton), image);
    
    return bouton;
}

void* worker_scan(void* user_data)
{
    st_widgets* st = (st_widgets*)user_data;
    FILE* fichier = NULL;
    gchar* buffer = NULL;
    char output[1024];

    clear_textView(st->textview);
    
    buffer = g_strdup_printf("clamscan \"%s\"", st->scanPath);
    if (buffer == NULL)
    {
        add_text_textview("Allocation mémoire Impossible !!", user_data);
        return NULL;
    }

    if (st->scanPath != NULL)
    {
        add_text_textview(st->scanPath, user_data);        
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
    st->lock = FALSE;
    
    return NULL;
}

void* worker_update(void* user_data)
{
    st_widgets* st = (st_widgets*)user_data;
    FILE* fichier = NULL;
    gchar* buffer = NULL;
    char output[1024];

    clear_textView(st->textview);
    
    buffer = g_strdup_printf("pkexec bash %s", CLAMGTK_SCRIPT);
    if (buffer == NULL)
    {
        add_text_textview("Allocation mémoire Impossible !!", user_data);
        return NULL;
    }
    
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

        char* buffer = textFormated(output);
        if(buffer == NULL)
        {
            break;
        }
        
        add_text_textview(buffer, user_data);

        free(buffer);
    }

    pclose(fichier);

    g_source_remove(st->threadID);
    cleanup_progress_bar(st->progressbar);
    st->lock = FALSE;
    
    return NULL;
}

gchar* selection_fichier(st_widgets* st)
{
    GtkFileChooserNative* native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_OPEN;
    gint res;

    char* buffer = NULL;

    native = gtk_file_chooser_native_new("Selection du fichier", GTK_WINDOW(st->window), action, "_Analyser", "_Retour");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GSList* liste = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(native));

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

gchar* selection_repertoire(st_widgets* st)
{
    GtkFileChooserNative* native;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER;
    gint res;

    char* buffer = NULL;

    native = gtk_file_chooser_native_new("Selection du Repertoire", GTK_WINDOW(st->window), action, "_Analyser", "_Retour");

    res = gtk_native_dialog_run(GTK_NATIVE_DIALOG(native));
    if (res == GTK_RESPONSE_ACCEPT)
    {
        GSList* liste = gtk_file_chooser_get_filenames(GTK_FILE_CHOOSER(native));

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
void selection_file_function(GtkWidget* button, gpointer user_data)
{
    UNUSED(button);
    st_widgets* st = (st_widgets*) user_data;
    pthread_t thread;

    if(st->lock == TRUE)
    {
        return;
    }

    st->scanPath = selection_fichier(user_data);
    if(st->scanPath == NULL)
    {
        return;
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    st->lock = TRUE;

    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);    
    pthread_create(&thread, NULL, worker_scan, (void*) user_data);

    return;
}

void selection_folder_function(GtkWidget* button, gpointer user_data)
{
    UNUSED(button);
    st_widgets* st = (st_widgets*) user_data;
    pthread_t thread;

    if(st->lock == TRUE)
    {
        return;
    }

    st->scanPath = selection_repertoire(user_data);
    if(st->scanPath == NULL)
    {
        return;
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    st->lock = TRUE;
    
    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    pthread_create(&thread, NULL, worker_scan, (void*) user_data);

    return;
}

void update_function(GtkWidget* button, gpointer user_data)
{
    UNUSED(button);
    st_widgets* st = (st_widgets*) user_data;
    pthread_t thread;

    if(st->lock == TRUE)
    {
        return;
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    st->lock = TRUE;
    
    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    pthread_create(&thread, NULL, worker_update, (void*) user_data);

    return;
}

void scan_cmd(gpointer user_data)
{
    st_widgets* st = (st_widgets*) user_data;
    pthread_t thread;

    if(st->lock == TRUE)
    {
        return;
    }

    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    st->lock = TRUE;
    
    st->threadID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    pthread_create(&thread, NULL, worker_scan, (void*) user_data);
    
    return;
}
