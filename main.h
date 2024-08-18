#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#include <gtk/gtk.h>

#define WINDOW_WIDTH    700
#define WINDOW_HEIGHT   400
#define BUFFER_SIZE     256

#define MSG_SCAN        "\n----------- Analyse en cours -----------\n"
#define MSG_UPDATE      "\n\n----------- ClamAV update process started -----------\n\n"
#define MSG_VIRUS       "Fichier dangereux identifié\nsupprimer ou conserver ?\n"
#define MSG_RUN         "zenity --warning --title \"ClamGTK\" --width 200 --height 100 --text \"ClamGTK already running\""


#define IMAGE_CLAMAV    "/usr/share/ClamGTK/clamav.png"
#define IMAGE_BT_SEARCH "/usr/share/ClamGTK/search.png"
#define IMAGE_BT_UPDATE "/usr/share/ClamGTK/update.png"
#define IMAGE_BT_FILE   "/usr/share/ClamGTK/file.png"
#define IMAGE_BT_FOLDER "/usr/share/ClamGTK/folder.png"
#define IMAGE_BT_VALID  "/usr/share/ClamGTK/valid.png"
#define IMAGE_BT_DEL    "/usr/share/ClamGTK/del.png"
#define IMAGE_ICON      "clamav"
#define CLAMGTK_CONF    ".config/ClamGTK"
#define CLAMGTK_LOCK    ".config/ClamGTK/ClamGTK.lock"

/*
#define IMAGE_CLAMAV    "DATA/clamav.png"
#define IMAGE_BT_SEARCH "DATA/search.png"
#define IMAGE_BT_UPDATE "DATA/update.png"
#define IMAGE_BT_FILE   "DATA/file.png"
#define IMAGE_BT_FOLDER "DATA/folder.png"
#define IMAGE_BT_VALID  "DATA/valid.png"
#define IMAGE_BT_DEL    "DATA/del.png"
#define IMAGE_ICON      "clamav"
#define CLAMGTK_CONF    ".config/ClamGTK"
*/

#define CMD_FRESHCLAN   "pkexec sh -c \"systemctl stop clamav-freshclam && freshclam && systemctl start clamav-freshclam\""
#define CMD_DEL   "pkexec sh -c"

typedef struct virus
{
    char virusName[BUFFER_SIZE];
    char VirusPath[BUFFER_SIZE];
}Virus;

typedef struct StackElement
{
    Virus pv;
    struct StackElement *next;
}StackElement, *Stack;

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
    guint threadID;

    int cmd_satus;
    int virusNb;

    gchar *scanPath;

    Stack st_virus;
}st_widgets;

#endif // MAIN_H_INCLUDED
