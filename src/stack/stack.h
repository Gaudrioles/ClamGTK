#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include "../common.h"

int is_empty_stack(StackElement *st);

StackElement *new_stack(void);
StackElement *push_stack(StackElement *st, gchar *fileName, char *filePath, char *virusName, gboolean isClean);
StackElement *pop_stack(StackElement *st);
StackElement *clear_stack(StackElement *st);

void print_stack(StackElement *st);

#endif /* !STACK_H_INCLUDED */