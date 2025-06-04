#include <glib.h>
#include <glib/gstdio.h>

/* Compter le nombre d'élément d'un répertoire */
void compteRepertoire(const gchar *repertoire, gint *NombreElement)
{
    GDir *dir;
    const gchar *entry;
    gchar *path;

    if (g_file_test(repertoire, G_FILE_TEST_IS_REGULAR))
    {
        (*NombreElement)++;        
        return;
    }

    dir = g_dir_open(repertoire, 0, NULL);
    if (!dir)
    {
        g_printerr("Erreur : Impossible d'ouvrir le répertoire %s\n", repertoire);
        return;
    }

    while ((entry = g_dir_read_name(dir)) != NULL)
    {
        // Construire le chemin complet
        path = g_build_filename(repertoire, entry, NULL);

        // Vérifier si c'est un fichier régulier
        if (g_file_test(path, G_FILE_TEST_IS_REGULAR))
        {
            (*NombreElement)++;
        }
        // Vérifier si c'est un répertoire et non "." ou ".."
        else if (g_file_test(path, G_FILE_TEST_IS_DIR) && g_strcmp0(entry, ".") != 0 && g_strcmp0(entry, "..") != 0)
        {
            compteRepertoire(path, NombreElement); // Récursion
        }

        g_free(path); // Libérer la mémoire pour chaque chemin construit
    }

    g_dir_close(dir);
}