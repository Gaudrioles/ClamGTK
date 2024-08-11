#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stack.h"
#include "main.h"

Stack new_stack(void)
{
	return NULL;
}

/*-----------------------------------------------------------------*/

int is_empty_stack(Stack st)
{
	if(st == NULL)
	{
		return 1;
	}

	return 0;
}

/*-----------------------------------------------------------------*/

Stack push_stack(Stack st, Virus p)
{
	StackElement *element;

	element = malloc(sizeof(*element));

	if(element == NULL)
	{
		fprintf(stderr, "Probleme allocation dynamique.\n");
		exit(EXIT_FAILURE);
	}

	strcpy(element->pv.virusName, p.virusName);
	strcpy(element->pv.VirusPath, p.VirusPath);
	element->next = st;

	return element;
}

/*-----------------------------------------------------------------*/

Stack pop_stack(Stack st)
{
	StackElement *element;

	if(is_empty_stack(st))
	{
		return new_stack();
	}

	element = st->next;
	
	free(st);

	return element;
}

/*-----------------------------------------------------------------*/

Stack clear_stack(Stack st)
{
	while(!is_empty_stack(st))
	{
		st = pop_stack(st);
	}

	return new_stack();
}

/*-----------------------------------------------------------------*/

Virus add_virus(char *name, char *path)
{
    Virus p;

    memset(&p, 0, sizeof(Virus));
	
	strncpy(p.virusName, name, sizeof(p.virusName) - 1);
    strncpy(p.VirusPath, path, sizeof(p.VirusPath) - 1);

    return p;
}