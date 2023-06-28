
#include <stdio.h>
#include <stdlib.h>
#include <labirinto.h>
#include <algorithms.h>
#include <string.h>

void print_result(ResultData *result)
{
    if (!result->sucesso)
    {
        printf("IMPOSSIVEL\n");
        return;
    }

    for (int i = 0; i < result->tamanho_caminho; i++)
        printf("%d %d\n", result->caminho[i].x, result->caminho[i].y);

    printf("%.2lf\n", result->custo_caminho);
    printf("%d\n", result->tamanho_caminho);
    printf("%d\n", result->nos_expandidos);
}

void mostra_caminho(Labirinto *l, ResultData *result, Celula inicio, Celula fim)
{
    if (result->sucesso)
    {
        for (int i = 0; i < result->tamanho_caminho; i++)
            labirinto_atribuir(l, result->caminho[i].y, result->caminho[i].x, CAMINHO);
    }

    labirinto_atribuir(l, inicio.y, inicio.x, INICIO);
    labirinto_atribuir(l, fim.y, fim.x, FIM);
    labirinto_print(l);
}

int main()
{
    char *arquivo_labirinto = "dados/labirintos/maze10x10_obstaculos_1.bin";
    ResultData result;
    Labirinto *lab;

    lab = labirinto_carregar(arquivo_labirinto);
    Celula inicio = {0, 9, NULL};
    Celula fim = {9, 0, NULL};
    labirinto_atribuir(lab, inicio.y, inicio.x, INICIO);
    labirinto_atribuir(lab, fim.y, fim.x, FIM);
    
    result = breadth_first_search(lab, inicio, fim);

    print_result(&result);
    mostra_caminho(lab, &result, inicio, fim);

    labirinto_destruir(lab);
    if (result.caminho != NULL)
    {
        free(result.caminho);
    }

    return 0;
}