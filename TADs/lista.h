#ifndef LIST_H
#define LIST_H

#include "../vinaC.h"

struct Node
{
  struct Member* info;
  struct Node* prev;
  struct Node* next;
};

struct List
{
  struct Node* first;
  struct Node* last;
  int size;
};

// Cria uma lista vazia.
// Retorno: ponteiro p/ a lista ou NULL em erro.
struct List* CreateList();

// Remove todos os itens da lista e libera a memória.
// Retorno: NULL.
struct List* DestroyList(struct List* list);

// Nas operações insere/retira/consulta/procura, a lista inicia na
// posição 0 (primeiro item) e termina na posição TAM-1 (último item).

// Insere o item na lista na posição indicada;
// se a posição for além do fim da lista ou for -1, insere no fim.
// Retorno: número de itens na lista após a operação ou -1 em erro.
int InsertList(struct List* list, struct Member* info, int pos);

// Retira o item da lista da posição indicada.
// se a posição for além do fim da lista ou for -1, retira do fim.
// Retorno: número de itens na lista após a operação ou -1 em erro.
int RemoveList(struct List* list, struct Member* info, int pos);

// Informa o valor do item na posição indicada, sem retirá-lo.
// se a posição for além do fim da lista ou for -1, consulta do fim.
// Retorno: número de itens na lista ou -1 em erro.
int ConsultList(struct List* list, struct Member* info, int pos);

// Informa a posição da 1ª ocorrência do valor indicado na lista.
// Retorno: posição do valor ou -1 se não encontrar ou erro.
int SearchList(struct List* list, struct Member* info);

// Informa o tamanho da lista (o número de itens presentes nela).
// Retorno: número de itens na lista ou -1 em erro.
int SizeList(struct List* list);

// Imprime o conteúdo da lista do inicio ao fim no formato "item item ...",
// com um espaço entre itens, sem espaços antes/depois, sem newline.
void PrintList(struct List* list);

#endif
