#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>
#include <pthread.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "main.h"
#include "fonction.h"
#include "stack.h"
#include "tool.h"
#include "strtool.h"

double fraction = 0.0;
int virusDetection = 0;

typedef struct DispatchData
{
    GtkTextBuffer *buffer;
    char *output_str;
    int color;
} DispatchData;

typedef struct DispatchDataVirus
{
    GtkTextBuffer *buffer;
    Stack scanItem;
} DispatchDataVirus;

typedef struct DispatchDataUpdate
{
    GtkTextBuffer *buffer;
    char *database;
    char *sigs;
    char *version;
    gboolean upToDate;
} DispatchDataUpdate;

void CleanupThread(guint *source_id)
{
    if (source_id && *source_id > 0)
    {
        g_source_remove(*source_id);
        *source_id = 0;
    }
}

void SetSensitiveButton(GtkWidget *widget)
{
    gtk_widget_set_sensitive(widget, !gtk_widget_get_sensitive(widget));
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

void initAllDone(st_widgets *st)
{
	st->compileDone = FALSE;
    st->scanDone    = FALSE;
}

gboolean virus_traitement(st_widgets *st)
{
    if (virusDetection == -1)
    {
        /* Déverrouiller les boutons */
        ActivationButton(st);

        /* Changement de la page */
        gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), (st->cmd_satus == 1) ? 0 : 2);
        
        /* Nettoyage de la pile */
        st->scanItem = clear_stack(st->scanItem);
        virusDetection = 0;
        return G_SOURCE_REMOVE;
    }
    
    return G_SOURCE_CONTINUE;
}

void *worker_completion(void *user_data)
{
    st_widgets *st = (st_widgets *)user_data;
    
    while(!st->scanDone)
    {
        g_usleep(100000);
    }

    /* Suppression du thread progressbar */
    CleanupThread(&st->thread_Progress_ID);

    /* Temporisation */
    g_usleep(2000000);

    check_virus_detected(st);

    guint thread_ID = g_timeout_add(100, (GSourceFunc)virus_traitement, st);
    if (thread_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return NULL;
    }
    
    return NULL;
}

gboolean check_thread_scan(st_widgets *st)
{
    if(st->compileDone)
    {
        /* Suppression du thread pulse progressbar */
        CleanupThread(&st->thread_Pulse_ID);
        
        /* Création du thread fraction progressbar */
        st->thread_Progress_ID = g_timeout_add(100, (GSourceFunc)fraction_progressbar, st->progressbar);
        if (st->thread_Progress_ID == 0)
        {
            g_error("Erreur lors de la création de la thread\n");
            return G_SOURCE_REMOVE;
        }
        
        return G_SOURCE_REMOVE;
    }

    return G_SOURCE_CONTINUE;
}

static gboolean display_status_textbuffer(DispatchData *data)
{
    GtkTextIter end;
    gchar *buffer = NULL;

    gtk_text_buffer_get_end_iter(data->buffer, &end);

    switch (data->color)
    {
        case 1:
            buffer = g_strdup_printf("<span foreground=\"green\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", data->output_str);
            break;
        case 2:
            buffer = g_strdup_printf("<span foreground=\"orange\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", data->output_str);
            break;
        case 3:
            buffer = g_strdup_printf("<span foreground=\"red\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", data->output_str);
            break;
        default:
            buffer = g_strdup_printf("<span foreground=\"white\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", data->output_str);
            break;
    }
    
    if (!buffer)
    {
        g_error("Allocation mémoire impossible");
        g_free(data);
        return G_SOURCE_REMOVE;
    }

    gtk_text_buffer_insert_markup(data->buffer, &end, buffer, -1);

    g_free(buffer);
    g_free(data);

    return G_SOURCE_REMOVE;
}

static gboolean display_scanItem(DispatchDataVirus *data)
{
    GtkTextIter end;
    gchar *buffer = NULL;

    /* Check DATA */
    if (!data || !data->scanItem || !data->buffer)
    {
        g_warning("Invalid data display_scanItem");
        return G_SOURCE_REMOVE;
    }

    /* Get iter position */
    gtk_text_buffer_get_end_iter(data->buffer, &end);

    /* Markup */
    if (data->scanItem->item.isClean)
    {
        buffer = g_strdup_printf("<span foreground=\"white\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>\t :" "<span foreground=\"green\" font_size=\"medium\" font_family=\"Ubuntu\"><b>CLEAN\n</b></span>",data->scanItem->item.fileName);
    }
    else
    {
        buffer = g_strdup_printf("<span foreground=\"white\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>\t :" "<span foreground=\"red\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s\n</b></span>", data->scanItem->item.fileName, data->scanItem->item.virusName);
    }

    if (!buffer)
    {
        g_error("Allocation mémoire impossible");
        g_free(data);
        return G_SOURCE_REMOVE;
    }

    /* Insert Text */
    gtk_text_buffer_insert_markup(data->buffer, &end, buffer, -1);

    /* Cleanup */
    g_free(buffer);
    g_free(data);

    return G_SOURCE_REMOVE;
}

static gboolean display_updateItem(DispatchDataUpdate *data)
{
    GtkTextIter end;
    gchar *buffer = NULL;

    /* Check DATA */
    if (!data || !data->database || !data->sigs || !data->version)
    {
        g_warning("Invalid data display_scanItem");
        return G_SOURCE_REMOVE;
    }
    
    /* Get iter position */
    gtk_text_buffer_get_end_iter(data->buffer, &end);

    /* Markup */
    if (data->upToDate)
    {
        buffer = g_strdup_printf("<span foreground=\"white\" font_size=\"medium\" font_family=\"Ubuntu\"><b>DataBase :%-25s  Signature :%-25s</b></span>\t :" "<span foreground=\"green\" font_size=\"medium\" font_family=\"Ubuntu\"><b>Version :%-25s\n</b></span>", data->database, data->sigs, data->version);
    }
    else
    {
        buffer = g_strdup_printf("<span foreground=\"white\" font_size=\"medium\" font_family=\"Ubuntu\"><b>DataBase :%-25s  Signature :%-25s</b></span>\t :" "<span foreground=\"orange\" font_size=\"medium\" font_family=\"Ubuntu\"><b>Version :%-25s\n</b></span>", data->database, data->sigs, data->version);
    }

    if (!buffer)
    {
        g_error("Allocation mémoire impossible");
        g_free(data);
        return G_SOURCE_REMOVE;
    }

    /* Insert Text */
    gtk_text_buffer_insert_markup(data->buffer, &end, buffer, -1);

    /* Cleanup */
    g_free(buffer);
    free(data->database);
    free(data->sigs);
    free(data->version);
    g_free(data);

    return G_SOURCE_REMOVE;
}

gboolean pulse_progress_bar(GtkWidget *progressBar)
{
    gtk_progress_bar_pulse(GTK_PROGRESS_BAR(progressBar));
    return G_SOURCE_CONTINUE;
}

gboolean fraction_progressbar(GtkWidget *progressBar)
{
    if (fraction <= 1.0)
    {
        gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), fraction);
    }
    
    return G_SOURCE_CONTINUE;
}

void cleanup_progress_bar(GtkWidget *progressBar)
{
    fraction = 0.0;
    gtk_progress_bar_set_fraction(GTK_PROGRESS_BAR(progressBar), fraction);
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
    if (text == NULL)
    {
        g_warning("Text argument is NULL, returning empty label.");
        return gtk_label_new("");
    }

    GtkWidget *label = NULL;
    gchar *buffer = NULL;

    /* Markup */
    switch (color)
    {
        case 1:
            buffer = g_strdup_printf("<span foreground=\"green\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", text);
            break;
        case 2:
            buffer = g_strdup_printf("<span foreground=\"orange\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", text);
            break;
        case 3:
            buffer = g_strdup_printf("<span foreground=\"red\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", text);
            break;
        case 4:
            buffer = g_strdup_printf("<span foreground=\"white\" font=\"20\" font_family=\"Ubuntu\"><b>%s</b></span>", text);
            break;
        default:
            buffer = g_strdup_printf("<span foreground=\"white\" font_size=\"medium\" font_family=\"Ubuntu\"><b>%s</b></span>", text);
            break;
    }

    /* Label */
    label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(label), buffer);

    /* Cleanup */
    g_free(buffer);

    return label;
}

void add_text_textview(GtkTextBuffer* buffer, const gchar *text, int color)
{
    if(text)
    {
        DispatchData *data = g_new0(DispatchData, 1);
    
        data->output_str = g_strdup_printf("%s\n", text);
        data->buffer = buffer;
        data->color = color;
        
        gdk_threads_add_idle((GSourceFunc)display_status_textbuffer, data);
    }
}

void add_text_textviewVirus(GtkTextBuffer* buffer, StackElement *scanItem)
{
    if(scanItem->item.fileName)
    {
        DispatchDataVirus *data = g_new0(DispatchDataVirus, 1);
    
        data->scanItem = scanItem;
        data->buffer = buffer;
        
        gdk_threads_add_idle((GSourceFunc)display_scanItem, data);
    }
}

void add_text_textviewUpdate(GtkTextBuffer* buffer, ExtractedData *updateItem)
{
    if(updateItem)
    {
        DispatchDataUpdate *data = g_new0(DispatchDataUpdate, 1);
        
        data->buffer = buffer;
        data->database = strdup(updateItem->database);
        data->sigs = strdup(updateItem->sigs);
        data->version = strdup(updateItem->version);
        data->upToDate = updateItem->upToDate;
        
        gdk_threads_add_idle((GSourceFunc)display_updateItem, data);
    }
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
    st_widgets *st = (st_widgets *)user_data;
    gchar *command = NULL;
    FILE *fichier = NULL;
    char output[1024];
    int compteur = 1;
    int NombreElement = 0;

    /* Compter nombre d'éléments dans le répertoire */
    compteRepertoire(st->scanPath, &NombreElement);
    
    /* Construction clamscan commande */
    command = g_strdup_printf("clamscan --stdout -r \"%s\"", st->scanPath);
    if (!command)
    {
        add_text_textview(st->textBuffer, "Allocation mémoire impossible", 0);
        return NULL;
    }

    /* Nettoyage scanPath */
    if (st->scanPath)
    {
        g_free(st->scanPath);
        st->scanPath = NULL;
    }

    /* Affichage du message */
    add_text_textview(st->textBuffer, MSG_COMPILE, 0);

    /* Execution clamscan commande */ 
    fichier = popen(command, "r");
    g_free(command);
    if (!fichier)
    {
        add_text_textview(st->textBuffer, "Erreur lors de l'exécution de ClamScan", 0);
        return NULL;
    }
    
    /* Process output ligne par ligne */
    while (fgets(output, sizeof(output), fichier) != NULL)
    {
        output[strcspn(output, "\n")] = '\0';

        if (compteur <= NombreElement)
        {
            if(!st->compileDone)
            {
                /* Affichage du message */
                add_text_textview(st->textBuffer, MSG_SCAN, 0);
                st->compileDone = TRUE;
            }

            char *filePath = getFilePath(output);
            if (filePath)
            {
                gchar *fileName = getFileName(filePath);
                if (fileName)
                {
                    gboolean isClean = checkIfClean(output);
                    char *virusName = isClean ? NULL : getVirusName(output);
                    st->scanItem = push_stack(st->scanItem, fileName, filePath, virusName, isClean);

                    /* Update UI with the scan result */
                    add_text_textviewVirus(st->textBuffer, st->scanItem);

                    g_free(fileName);
                    if (virusName)
                    {
                        free(virusName);
                    }
                }
                free(filePath);
            }
        }
        else
        {
            /* Display other output */
            add_text_textview(st->textBuffer, output, 0);
        }

        /* Update the progress bar */
        if (NombreElement > 0)
        {
            fraction = (double)compteur / NombreElement;
        }

        compteur++;
    }

    /* Fermer le processus */
    pclose(fichier);

    /* Affichage du message */
    add_text_textview(st->textBuffer, MSG_SCAN_F, 0);

    /* Clean up */
    st->scanDone = TRUE;
    
    return NULL;
}

void *worker_update(void *user_data)
{
    int regexSelect = -1;
    st_widgets *st = (st_widgets *)user_data;
    
    /* Process Update */
    FILE *fichier = popen(CMD_FRESHCLAN, "r");
    if (!fichier)
    {
        add_text_textview(st->textBuffer, "Erreur lors de l'exécution de freshclam !", 0);
        return NULL;
    }

    /* Affichage du message */
    add_text_textview(st->textBuffer, MSG_UPDATE, 0);

    char output[1024];
    while (fgets(output, sizeof(output), fichier) != NULL)
    {
        /* Chercher dans le texte */
        if(strstr(output, UPDATE_OK ))
        {
            regexSelect = 0;
        }
        else if(strstr(output, UPDATE_KO))
        {
            regexSelect = 1;
        }
        else
        {
            continue;
        }

        ExtractedData result;

        if (extractData(output, regexSelect, &result))
        {
            /* Affichage du texte */
            add_text_textviewUpdate(st->textBuffer, &result);
        }
    }

    /* Fermer le processus */
    pclose(fichier);

    /* Affichage du message */
    add_text_textview(st->textBuffer, MSG_UPDATE_F, 0);

    /* Cleanup */
    CleanupThread(&st->thread_Pulse_ID);
    cleanup_progress_bar(st->progressbar);
    ActivationButton(st);

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
    /* Récupération du Fichier Path */
    st->scanPath = selection_fichier(st);
    if(!st->scanPath)
    {
        return;
    }
    
    /* Changement de la page */
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    /* Initialisation */
    initAllDone(st);

    /* Preparation UI */
    cleanup_progress_bar(st->progressbar);
    clear_textView(st->textview);
    ActivationButton(st);
    
    /* Lancement du thread pulse */
    st->thread_Pulse_ID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    if (st->thread_Pulse_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }

    /* Lancement du thread worker_scan*/
    GThread *thread_scan = g_thread_new("worker-scan", (GThreadFunc)worker_scan, (void *)st);
    if(!thread_scan)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    /* Lancement du thread check_thread_scan */
    st->thread_Scan_ID = g_timeout_add(100, (GSourceFunc)check_thread_scan, st);
    if (st->thread_Scan_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    /* Lancement du thread check_thread_completion */
    GThread *thread_completion = g_thread_new("worker_completion", (GThreadFunc)worker_completion, (void *)st);
    if(!thread_completion)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    return;
}

void selection_folder_function(GtkWidget *button, st_widgets *st)
{
    /* Récupération du Fichier Path */
    st->scanPath = selection_repertoire(st);
    if(!st->scanPath)
    {
        return;
    }

    /* Changement de la page */
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    /* Initialisation */
    initAllDone(st);

    /* Preparation UI */
    cleanup_progress_bar(st->progressbar);
    clear_textView(st->textview);
    ActivationButton(st);
    
    /* Lancement du thread pulse */
    st->thread_Pulse_ID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    if (st->thread_Pulse_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }

    /* Lancement du thread worker_scan*/
    GThread *thread_scan = g_thread_new("worker-scan", (GThreadFunc)worker_scan, (void *)st);
    if(!thread_scan)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    /* Lancement du thread check_thread_scan */
    st->thread_Scan_ID = g_timeout_add(100, (GSourceFunc)check_thread_scan, st);
    if (st->thread_Scan_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    /* Lancement du thread check_thread_completion */
    GThread *thread_completion = g_thread_new("worker_completion", (GThreadFunc)worker_completion, (void *)st);
    if(!thread_completion)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    return;
}

void update_function(GtkWidget *bouton, st_widgets *st)
{
    /* Changement de la page */
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    /* Initialisation */
    initAllDone(st);

    /* Preparation UI */
    cleanup_progress_bar(st->progressbar);
    clear_textView(st->textview);
    ActivationButton(st);

     /* Lancement du thread pulse */
    st->thread_Pulse_ID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    if (st->thread_Pulse_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }

    /* Lancement du thread worker_update*/
    GThread *thread_update = g_thread_new("worker-update", (GThreadFunc)worker_update, (void *)st);
    if(!thread_update)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }

    return;
}

void scan_cmd(st_widgets *st)
{
    if(!st->scanPath)
    {
        return;
    }

    /* Changement de la page */
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), 2);

    /* Initialisation */
    initAllDone(st);

    /* Preparation UI */
    cleanup_progress_bar(st->progressbar);
    clear_textView(st->textview);
    ActivationButton(st);
    
    /* Lancement du thread pulse */
    st->thread_Pulse_ID = g_timeout_add(100, (GSourceFunc)pulse_progress_bar, st->progressbar);
    if (st->thread_Pulse_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }

    /* Lancement du thread worker_scan*/
    GThread *thread_scan = g_thread_new("worker-scan", (GThreadFunc)worker_scan, (void *)st);
    if(!thread_scan)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    /* Lancement du thread check_thread_scan */
    st->thread_Scan_ID = g_timeout_add(100, (GSourceFunc)check_thread_scan, st);
    if (st->thread_Scan_ID == 0)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    /* Lancement du thread check_thread_completion */
    GThread *thread_completion = g_thread_new("worker_completion", (GThreadFunc)worker_completion, (void *)st);
    if(!thread_completion)
    {
        g_error("Erreur lors de la création de la thread\n");
        return;
    }
    
    return;
}

gboolean removeVirus(const gchar *filepath)
{
    if (!filepath)
    {
        return FALSE;
    }

    if (!g_file_test(filepath, G_FILE_TEST_EXISTS))
    {
        return FALSE;
    }

    if (remove(filepath) == 0)
    {
        return TRUE;
    }
    else
    {
        /* Tentative de suppression avec une commande */
        gchar *cmd = g_strdup_printf("%s \"%s\"", CMD_DEL, filepath);
        if (!cmd)
        {
            g_critical("Échec d'allocation mémoire pour la commande de suppression.");
            return FALSE;
        }

        int ret = system(cmd);
        g_free(cmd);

        if (ret != 0)
        {
            g_warning("Échec de suppression du fichier '%s' avec la commande.", filepath);
            return FALSE;
        }
    }

    return TRUE;
}

VirusData* createVirusData(const char* virusName, const char* filepath, const char* fileName, GtkWidget *virusTitre)
{
    VirusData* data = (VirusData*)malloc(sizeof(VirusData));
    if (data == NULL)
    {
        g_error("Allocation mémoire impossible");
        return NULL;
    }

    /* Initialisation des champs avec strncpy pour éviter les débordements */ 
    strncpy(data->virusName, virusName, BUFFER_SIZE - 1);
    data->virusName[BUFFER_SIZE - 1] = '\0';

    strncpy(data->filepath, filepath, BUFFER_SIZE - 1);
    data->filepath[BUFFER_SIZE - 1] = '\0';

    strncpy(data->fileName, fileName, BUFFER_SIZE - 1);
    data->fileName[BUFFER_SIZE - 1] = '\0';

    data->virusTitre = virusTitre;

    return data;
}

void freeVirusData(VirusData* data)
{
    free(data);
}

void supression_fonction(GtkWidget *widget, VirusData *virus)
{
    if(!removeVirus(virus->filepath))
    {
        g_error("Suppression Impossible %s\n", virus->filepath);
    }
    
    GtkWidget *parent = gtk_widget_get_parent(widget);
    if(parent)
    {
        gtk_widget_destroy(parent);
    }

    virusDetection--;
    if(virusDetection == 0)
    {
        gtk_widget_destroy(virus->virusTitre);
        virusDetection = -1;
    }

    freeVirusData(virus);
}

void validation_fonction(GtkWidget *widget, VirusData *virus)
{
    GtkWidget *parent = gtk_widget_get_parent(widget);
    if(parent)
    {
        gtk_widget_destroy(parent);
    }

    virusDetection--;
    if(virusDetection == 0)
    {
        gtk_widget_destroy(virus->virusTitre);
        virusDetection = -1;
    }

    freeVirusData(virus);
}

GtkWidget *createVirusTitre(void)
{
    /* Box */
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    /* Label */
    GtkWidget *label = gtk_label_new_with_markup(MSG_VIRUS, 0);

    /* Add in Box */
    gtk_box_pack_start(GTK_BOX(box), label, TRUE, FALSE, 0);

    /* Show */
    gtk_widget_show_all(box);
    
    return box;
}

void add_virus_elements(st_widgets *st, int nombre, VirusData *virus)
{
    gchar *indexStr = g_strdup_printf("%d", nombre);
    if (!indexStr) {
        g_error("Allocation mémoire impossible");
        return;
    }

    /* Box */
    GtkWidget *box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 5);

    /* label */
    GtkWidget *label_fichier    = gtk_label_new_with_markup(virus->fileName, 2);
    GtkWidget *label_virus      = gtk_label_new_with_markup(virus->virusName, 3);

    /* Button */
    GtkWidget *button_del = gtk_button_new_with_image(IMAGE_BT_DEL);
    GtkWidget *button_valid = gtk_button_new_with_image(IMAGE_BT_VALID);
    
    /* Name */
    gtk_widget_set_name(button_del, indexStr);
    gtk_widget_set_name(button_valid, indexStr);

    /* Add in box */
    gtk_box_pack_start(GTK_BOX(box), label_fichier, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), label_virus, TRUE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), button_del, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(box), button_valid, FALSE, FALSE, 0);

    /* Add Title */
    if(nombre == 0)
    {
        gtk_box_pack_start(GTK_BOX(st->box_objets), virus->virusTitre, FALSE, FALSE, 0);
    }
    gtk_box_pack_start(GTK_BOX(st->box_objets), box, FALSE, FALSE, 0);

    /* Signals */
    g_signal_connect(button_del, "clicked", G_CALLBACK(supression_fonction), virus);
    g_signal_connect(button_valid, "clicked", G_CALLBACK(validation_fonction), virus);

    /* Show */
    gtk_widget_show_all(box);

    /* Cleanup */
    g_free(indexStr);
}

void check_virus_detected(st_widgets *st)
{
    if (is_empty_stack(st->scanItem))
    {
        return;
    }
    
    /* Title */
    GtkWidget *virusTitre = createVirusTitre();
    
    StackElement *current = st->scanItem;
    
    while (current)
    {
        if (!current->item.isClean)
        {
            VirusData* virus = createVirusData(current->item.virusName, current->item.filepath, current->item.fileName, virusTitre);
            if (!virus)
            {
                g_error("Allocation mémoire impossible");
                return;
            }
            
            add_virus_elements(st, virusDetection, virus);
            virusDetection++;
        }
        current = current->next;
    }

    if(virusDetection == 0)
    {
        virusDetection = -1;
        return;
    }
    
    /* Mise à jour de la page du notebook en fonction de cmd_status */
    gtk_notebook_set_current_page(GTK_NOTEBOOK(st->notebook), (st->cmd_satus == 1) ? 1 : 3);
}