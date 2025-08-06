#include <glib.h>
#include <glib/gstdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stddef.h>
#include <regex.h>

#include "../common.h"

/* Supprimer un élément dans une chaine */
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

/* Extraire le nom du virus depuis ouputBuffer */
char *getVirusName(const char *ouputBuffer)
{
    if (!ouputBuffer)
    {
        return NULL;
    }

    const char *result = strstr(ouputBuffer, ": ");
    if (!result)
    {
        return NULL;
    }

    result += 2; // Skip past ": "

    // Calcule de la longeur de virus
    const char *end = strchr(result, ' ');
    if (!end)
    {
        return NULL;
    }

    size_t name_len = end - result;

    char *virus_name = (char*)malloc(name_len + 1);
    if (!virus_name)
    {
        fprintf(stderr, "Allocation impossible [getVirusName]\n");
        return NULL;
    }

    strncpy(virus_name, result, name_len);
    virus_name[name_len] = '\0';

    return virus_name;
}

/* Extraire le chemin du fichier depuis ouputBuffer */
char *getFilePath(const char *ouputBuffer)
{
    // Trouver l'emplacement de ": "
    const char *pathEnd = strstr(ouputBuffer, ": ");
    if (!pathEnd)
    {
        return NULL;
    }

    // Calculer la longueur du chemin (jusqu'à ": ")
    size_t path_len = pathEnd - ouputBuffer;

    // Allouer de la mémoire pour le chemin
    char *filePath = (char *)malloc(path_len + 1);
    if (!filePath)
    {
        fprintf(stderr, "Allocation impossible [getFilePath]\n");
        return NULL;
    }

    // Copier le chemin dans la nouvelle chaîne
    strncpy(filePath, ouputBuffer, path_len);
    filePath[path_len] = '\0'; // Terminer la chaîne avec '\0'

    return filePath;
}

/* Extraire le nom du fichier depuis filePath */
gchar *getFileName(const char *filePath)
{
    if (g_file_test(filePath, G_FILE_TEST_EXISTS))
    {
        return g_path_get_basename(filePath);
    }

    return NULL;
}

gboolean checkIfClean(const gchar *src)
{
    gchar *result_clean = g_strstr_len(src, -1, ": OK");    /* File: OK */
    gchar *result_virus = g_strstr_len(src, -1, "FOUND");   /* File: Win.Test.EICAR_HDB-1 FOUND */

    if (result_clean)
    {
        return TRUE;
    }
    else if (result_virus)
    {
        return FALSE;
    }

    return TRUE;
}

gboolean extractData(const char *input, int regexSelect, ExtractedData *result)
{
    if (!input || !result)
    {
        return FALSE;
    }

    const char *pattern = NULL;
    if (regexSelect == 1)
    {
        pattern = "([a-zA-Z0-9_.]+) updated \\(version: ([0-9]+), sigs: ([0-9]+)";
        result->upToDate = FALSE;
    }
    else
    {
        pattern = "([a-zA-Z0-9_.]+) database is up-to-date \\(version: ([0-9]+), sigs: ([0-9]+)";
        result->upToDate = TRUE;
    }

    regex_t regex;
    regmatch_t matches[4]; // Indices pour le texte entier, database, version, sigs

    // Compiler l'expression régulière
    if (regcomp(&regex, pattern, REG_EXTENDED) != 0)
    {
        fprintf(stderr, "Erreur: impossible de compiler l'expression régulière.\n");
        return FALSE;
    }

    // Effectuer la correspondance
    if (regexec(&regex, input, 4, matches, 0) != 0)
    {
        regfree(&regex);
        return FALSE; // Aucun résultat trouvé
    }

    // Extraire les valeurs correspondantes
    size_t len;

    len = matches[1].rm_eo - matches[1].rm_so;
    strncpy(result->database, input + matches[1].rm_so, len);
    result->database[len] = '\0';

    len = matches[2].rm_eo - matches[2].rm_so;
    strncpy(result->version, input + matches[2].rm_so, len);
    result->version[len] = '\0';

    len = matches[3].rm_eo - matches[3].rm_so;
    strncpy(result->sigs, input + matches[3].rm_so, len);
    result->sigs[len] = '\0';

    regfree(&regex);
    return TRUE;
}

char *extract_version(const char *input)
{
    char *copy = strdup(input);
    char *slash = strchr(copy, '/');
    if (slash)
    {
        *slash = '\0';
    }

    return copy;
}

gchar *extract_db_version(const char *input)
{
    char buffer[BUFFER_SIZE] = {0};
    const char *first_slash = strchr(input, '/');
    if (!first_slash)
    {
        return NULL;
    }

    const char *second_slash = strchr(first_slash + 1, '/');
    if (!second_slash)
    {
        return NULL;
    }

    size_t len = second_slash - (first_slash + 1);
    strncpy(buffer, first_slash + 1, len);
    buffer[len] = '\0';
    
    return g_strdup_printf("%s :\t%s", UI_DB, buffer);
}

gchar *extract_date(const char *input)
{
    const char *last_slash = strrchr(input, '/');
    if (!last_slash)
    {
        return NULL;
    }

    /* On saute le début */
    const char *start = strchr(last_slash + 1, ' ');
    if (!start) return NULL;
    start++;

    char short_month[4], day[3], hour[9], year[5];
    if (sscanf(start, "%3s %2s %8s %4s", short_month, day, hour, year) != 4)
        return NULL;

    /* Correspondance des mois */
    const char *short_names[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    const char *full_names[] = {
        "Janvier", "Février", "Mars", "Avril", "Mai", "Juin",
        "Juillet", "Août", "Septembre", "Octobre", "Novembre", "Decembre"
    };

    const char *full_month = NULL;
    for (int i = 0; i < 12; i++)
    {
        if (strcmp(short_month, short_names[i]) == 0)
        {
            full_month = full_names[i];
            break;
        }
    }

    if (!full_month)
    {
        return NULL;
    }
    
    return g_strdup_printf("%s :\t%s %s %s", UI_DATE, day, full_month, year);
}

char *getClamscanVersion(void)
{
    char *result = NULL;
    FILE *fichier = NULL;
    
    /* Process Update */
    fichier = popen("clamscan --version", "r");
    if (!fichier)
    {
        return NULL;
    }

    char output[1024];
    
    if (fgets(output, sizeof(output), fichier))
    {
        // Supprime le retour à la ligne en fin de chaîne, s’il existe
        output[strcspn(output, "\n")] = '\0';

        result = strdup(output);
    }
    
    /* Fermer le processus */
    pclose(fichier);

    return result;
}