#include "vinaC.h"

//Start: the first byte to move
//End: The last byte to move
//Ref: The byte that will hold the block of information
void move(void* start, void* end, void* ref)
{
    size_t size;

    if(!start || !end || !ref)
    {
        fprintf(stderr, "Invalid pointers to move data\n");
        return; 
    }

    uintptr_t startAddr = (uintptr_t)start;
    uintptr_t endAddr = (uintptr_t)end;
    
    if(endAddr <= startAddr)
    {
        fprintf(stderr, "Error: Invalid memory region\n");
        return; 
    }

    size = endAddr - startAddr;
    memmove(ref, start, size);
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

void InsertArquive(FILE* archive, FILE* binary, char* name)
{
    struct Member member;
    struct stat stats;
    char* buffer = NULL;

    int fd = fileno(archive);
    
    if(fd == -1 || fstat(fd, &stats) != 0)
    {
        fprintf(stderr, "Erro no stat/fd do arquivo\n");
        return;
    }

    member.name = name;
    //member.UID = stats.st_uid;
    //member.pos
    //member.origSize
    //member.comprSize
    //member.order
    //member.modifData
    member.origSize = stats.st_size;

    buffer = (char*) malloc(member.origSize + 1);

    size_t readBytes = fread(buffer, 1, member.origSize, archive);

    printf("%d - %s - %s\n", member.origSize, buffer, name);

    if(readBytes != (size_t)member.origSize)
    {
        fprintf(stderr, "Erro na na leitura do arquivo");
        free(buffer);
        buffer = NULL;

        return;
    }

    fwrite(buffer, 1, member.origSize, binary);

    free(buffer);
    buffer = NULL;
}
