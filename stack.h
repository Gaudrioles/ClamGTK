#ifndef STACK_H_INCLUDED
#define STACK_H_INCLUDED

#include "main.h"

Stack new_stack(void);
int is_empty_stack(Stack st);
Stack push_stack(Stack st, Virus p);
Stack pop_stack(Stack st);
Stack clear_stack(Stack st);
Virus add_virus(char *name, char *path);

#endif /* !STACK_H_INCLUDED */