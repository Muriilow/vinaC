#include "vinaC.h"

void printMember(struct Member member)
{
    printf("Name: %s \n", member.name);
    printf("Position: %d\n", member.pos);
    printf("UID: %d\n", member.UID);
    printf("Original size: %d\n", member.origSize);
    printf("Modified data: %d\n", member.modifData);
}
void MoveData(long int start, long int size, long int pos, FILE* binary)
{
    char* buffer = (char*) malloc(size); //Maybe only size here?

    fseek(binary, start , SEEK_SET);
    size_t readBytes = fread(buffer, 1, size, binary);

    fseek(binary, pos, SEEK_SET);
    fwrite(buffer, 1, size, binary);
    free(buffer);
}
void ListMembers(FILE* binary)
{
    struct Directory directory;
    struct Member member;
    int memSize;

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    fread(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    for(int i = 0; i < memSize; i++)
    {
        fread(&member, sizeof(struct Member), 1, binary);
        printf("---------------------------\n");
        printMember(member);
    }
    printf("---------------------------\n");

}

void ExplainProg()
{
        printf("\n");
        printf("VinaC, the program that groups and compresses multiple files\n");
        printf("%-50s", "Usage: ./vinaC [command] [arquive]\n");
        printf("\n");
        printf("%-10s %-30s %-60s\n", "\"-p\"", "[arquive] [member1 ..]", "Add one or more members without compression to the arquive.vc");
        printf("%-10s %-30s %-60s\n", "\"-i\"", "[arquive] [member1 ..]", "Add one or more members with compression to the arquive.vc");
        printf("%-10s %-30s %-60s\n", "\"-s\"", "[arquive] [member]", "Moves the indicated member to after the existing target member in the archive");
        printf("%-10s %-30s %-60s\n", "\"-x\"", "[arquive] [member1 ..]", "Extract the indicated members on the archive.vc, if there are no members indicated, extract all");
        printf("%-10s %-30s %-60s\n", "\"-r\"", "[arquive] [member1 ..]", "Remove the indicated members of the archive");
        printf("%-10s %-30s %-60s\n", "\"-c\"", "[arquive]", "List every member of the archive in order, including properties of each member");
};

void InsertNormalArchive(FILE* archive, FILE* binary, char* name)
{
    unsigned int totalSize = 0;
    struct Member member;
    struct stat stats;
    struct Directory directory;
    int memSize;
    long pointerPos;
    int fd = fileno(archive);
    int fdBinary = fileno(binary);
    char* buffer = NULL;
    
    if(fd == -1 || fstat(fd, &stats) != 0 || fdBinary == -1)
    {
        fprintf(stderr, "Erro no stat/fd do arquivo\n");
        return;
    }

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    fread(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

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
        int sizeDiff;
        int firstPointer, secPointer;
        
        pointerPos = ftell(binary);
        fread(&tmp, sizeof(struct Member), 1, binary);

        //Se o arquivo ja existe no binario 
        if(strcmp(name, tmp.name) == 0)
        {
            int size;
            
            sizeDiff = member.origSize - tmp.origSize;

            //Because the member already exists, its position needs to be the exactly same as the other
            member.pos = tmp.pos;

            if(sizeDiff >= 0)
            {
                int pos;
                firstPointer = totalSize;

                fseek(binary, 0, SEEK_END);
                secPointer = ftell(binary);
                size = secPointer - firstPointer;
                while(size > member.origSize)
                {
                    pos = secPointer - member.origSize;
                    secPointer -= member.origSize;
                    MoveData(pos, member.origSize, pos + sizeDiff, binary);
                    size -= member.origSize;
                }
                pos = secPointer - size;
                MoveData(pos, size, pos + sizeDiff, binary);
                fseek(binary, totalSize, SEEK_SET);
                

                buffer = (char*) malloc(member.origSize);
                fread(buffer, 1, member.origSize, archive);

                fwrite(buffer, member.origSize, 1, binary);

                free(buffer);
                buffer = NULL;
            }
            else
            {
                totalSize += tmp.origSize;
                firstPointer = totalSize;

                fseek(binary, 0, SEEK_END);
                secPointer = ftell(binary);

                size = secPointer - firstPointer;
                while(size > member.origSize)
                {
                    MoveData(firstPointer, member.origSize, firstPointer + sizeDiff, binary);
                    firstPointer += member.origSize;
                    size -= member.origSize;
                }
                MoveData(firstPointer, size, firstPointer + sizeDiff, binary);

                fseek(binary, totalSize - tmp.origSize, SEEK_SET);
                buffer = (char*) malloc(member.origSize);
                fread(buffer, 1, member.origSize, archive);

                fwrite(buffer, member.origSize, 1, binary);

                //We need to truncate the archive now
                ftruncate(fdBinary, secPointer + sizeDiff);
                free(buffer);
                buffer = NULL;
            }
            //We already updated the new archive
            //Let's update the struct member
            fseek(binary, pointerPos + sizeDiff, SEEK_SET);
            fwrite(&member, sizeof(struct Member), 1, binary);

            return;
        }
        else
            printf("Not Equal\n", tmp.name);

        totalSize += tmp.origSize;
    }

    directory.quantity += 1;
    printf("%d - %c\n", directory.quantity, directory.name);

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
    MoveData(pointerPos,
            sizeof(struct Directory) + (memSize * sizeof(struct Member)),
            pointerPos + member.origSize,
            binary);

    //Escrevendo conteudo do arquivo
    fseek(binary, pointerPos, SEEK_SET);
    fwrite(buffer, 1, member.origSize, binary);

    free(buffer);
    buffer = NULL;
}
void InsertCompressedArchive(FILE* archive, FILE* binary, char* name)
{
/*
    unsigned int totalSize = 0;
    struct Member member;
    struct stat stats;
    struct Directory directory;
    int memSize;
    long pointerPos;
    int fd = fileno(archive);
    int fdBinary = fileno(binary);
    char* uncompBuffer, *compBuffer;
    


    if(fd == -1 || fstat(fd, &stats) != 0 || fdBinary == -1)
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
        int sizeDiff;
        int firstPointer, secPointer;
        
        pointerPos = ftell(binary);
        fread(&tmp, sizeof(struct Member), 1, binary);

        //Se o arquivo ja existe no binario 
        if(strcmp(name, tmp.name) == 0)
        {
            int size;
            
            sizeDiff = member.origSize - tmp.origSize;
            if(sizeDiff >= 0)
            {
                firstPointer = totalSize;

                fseek(binary, 0, SEEK_END);
                secPointer = ftell(binary);
                size = secPointer - firstPointer;
                MoveData(firstPointer, size, firstPointer + sizeDiff, binary);

                fseek(binary, totalSize, SEEK_SET);
                buffer = (char*) malloc(member.origSize);
                fread(buffer, 1, member.origSize, archive);

                fwrite(buffer, member.origSize, 1, binary);

                free(buffer);
                buffer = NULL;
            }
            else
            {
                totalSize += tmp.origSize;
                firstPointer = totalSize;

                fseek(binary, 0, SEEK_END);
                secPointer = ftell(binary);
                size = secPointer - firstPointer;

                fseek(binary, firstPointer + sizeDiff, SEEK_SET);
                int thirdPointer = ftell(binary);
                MoveData(firstPointer, size, thirdPointer, binary);

                fseek(binary, totalSize - tmp.origSize, SEEK_SET);
                buffer = (char*) malloc(member.origSize);
                fread(buffer, 1, member.origSize, archive);

                fwrite(buffer, member.origSize, 1, binary);

                //We need to truncate the archive now
                ftruncate(fdBinary, secPointer + sizeDiff);
                free(buffer);
                buffer = NULL;
            }
            //We already updated the new archive
            //Let's update the struct member
            fseek(binary, pointerPos + sizeDiff, SEEK_SET);
            fwrite(&member, sizeof(struct Member), 1, binary);

            return;
        }
        else
            printf("nao sao iguais\n", tmp.name);

        totalSize += tmp.origSize;
    }
    uncompBuffer = (char*) malloc(member.origSize);
    compBuffer = (char*) malloc((member.origSize * 1.04) + 1);

    size_t readBytes = fread(uncompBuffer, 1, member.origSize, archive);

    if(readBytes != (size_t)member.origSize)
    {
        fprintf(stderr, "Erro na na leitura do arquivo");
        free(buffer);
        buffer = NULL;

        return;
    }
    
    //int compSize = LZ_Compress(buffer, compBuffer, 

    pointerPos = ftell(binary);
    //Nesse pedaco de memoria eu estou ajustando o ponteiro do arquivo
    //para escrever o membro antes do directory
    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    //Escrevendo o membro atras do diretorio
    fwrite(&member, sizeof(struct Member), 1, binary);
    //Reescrevendo o diretorio
    fwrite(&directory, sizeof(struct Directory), 1, binary);

    memSize += 1;
    //Movendo todo o conteudo do diretorio para frente e colocando o arquivo
    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);
    //Movendo os bytes do diretorio para frente
    MoveData(pointerPos,
            sizeof(struct Directory) + (memSize * sizeof(struct Member)),
            pointerPos + member.origSize,
            binary);

    //Escrevendo conteudo do arquivo
    fseek(binary, pointerPos, SEEK_SET);
    fwrite(buffer, 1, member.origSize, binary);

    free(buffer);
    buffer = NULL;
    return;
    */
}
