
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include "algorithms.h"
#include "stack.h"
#include "vector.h"
#include "hash.h"
#include "queue.h"

int direcao[8][2] = {{0, -1}, {1, -1}, {1, 0}, {1, 1}, {0, 1}, {-1, 1}, {-1, 0}, {-1, -1}}; // direcao dos nos vizinhos a serem explorados em ordem de prioridade

Celula *celula_create(int x, int y, Celula *pai)
{
    Celula *c = malloc(sizeof(Celula));
    c->x = x;
    c->y = y;
    c->pai = pai;
    return c;
}

void celula_destroy(Celula *c)
{
    free(c);
}

int celula_hash(HashTable *h, void *key)
{
    Celula *c = (Celula *)key;
    // 83 e 97 sao primos e o operador "^" é o XOR bit a bit
    return ((c->x * 83) ^ (c->y * 97)) % hash_table_size(h);
}

int celula_cmp(void *c1, void *c2)
{
    Celula *a = (Celula *)c1;
    Celula *b = (Celula *)c2;

    if (a->x == b->x && a->y == b->y)
        return 0;
    else
        return 1;
}

ResultData caminho(Celula *current, ResultData result)
{
    result.sucesso = 1;
    while (current)
    {
        result.caminho[result.tamanho_caminho] = *current;
        result.tamanho_caminho++;
        Celula *pai = current->pai;

        if (pai)
        {
            result.custo_caminho = sqrt(pow(current->x - pai->x, 2) + pow(current->y - pai->y, 2));
        }
        current = pai;
    }

    // reverse
    for (int i = 0; i < result.tamanho_caminho / 2; i++)
    {
        Celula tmp = result.caminho[i];
        result.caminho[i] = result.caminho[result.tamanho_caminho - i - 1];
        result.caminho[result.tamanho_caminho - i - 1] = tmp;
    }
    return result;
}
ResultData _default_result()
{
    ResultData result;

    result.caminho = NULL;
    result.custo_caminho = 0;
    result.nos_expandidos = 0;
    result.tamanho_caminho = 0;
    result.sucesso = 0;

    return result;
}

ResultData a_star(Labirinto *l, Celula inicio, Celula fim)
{
    // TODO!
    return _default_result();
}

ResultData breadth_first_search(Labirinto *l, Celula inicio, Celula fim)
{
    ResultData result = _default_result();
    int max_path_length = labirinto_n_colunas(l) * labirinto_n_linhas(l);
    result.caminho = (Celula *)malloc(sizeof(Celula) * max_path_length);

    Queue *queue = queue_construct(free);
    queue_push(queue, celula_create(inicio.x, inicio.y, NULL));

    Stack *expanded = stack_construct(free);

    while (!queue_empty(queue))
    {
        Celula *current = queue_pop(queue);
        labirinto_atribuir(l, current->y, current->x, EXPANDIDO);
        result.nos_expandidos++;

        stack_push(expanded, current);

        if (current->x == fim.x && current->y == fim.y)
        {
            result = caminho(current, result);
            break;
        }

        for (int i = 0; i < 8; i++)
        {
            int x = current->x + direcao[i][0];
            int y = current->y + direcao[i][1];

            if (x >= 0 && y >= 0 && x < labirinto_n_colunas(l) && y < labirinto_n_linhas(l))
            {
                unsigned char valor = labirinto_obter(l, y, x);
                if (valor == LIVRE || valor == FIM)
                {
                    labirinto_atribuir(l, y, x, FRONTEIRA);
                    queue_push(queue, celula_create(x, y, current));
                }
            }
        }
    }
    
    if(result.sucesso == 0)
    {
        free(result.caminho);
        result.caminho = NULL;
    }

    queue_destroy(queue);
    stack_destroy(expanded);

    return result;
}

ResultData depth_first_search(Labirinto *l, Celula inicio, Celula fim)
{
    ResultData result = _default_result();
    int max_path_length = labirinto_n_colunas(l) * labirinto_n_linhas(l);
    result.caminho = (Celula *)malloc(sizeof(Celula) * max_path_length);

    Stack *stack = stack_construct(free);
    stack_push(stack, celula_create(inicio.x, inicio.y, NULL));

    Stack *expanded = stack_construct(free);

    while (!stack_empty(stack))
    {
        Celula *current = stack_pop(stack);
        labirinto_atribuir(l, current->y, current->x, EXPANDIDO);
        result.nos_expandidos++;

        stack_push(expanded, current);

        if (current->x == fim.x && current->y == fim.y)
        {
            result = caminho(current, result);
            break;
        }

        for (int i = 0; i < 8; i++)
        {
            int x = current->x + direcao[i][0];
            int y = current->y + direcao[i][1];

            if (x >= 0 && y >= 0 && x < labirinto_n_colunas(l) && y < labirinto_n_linhas(l))
            {
                unsigned char valor = labirinto_obter(l, y, x);
                if (valor == LIVRE || valor == FIM)
                {
                    labirinto_atribuir(l, y, x, FRONTEIRA);
                    stack_push(stack, celula_create(x, y, current));
                }
            }
        }
    }
    
    if(result.sucesso == 0)
    {
        free(result.caminho);
        result.caminho = NULL;
    }

    stack_destroy(stack);
    stack_destroy(expanded);

    return result;
}

ResultData dummy_search(Labirinto *l, Celula inicio, Celula fim)
{
    int max_path_length = 0;
    float dx, dy;

    ResultData result = _default_result();

    max_path_length = abs(fim.x - inicio.x) + abs(fim.y - inicio.y);
    result.caminho = (Celula *)malloc(sizeof(Celula) * max_path_length);
    result.sucesso = 1;

    Celula atual = inicio;
    result.caminho[result.tamanho_caminho++] = atual;
    result.nos_expandidos++;

    while ((atual.x != fim.x) || (atual.y != fim.y))
    {
        dx = fim.x - atual.x;
        dy = fim.y - atual.y;

        if (dx != 0)
            dx /= fabs(dx);

        if (dy != 0)
            dy /= fabs(dy);

        atual.x += (int)dx;
        atual.y += (int)dy;

        if (labirinto_obter(l, atual.y, atual.x) == OCUPADO || (atual.x > labirinto_n_colunas(l) - 1) || (atual.y > labirinto_n_linhas(l) - 1) || (atual.x < 0) || (atual.y < 0))
        {
            result.sucesso = 0;
            free(result.caminho);
            result.caminho = NULL;
            return result;
        }

        result.caminho[result.tamanho_caminho++] = atual;
        result.nos_expandidos++;
        result.custo_caminho += sqrt(pow(dx, 2) + pow(dy, 2));
    }

    return result;
}
