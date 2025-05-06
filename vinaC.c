#include "vinaC.h"

void printMember(struct Member member)
{
    printf("Name: %s \n", member.name);
    printf("UID: %d\n", member.UID);
    printf("pos: %d\n", member.pos);
    printf("Original size: %d\n", member.origSize);
    printf("Modified data: %d\n", member.modifData);
}
void moveData(long int start, long int size, long int pos, FILE* binary)
{
    char* buffer = (char*) calloc(size, 1); //Maybe only size here?

    fseek(binary, start , SEEK_SET);
    size_t readBytes = fread(buffer, 1, size, binary);

    fseek(binary, pos, SEEK_SET);
    fwrite(buffer, 1, size, binary);
    free(buffer);
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
    unsigned int totalSize = 0;
    struct Member member;
    struct stat stats;
    struct Directory directory;
    int memSize;
    long pointerPos;
    int fd = fileno(archive);
    char* buffer = NULL;
    
    if(fd == -1 || fstat(fd, &stats) != 0)
    {
        fprintf(stderr, "Erro no stat/fd do arquivo\n");
        return;
    }

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    fread(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;
    directory.quantity += 1;
    printf("%d - %c\n", directory.quantity, directory.name);

    memcpy(member.name, name, sizeof(member.name));
    member.UID = stats.st_uid;
    member.pos = memSize;
    member.comprSize = stats.st_size;
    member.modifData = stats.st_mtime;
    member.origSize = stats.st_size;

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);

    for(int i = 0; i < memSize; i++)
    {
        struct Member tmp;
        unsigned int sizeDiff;
        int firstPointer, secPointer;
        
        fread(&tmp, sizeof(struct Member), 1, binary);
        pointerPos = ftell(binary);

        //Se o arquivo ja existe no binario 
        if(strcmp(name, tmp.name) == 0)
        {
            int size;
            
            printf("Repetido\n");
            sizeDiff = member.origSize - tmp.origSize;
            firstPointer = totalSize;
            
            //Se o tamanho do arquivo diminuir da problema
            fseek(binary, 0, SEEK_END);
            secPointer = ftell(binary);
            size = secPointer - firstPointer;
            moveData(firstPointer, size, firstPointer + sizeDiff, binary);

            fseek(binary, totalSize, SEEK_SET);
            buffer = (char*) malloc(member.origSize);
            fread(buffer, 1, member.origSize, archive);

            fwrite(buffer, member.origSize, 1, binary);


            free(buffer);
            buffer = NULL;

            printMember(tmp);
            printMember(member);
            tmp = member;
            return;
        }
        else
            printf("nao sao iguais\n", tmp.name);

        totalSize += tmp.origSize;
    }

    pointerPos = ftell(binary);
    //Nesse pedaco de memoria eu estou ajustando o ponteiro do arquivo
    //para escrever o membro antes do directory
    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    //Escrevendo o membro atras do diretorio
    fwrite(&member, sizeof(struct Member), 1, binary);
    //Reescrevendo o diretorio
    fwrite(&directory, sizeof(struct Directory), 1, binary);

    buffer = (char*) malloc(member.origSize);
    size_t readBytes = fread(buffer, 1, member.origSize, archive);

    if(readBytes != (size_t)member.origSize)
    {
        fprintf(stderr, "Erro na na leitura do arquivo");
        free(buffer);
        buffer = NULL;

        return;
    }

    memSize += 1;
    //Movendo todo o conteudo do diretorio para frente e colocando o arquivo
    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);
    //Movendo os bytes do diretorio para frente
    moveData(pointerPos,
            sizeof(struct Directory) + (memSize * sizeof(struct Member)),
            pointerPos + member.origSize,
            binary);

    //Escrevendo conteudo do arquivo
    fseek(binary, pointerPos, SEEK_SET);
    fwrite(buffer, 1, member.origSize, binary);

    free(buffer);
    buffer = NULL;
}
