#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#define WINDOW_WIDTH    700
#define WINDOW_HEIGHT   400

#define MSG_SCAN        "\n----------- Analyse en cours -----------\n"
#define MSG_UPDATE      "\n\n----------- ClamAV update process started -----------\n\n"


#define IMAGE_CLAMAV    "/usr/share/ClamGTK/clamav.png"
#define IMAGE_BT_SEARCH "/usr/share/ClamGTK/search.png"
#define IMAGE_BT_UPDATE "/usr/share/ClamGTK/update.png"
#define IMAGE_BT_FILE   "/usr/share/ClamGTK/file.png"
#define IMAGE_BT_FOLDER "/usr/share/ClamGTK/folder.png"
#define IMAGE_ICON      "clamav"

/*
#define IMAGE_CLAMAV    "DATA/clamav.png"
#define IMAGE_BT_SEARCH "DATA/search.png"
#define IMAGE_BT_UPDATE "DATA/update.png"
#define IMAGE_BT_FILE   "DATA/file.png"
#define IMAGE_BT_FOLDER "DATA/folder.png"
#define IMAGE_ICON      "clamav"
*/

#define CMD_FRESHCLAN   "pkexec sh -c \"systemctl stop clamav-freshclam && freshclam && systemctl start clamav-freshclam\""

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
    GtkTextBuffer *textBuffer;
    guint threadID;
    int cmd_satus;
    int virusNb;

    gchar *scanPath;
}st_widgets;

#endif // MAIN_H_INCLUDED
