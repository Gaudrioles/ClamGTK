#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glib.h>

#include "stack.h"
#include "main.h"

/*-----------------------------------------------------------------*/

Stack new_stack(void)
{
    return NULL;
}

/*-----------------------------------------------------------------*/

gboolean is_empty_stack(Stack st)
{
	if(!st)
	{
		return TRUE;
	}
    
	
	return FALSE;
}

/*-----------------------------------------------------------------*/

Stack push_stack(Stack st, gchar *fileName, char *filePath, char *virusName, gboolean isClean)
{
    if (!fileName || !filePath)
    {
        fprintf(stderr, "Erreur : fileName ou filePath est NULL.\n");
        exit(EXIT_FAILURE);
    }

    StackElement *element = malloc(sizeof(*element));
    if (!element)
    {
        fprintf(stderr, "Erreur : Problème d'allocation dynamique.\n");
        exit(EXIT_FAILURE);
    }

    element->item.filepath = strdup(filePath);
    if (!element->item.filepath)
    {
        fprintf(stderr, "Erreur : Problème d'allocation pour filePath.\n");
        free(element);
        exit(EXIT_FAILURE);
    }

    element->item.fileName = g_strdup(fileName);
    if (!element->item.fileName)
    {
        fprintf(stderr, "Erreur : Problème d'allocation pour fileName.\n");
        free(element->item.filepath);
        free(element);
        exit(EXIT_FAILURE);
    }

    element->item.virusName = virusName ? g_strdup(virusName) : NULL;
    if (virusName && !element->item.virusName)
    {
        fprintf(stderr, "Erreur : Problème d'allocation pour virusName.\n");
        g_free(element->item.fileName);
        free(element->item.filepath);
        free(element);
        exit(EXIT_FAILURE);
    }

    element->item.isClean = isClean;
    element->next = st;

    return element;
}

/*-----------------------------------------------------------------*/

Stack pop_stack(Stack st)
{
    if (is_empty_stack(st))
    {
        return new_stack();
    }

    StackElement *element = st->next;

    free(st->item.filepath);
    g_free(st->item.fileName);
    g_free(st->item.virusName);

    free(st);

    return element;
}

/*-----------------------------------------------------------------*/

Stack clear_stack(Stack st)
{
    while (!is_empty_stack(st))
    {
        st = pop_stack(st);
    }

    return new_stack();
}

/*-----------------------------------------------------------------*/

void print_stack(Stack st)
{
    StackElement *current = st;
	
	
	if (is_empty_stack(st))
    {
        printf("La pile est vide.\n");
        return;
    }

    printf("Contenu de la pile :\n");
    while (current)
    {
		if(!current->item.isClean)
        {
			printf("  Nom du fichier : %s\n", current->item.fileName ? current->item.fileName : "(NULL)");
			printf("  Chemin du fichier : %s\n", current->item.filepath ? current->item.filepath : "(NULL)");
			printf("  Nom du virus : %s\n", current->item.virusName ? current->item.virusName : "(NULL)");
		}
		current = current->next;
    }
}

/*-----------------------------------------------------------------*/