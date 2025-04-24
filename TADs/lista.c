#include <stdio.h>
#include <stdlib.h>
#include "lista.h"

struct List* CreateList()
{
    struct List* list;

    list = malloc(sizeof(struct List));

    if(list == NULL)
        return NULL;

    list->first = NULL;
    list->last = NULL;
    list->size = 0;

    return list;
}

struct List* DestroyList(struct List* list)
{
    struct Node* tmp;

    while(list->first != NULL)
    {
        tmp = list->first;
		list->first = tmp->next;
        free(tmp); // Need to probably free the struct Member inside the node
    }

    free(list);
    return NULL;
}

int InsertList(struct List* list, struct Member* info, int pos)
{
    struct Node* newNode;
    struct Node* aux;

    //Se a lista e nula ou nao tem elementos ou a posicao e maior que o tamanho 
    if(list == NULL || !(newNode = malloc(sizeof(struct Node))))
        return -1;
    
    newNode->info = info;

    //Se a lista nao tem ninguem
    if(list->size == 0)
    {
        list->first = newNode;
        list->last = newNode;

        newNode->next = NULL;
        newNode->prev = NULL;

        return ++list->size;
    }
    
    //Se queremos colocar na primeira posicao
    if(pos == 0)
    {
        list->first->prev = newNode;

        newNode->next = list->first;
        newNode->prev = NULL;

        list->first = newNode;

        return ++list->size;
    }

    //Posicao maior que o tamanho
    if(pos >= list->size || pos == -1)
    {
        list->last->next = newNode;
        newNode->prev = list->last;
        newNode->next = NULL;
        list->last = newNode;

        return ++list->size;
    }

    //Se nao for o ultimo elemento, nem o primeiro, e se a lista nao for vazia 
    aux = list->first;

    for(int i = 0; i < pos; i++)
    {
        aux = aux->next;
    }

    newNode->next = aux;
    newNode->prev = aux->prev;

    aux->prev->next = newNode;
    aux->prev = newNode;

    return ++list->size;
}

int RemoveList(struct List* list, struct Member* info, int pos)
{
    struct Node* aux;

    //Se a lista e nula ou nao tem elementos ou a posicao e maior que o tamanho 
    if(list == NULL || list->size <= 0 || pos >= list->size)
        return -1;
    
    //Ha apenas um elemento
    if(list->first->next == NULL)
    {
		*info = *list->first->info;
		free(list->first);
		list->first = NULL;
		list->last = NULL;
		return --list->size;
    }

    //Se queremos retirar da primeira posicao
    if(pos == 0)
    {
        aux = list->first;

        list->first = aux->next;
        list->first->prev = NULL;

        *info = *aux->info;
        free(aux);
    
        aux = NULL;
        return --list->size;
    }

    //Retira ultimo
    if(pos == -1 || pos == list->size - 1)
    {
        aux = list->last;
        list->last = aux->prev;
        list->last->next = NULL;
        *info = *aux->info;
        free(aux);
        
        aux = NULL;
        return --list->size;
    }

    aux = list->first;

    for(int i = 0; i < pos; i++)
    {
        aux = aux->next;
    }

    aux->next->prev = aux->prev;
    aux->prev->next = aux->next;

    *info = *aux->info;
    free(aux);
	aux = NULL;

    return --list->size;
}

int ConsultList(struct List* list, struct Member* info, int pos)
{
    struct Node* aux;

    //Se a lista e nula ou nao tem elementos ou a posicao e maior que o tamanho 
    if(list == NULL || list->size == 0 || pos >= list->size)
        return -1;

    if(pos == -1)
    {
        *info = *list->last->info;
        return list->size;
    }

    aux = list->first;

    for(int i = 0; i < pos; i++)
        aux = aux->next;

    *info = *aux->info;

    return list->size;
}

int SearchList(struct List* list, struct Member* info)
{
    struct Node* aux;
    int cont;

    //Se a lista e nula ou nao tem elementos
    if(list == NULL || list->size == 0)
        return -1;

    aux = list->first;
    cont = 0;

    while(aux != NULL && aux->info != info)
    {
        aux = aux->next;
        cont++;
    }

    if(aux == NULL)
        return -1;
    
    return cont;
}

int SizeList(struct List* list)
{
    if(list == NULL)
        return -1;

    return list->size;
}

void PrintList(struct List* list)
{
    struct Node* aux;

    //Se a lista e nula ou nao tem elementos
    if(list == NULL || list->size == 0)
        return;
    
    aux = list->first;

    while(aux->next != NULL)
    {
        printf("%d", aux->info->pos);
        printf(" ");
        aux = aux->next;
    }

    printf("%d\n", aux->info->pos);
}
