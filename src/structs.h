#ifndef STRUCTS_H_INCLUDED
#define STRUCTS_H_INCLUDED

#include "defs.h"

typedef struct StackElement
{
    struct {
        gchar *fileName;
        char *filepath;
        char *virusName;
        gboolean isClean;
    } item;
    struct StackElement *next;
} StackElement;

typedef struct {
    char database[124];
    char version[124];
    char sigs[124];
    gboolean upToDate;
} ExtractedData;

typedef struct
{
    GtkApplication *application;
    GtkWidget *window;
    GtkWidget *notebook;
    GtkWidget *bouton_analyser;
    GtkWidget *bouton_update;
    GtkWidget *bouton_retour;
    GtkWidget *bouton_file;
    GtkWidget *bouton_folder;
    GtkWidget *progressbar;
    GtkWidget *scrolled_text;
    GtkWidget *scrolled_analyse;
    GtkWidget *textview;
    GtkWidget *box_objets;
    GtkTextBuffer *textBuffer;
    
    guint thread_Pulse_ID;
    guint thread_Progress_ID;
    guint thread_Scan_ID;
    
    gboolean compileDone;
    gboolean scanDone;

    int cmd_satus;

    gchar *scanPath;

    StackElement *scanItem;
}Application;

typedef struct
{
    char virusName[BUFFER_SIZE];
    char filepath[BUFFER_SIZE];    
    gchar fileName[BUFFER_SIZE];
    GtkWidget *virusTitre;
}VirusData;

typedef struct DispatchData
{
    GtkTextBuffer *buffer;
    char *output_str;
    int color;
} DispatchData;

typedef struct DispatchDataVirus
{
    GtkTextBuffer *buffer;
    StackElement *scanItem;
} DispatchDataVirus;

typedef struct DispatchDataUpdate
{
    GtkTextBuffer *buffer;
    char *database;
    char *sigs;
    char *version;
    gboolean upToDate;
} DispatchDataUpdate;

#endif /* !STRUCTS_H_INCLUDED */