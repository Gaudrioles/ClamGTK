#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <gtk/gtk.h>

#define WINDOW_WIDTH    700
#define WINDOW_HEIGHT   400
#define BUFFER_SIZE     256

#define MSG_COMPILE     "\n----------- Compilation des Signatures en cours -----------\n"
#define MSG_SCAN        "\n----------- Analyse en cours -----------\n"
#define MSG_SCAN_F      "\n----------- Analyse en Terminée -----------\n"
#define MSG_UPDATE      "\n\n----------- ClamAV Mise à jours -----------\n\n"
#define MSG_UPDATE_F    "\n\n----------- ClamAV Terminée -----------\n\n"
#define MSG_VIRUS       "Fichier dangereux identifié\nsupprimer ou conserver ?\n"
#define MSG_RUN         "zenity --warning --title \"ClamGTK\" --width 200 --height 100 --text \"ClamGTK already running\""

#define UPDATE_OK       "up-to-date"
#define UPDATE_KO       "updated"

/*
#define IMAGE_CLAMAV    "/usr/share/ClamGTK/clamav.png"
#define IMAGE_BT_SEARCH "/usr/share/ClamGTK/search.png"
#define IMAGE_BT_UPDATE "/usr/share/ClamGTK/update.png"
#define IMAGE_BT_FILE   "/usr/share/ClamGTK/file.png"
#define IMAGE_BT_FOLDER "/usr/share/ClamGTK/folder.png"
#define IMAGE_BT_VALID  "/usr/share/ClamGTK/valid.png"
#define IMAGE_BT_DEL    "/usr/share/ClamGTK/del.png"
#define IMAGE_ICON      "clamav"
#define CLAMGTK_APP     "ClamGTK.app"
*/

#define IMAGE_CLAMAV    "DATA/clamav.png"
#define IMAGE_BT_SEARCH "DATA/search.png"
#define IMAGE_BT_UPDATE "DATA/update.png"
#define IMAGE_BT_FILE   "DATA/file.png"
#define IMAGE_BT_FOLDER "DATA/folder.png"
#define IMAGE_BT_VALID  "DATA/valid.png"
#define IMAGE_BT_DEL    "DATA/del.png"
#define IMAGE_ICON      "clamav"
#define CLAMGTK_APP     "ClamGTK.app"



#define CMD_FRESHCLAN   "pkexec sh -c \"systemctl stop clamav-freshclam && freshclam && systemctl start clamav-freshclam\""
#define CMD_DEL         "pkexec rm"

typedef struct detection
{
    char *virusName;
    char *filepath;    
    gchar *fileName;
    gboolean isClean;
}Detection;

typedef struct StackElement
{
    Detection item;
    struct StackElement *next;
} StackElement;

typedef StackElement *Stack;

typedef struct {
    char database[124];
    char version[124];
    char sigs[124];
    gboolean upToDate;
} ExtractedData;

typedef struct st_widgets
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

    Stack scanItem;
}st_widgets;

typedef struct
{
    char virusName[BUFFER_SIZE];
    char filepath[BUFFER_SIZE];    
    gchar fileName[BUFFER_SIZE];
    GtkWidget *virusTitre;
}VirusData;

#endif // MAIN_H_INCLUDED
