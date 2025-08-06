#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/wait.h>

/* Exécute freshclam et affiche la sortie */
bool worker(void)
{
    FILE *fichier = popen("freshclam", "r");
    if (!fichier)
    {
        return false;
    }

    char output[1024];
    while (fgets(output, sizeof(output), fichier) != NULL)
    {
        fprintf(stdout, "%s", output);
    }

    int status = pclose(fichier);
    return (status == 0);
}

/* Arrête le service freshclam */
bool disableFreshClamService(void)
{
    int ret = system("systemctl stop clamav-freshclam");
    if (ret == -1)
    {
        return false;
    }

    return WIFEXITED(ret) && WEXITSTATUS(ret) == 0;
}

/* Redémarre le service freshclam */
bool enableFreshClamService(void)
{
    int ret = system("systemctl start clamav-freshclam");
    if (ret == -1)
    {
        return false;
    }

    return WIFEXITED(ret) && WEXITSTATUS(ret) == 0;
}

int main(void)
{
    if (!disableFreshClamService())
    {
        fprintf(stderr, "Erreur : impossible d'arrêter freshclam.\n");
        return 1;
    }

    if (!worker())
    {
        fprintf(stderr, "Erreur lors de l'exécution de freshclam.\n");
        return 1;
    }

    if (!enableFreshClamService())
    {
        fprintf(stderr, "Erreur : impossible de redémarrer freshclam.\n");
        return 1;
    }

    return 0;
}
