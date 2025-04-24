#include "vinaC.h"

//Start: the first byte to move
//End: The last byte to move
//Ref: The byte that will hold the block of information
void move(ulong start, ulong end, ulong ref, FILE* archive)
{
    ulong blockSize = end - start;
    char* content = malloc(sizeof(blockSize));

    //Movendo ponteiro para o inicio da info
    fseek(archive, 0, start);
    //Salvando a informacao no buffer
    fread(content, sizeof(blockSize), 1, archive);

    fseek(archive, 0, ref);
    fwrite(content, sizeof(blockSize), 1, archive);
}

void ExplainProg()
{
        printf("\n");
        printf("VinaC, the program that groups and compresses multiple files\n");
        printf("%-50s", "Usage: ./vinaC [command] [arquive]\n");
        printf("\n");
        printf("%-10s %-30s %-60s\n", "\"-ip\"", "[arquive] [member1 ..]", "Add one or more members without compression to the arquive.vc");
        printf("%-10s %-30s %-60s\n", "\"-ic\"", "[arquive] [member1 ..]", "Add one or more members with compression to the arquive.vc");
        printf("%-10s %-30s %-60s\n", "\"-s\"", "[arquive] [member]", "Moves the indicated member to after the existing target member in the archive");
        printf("%-10s %-30s %-60s\n", "\"-x\"", "[arquive] [member1 ..]", "Extract the indicated members on the archive.vc, if there are no members indicated, extract all");
        printf("%-10s %-30s %-60s\n", "\"-r\"", "[arquive] [member1 ..]", "Remove the indicated members of the archive");
        printf("%-10s %-30s %-60s\n", "\"-c\"", "[arquive]", "List every member of the archive in order, including properties of each member");


};

void InsertArquive(FILE* arquive, FILE* binary)
{
    printf("tudo certo aqui\n");
}
