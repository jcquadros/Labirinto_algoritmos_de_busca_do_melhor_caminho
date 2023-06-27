#ifndef _CELULA_H_
#define _CELULA_H_

typedef struct Celula
{
    int x;
    int y;
    struct Celula *pai;
} Celula;

Celula *celula_construct(int x, int y);
void celula_destroy(Celula *c);

#endif