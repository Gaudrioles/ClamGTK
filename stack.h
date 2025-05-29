#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include <glib.h>

#include "main.h"

int is_empty_stack(Stack st);

Stack new_stack(void);
Stack push_stack(Stack st, gchar *fileName, char *filePath, char *virusName, gboolean isClean);
Stack pop_stack(Stack st);
Stack clear_stack(Stack st);

void print_stack(Stack st);

#endif /* !STACK_H_INCLUDED */