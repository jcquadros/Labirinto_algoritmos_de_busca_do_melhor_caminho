#include "celula.h"
#include <stdio.h>
#include <stdlib.h>

Celula *celula_construct(int x, int y)
{
    Celula *c = malloc(sizeof(Celula));
    c->x = x;
    c->y = y;
    c->pai = NULL;
    return c;
}

void celula_destroy(Celula *c)
{
    free(c);
}