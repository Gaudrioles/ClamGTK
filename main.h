#ifndef MAIN_H_INCLUDED
#define MAIN_H_INCLUDED

#define WINDOW_WIDTH    700
#define WINDOW_HEIGHT   400

#define IMAGE_CLAMAV    "/usr/share/ClamGTK/DATA/clamav.png"
#define IMAGE_BT_SEARCH "/usr/share/ClamGTK/DATA/search.png"
#define IMAGE_BT_UPDATE "/usr/share/ClamGTK/DATA/update.png"
#define IMAGE_BT_FILE   "/usr/share/ClamGTK/DATA/file.png"
#define IMAGE_BT_FOLDER "/usr/share/ClamGTK/DATA/folder.png"
#define IMAGE_ICON      "clamav"
#define CLAMGTK_SCRIPT  "/usr/share/ClamGTK/DATA/clam_gtk_update_script.sh"

/*
#define IMAGE_CLAMAV    "DATA/clamav.png"
#define IMAGE_BT_SEARCH "DATA/search.png"
#define IMAGE_BT_UPDATE "DATA/update.png"
#define IMAGE_BT_FILE   "DATA/file.png"
#define IMAGE_BT_FOLDER "DATA/folder.png"
#define IMAGE_ICON      "clamav"
#define CLAMGTK_SCRIPT  "DATA/clam_gtk_update_script.sh"
*/

typedef struct
{
    GtkApplication* application;
    GtkWidget* window;
    GtkWidget* notebook;
    GtkWidget* bouton_analyser;
    GtkWidget* bouton_update;
    GtkWidget* bouton_retour;
    GtkWidget* bouton_file;
    GtkWidget* bouton_folder;
    GtkWidget* progressbar;
    GtkWidget* scrolled;
    GtkWidget* textview;
    GtkTextBuffer* TextBuffer;
    guint threadID;
    gboolean lock;

    gchar* scanPath;
}st_widgets;

#define UNUSED(x) (void)(x)

#endif // MAIN_H_INCLUDED
