#include "vinaC.h"
#include "lz.h"
void PrintMember(struct Member member)
{
    char buffer[80];
    struct tm *timeInfo = localtime(&member.modifData);

    if (timeInfo == NULL) 
    {
        fprintf(stderr, "Error formating the modification time\n");
        exit(EXIT_FAILURE);
    }

    // Form 
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeInfo);


    printf("Name: %s \n", member.name);
    printf("Order: %d\n", member.order);
    printf("Position: %d\n", member.pos);
    printf("UID: %d\n", member.UID);
    printf("Original size: %d\n", member.origSize);
    printf("Disk size: %d\n", member.size);
    printf("Modified data: %s\n", buffer);
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

int CheckArchive(char* name, FILE* binary, struct Member *member)
{
    struct Directory directory = {0};
    int memSize, pointerPos;


    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    fread(&directory, sizeof(struct Directory), 1, binary);
    
    memSize = directory.quantity;

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);

    for(int i = 0; i < memSize; i++)
    {
        struct Member tmp;
        fread(&tmp, sizeof(struct Member), 1, binary);

        //Se o arquivo ja existe no binario 
        if(strcmp(name, tmp.name) == 0)
        {
            *member = tmp; 
            return 0;
        }
    }
    
    fprintf(stderr, "Archive doesn't exists...\n");
    exit(EXIT_FAILURE);
}

void RemoveMember(char* name, FILE* binary)
{
    struct Member member = {0};
    struct Directory directory = {0};
    int fdBinary, endBinary, isAfter, structPos, offset;


    if(CheckArchive(name, binary, &member) != 0)
    {
        fprintf(stderr, "This member doesn't exist.\n");
        exit(EXIT_FAILURE);
    }

    fdBinary = fileno(binary);

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);


    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    fread(&directory, sizeof(struct Directory), 1, binary);
    
    offset = member.pos + member.size;
    fseek(binary, member.pos, SEEK_SET);
    MoveData(offset, endBinary - offset, member.pos, binary);

    ftruncate(fdBinary, endBinary - member.size);
    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    isAfter = 0;
    for(int i = 0; i < directory.quantity; i++)
    {
        struct Member tmp;

        fread(&tmp, sizeof(struct Member), 1, binary);

        if(strcmp(member.name, tmp.name) == 0)
        {
            structPos = ftell(binary);
            isAfter = 1;
            continue;
        }
        if(isAfter == 1)
        {
            tmp.order--;
            tmp.pos -= member.size;
            PrintMember(tmp);
            fseek(binary, -sizeof(struct Member), SEEK_CUR);
            fwrite(&tmp, sizeof(struct Member), 1, binary);
        }
    }

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    MoveData(structPos, endBinary - structPos, structPos - sizeof(struct Member), binary);
    ftruncate(fdBinary, endBinary - sizeof(struct Member));

    //The member count decreased, so let's change the directory
    directory.quantity -= 1;
    fseek(binary, -sizeof(struct Directory), SEEK_END);
    fwrite(&directory, sizeof(struct Directory), 1, binary);
}
void MoveMembers(char* name1, char* name2, FILE* binary)
{
    struct Member member1 = {0};
    struct Member member2 = {0};
    struct Directory directory = {0};
    int member1Pos, member2Pos, firstStruct, struct1Pos, struct2Pos;
    int endBinary, fdBinary, offset, isAfter;
    unsigned char *buffer;

    //Check if the members exist and getting the information
    if(CheckArchive(name1, binary, &member1) == 0)
    {
        //PrintMember(member1);
        if(name2 == NULL)
            printf("nulo\n");
        else
        {
            if(CheckArchive(name2, binary, &member2) == 0)
            PrintMember(member2);
        }
    }
    
    fdBinary = fileno(binary);

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);
    //getting the information
    fseek(binary, member1.pos, SEEK_SET);
    member1Pos = ftell(binary);

    buffer = (unsigned char*) malloc(member1.size);
    fread(buffer, member1.size, 1, binary);

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    fread(&directory, sizeof(struct Directory), 1, binary);

    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    firstStruct = ftell(binary);

    //If the user has passed an membr to move infront
    if(name2 != NULL)
    {
        //Atualizando o conteudo 
        offset = member2.pos + member2.size;
        MoveData(offset, endBinary - offset, offset + member1.size, binary);

        fseek(binary, offset, SEEK_SET);
        fwrite(buffer, member1.size, 1, binary);
        fseek(binary, 0, SEEK_END);
        endBinary = ftell(binary);

        offset = member1.pos + member1.size*2;
        MoveData(offset, endBinary - offset, member1.pos + member1.size, binary);

        ftruncate(fdBinary, endBinary - member1.size);

        //Atualizando as structs
        fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
        isAfter = 0;
        for(int i = 0; i < directory.quantity; i++)
        {

            struct Member tmp;

            fread(&tmp, sizeof(struct Member), 1, binary);

            //Se o arquivo ja existe no binario 
            if(strcmp(member2.name, tmp.name) == 0)
            {
                struct2Pos = ftell(binary);
                isAfter =1;
                continue;
            }
            if(strcmp(member1.name, tmp.name) == 0)
            {
                struct1Pos = ftell(binary);
                continue;
            }
            if(isAfter == 1)
            {
                tmp.order++;
                tmp.pos += member1.size;
                PrintMember(tmp);
                fseek(binary, -sizeof(struct Member), SEEK_CUR);
                fwrite(&tmp, sizeof(struct Member), 1, binary);
            }
        }
        
        member1.pos = member2.pos + member2.size;
        member1.order = member2.order + 1;
        fseek(binary, 0, SEEK_END);
        endBinary = ftell(binary);

        //My archive got bigger, so I need to update the pointers 
        MoveData(struct2Pos, endBinary - struct2Pos, struct2Pos + sizeof(struct Member), binary);

        //The end pointer also needs to be updated
        fseek(binary, 0, SEEK_END);
        endBinary = ftell(binary);

        struct1Pos += sizeof(struct Member);

        fseek(binary, struct2Pos, SEEK_SET);
        fwrite(&member1, sizeof(struct Member), 1, binary);

        MoveData(struct1Pos, endBinary - struct1Pos, struct1Pos - sizeof(struct Member), binary);
        ftruncate(fdBinary, endBinary - sizeof(struct Member));
        
        free(buffer);
        return;
    }

    //Moving the data to the start of the function 
    MoveData(0, endBinary, member1.size, binary);

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    fseek(binary, 0, SEEK_SET);
    fwrite(buffer, member1.size, 1, binary);

    offset = member1.pos + member1.size*2;
    MoveData(offset, endBinary - offset, offset - member1.size, binary);
    
    ftruncate(fdBinary, endBinary - member1.size);
    
    //Updating the structs 
    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    for(int i = 0; i < directory.quantity; i++)
    {
        struct Member tmp;
        fread(&tmp, sizeof(struct Member), 1, binary);

        //Se o arquivo ja existe no binario 
        if(strcmp(member1.name, tmp.name) == 0)
        {
            member1.pos = 0;
            member1.order = 0;
            struct1Pos = ftell(binary);
            PrintMember(member1);
            break;
        }
        else
        {
            tmp.order++;
            tmp.pos += member1.size;
            PrintMember(tmp);
            fseek(binary, -sizeof(struct Member), SEEK_CUR);
            fwrite(&tmp, sizeof(struct Member), 1, binary);
        }
    }

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    //My archive got bigger, so I need to update the pointers 
    MoveData(firstStruct, endBinary - firstStruct, firstStruct + sizeof(struct Member), binary);
    struct1Pos += sizeof(struct Member);

    //The end pointer also needs to be updated
    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);
     

    fseek(binary, firstStruct, SEEK_SET);
    fwrite(&member1, sizeof(struct Member), 1, binary);

    MoveData(struct1Pos, endBinary - struct1Pos, struct1Pos - sizeof(struct Member), binary);

    ftruncate(fdBinary, endBinary - sizeof(struct Member));

    free(buffer);
}
void ExtractAllArchives(FILE* binary)
{
    struct Directory directory = {0};
    int pointerPos;

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    fread(&directory, sizeof(struct Directory), 1, binary);
    
    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);

    for(int i = 0; i < directory.quantity; i++)
    {
        struct Member tmp = {0};
        fseek(binary, pointerPos, SEEK_SET);
        fread(&tmp, sizeof(struct Member), 1, binary);

        pointerPos = ftell(binary);

        PrintMember(tmp);
        fprintf(stderr, "%s\n", tmp.name);

        ExtractArchive(tmp, binary);
    }

}

void ExtractArchive(struct Member member, FILE* binary)
{
    struct Directory directory = {0};
    int memSize, readBytes;
    unsigned char *bufferOut, *bufferIn;
    FILE* archive;

    archive = fopen(member.name, "wb+");
    if(!archive)
    {
        fprintf(stderr, "Error opening archive");
        return;
    }

    bufferIn = (unsigned char*) malloc(member.size);
    if(bufferIn == NULL)
    {
        printf("Error malloc\n");
        return;
    }
    fseek(binary, member.pos, SEEK_SET);

    readBytes = fread(bufferIn, member.size, 1, binary);

    //If the archive is compressed
    if(member.origSize > member.size)
    {
        bufferOut = (unsigned char*) malloc(member.origSize);
        if(bufferOut == NULL)
        {
            printf("Error malloc\n");
            return;
        }
        LZ_Uncompress(bufferIn, bufferOut, member.size);
       
        fseek(archive, 0, SEEK_SET);
        fwrite(bufferOut, member.origSize, 1, archive);

        free(bufferOut);
        free(bufferIn);
        fclose(archive);
        return;
    }

    fseek(archive, 0, SEEK_SET);
    fwrite(bufferIn, member.size, 1, archive);

    free(bufferIn);
    fclose(archive);
}

void ListMembers(FILE* binary)
{
    struct Directory directory = {0};
    struct Member member = {0};
    int memSize;

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    fread(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    for(int i = 0; i < memSize; i++)
    {
        fread(&member, sizeof(struct Member), 1, binary);
        printf("---------------------------\n");
        PrintMember(member);
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
    long int pointerPos;
    int fd, memSize;
    struct stat stats = {0};
    struct Member member = {0};
    struct Directory directory = {0};
    size_t readBytes;
    char* buffer;

    fd = fileno(archive);

    if(fd == 1 || fstat(fd, &stats)!= 0)
    {
        fprintf(stderr, "Error when getting the stats of the archive\n");
        return; 
    }

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    fread(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

    strncpy(member.name, name, sizeof(member.name)-1);
    member.UID = stats.st_uid;
    member.order = memSize;
    member.origSize = stats.st_size;
    member.modifData = stats.st_mtime;
    member.size = stats.st_size;

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);
    member.pos = pointerPos;

    //Getting the buffer
    buffer = (char*) malloc(member.size);
    readBytes = fread(buffer, 1, member.size, archive);

    if(readBytes != (size_t)member.size)
    {
        fprintf(stderr, "Error when getting the archive information\n");
        free(buffer);
        buffer = NULL;

        return;
    }

    InsertArchive(buffer, binary, member);

    free(buffer);
    buffer = NULL;
}
void InsertCompressedArchive(FILE* archive, FILE* binary, char* name)
{
    
    struct Member member = {0};
    struct stat stats = {0};
    struct Directory directory = {0};
    int memSize, fd, comprData;
    long pointerPos;
    unsigned char *bufferIn, *bufferOut;

    fd = fileno(archive);
    bufferIn = NULL;
    bufferOut = NULL;
    
    if(fd == -1 || fstat(fd, &stats) != 0)
    {
        fprintf(stderr, "Erro no stat/fd do arquivo\n");
        return;
    }

    bufferIn = (unsigned char*) malloc(stats.st_size);
    bufferOut = (unsigned char*) malloc(stats.st_size * 1.004 + 1);
    
    fread(bufferIn, stats.st_size, 1, archive);

    comprData = LZ_Compress(bufferIn, bufferOut, stats.st_size);

    printf("%d - %d\n", comprData, stats.st_size);

    if(comprData >= stats.st_size)
    {
        //Armazenar os dados descomprimidos 
        fseek(archive, 0, SEEK_SET);
        InsertNormalArchive(archive, binary, name);
        return;
    };

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    fread(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

    strncpy(member.name, name, sizeof(member.name)-1);
    member.UID = stats.st_uid;
    member.order = memSize;
    member.size = comprData;
    member.origSize = stats.st_size;
    member.modifData = stats.st_mtime;

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);
    member.pos = pointerPos;

    InsertArchive(bufferOut, binary, member);

    free(bufferIn);
    free(bufferOut);
    bufferIn = NULL;
    bufferOut = NULL;
}

void InsertArchive(void* buffer, FILE* binary, struct Member member)
{
    unsigned int totalSize;
    struct Directory directory = {0};
    int memSize, fdBinary, foundMember, size;
    long pointerPos;

    foundMember = 0;
    totalSize = 0; 
    fdBinary = fileno(binary);
    memSize = member.order;
    
    if(fdBinary == -1)
    {
        fprintf(stderr, "Erro no fd do arquivo\n");
        return;
    }

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    fread(&directory, sizeof(struct Directory), 1, binary);

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);

    for(int i = 0; i < memSize; i++)
    {
        struct Member tmp;
        int sizeDiff, origDiff;
        int firstPointer, secPointer;
        
        pointerPos = ftell(binary);
        fread(&tmp, sizeof(struct Member), 1, binary);


        //Se o arquivo ja existe no binario 
        if(strcmp(member.name, tmp.name) == 0 && foundMember == 0)
        {
            
            sizeDiff = member.size - tmp.size;
            //Because the member already exists, its position needs to be the exactly same as the other
            member.order = tmp.order;
            member.pos = tmp.pos;

            if(sizeDiff >= 0)
            {
                int pos;
                firstPointer = totalSize;

                fseek(binary, 0, SEEK_END);
                secPointer = ftell(binary);
                size = secPointer - firstPointer;
                while(size > member.size)
                {
                    pos = secPointer - member.size;
                    secPointer -= member.size;
                    MoveData(pos, member.size, pos + sizeDiff, binary);
                    size -= member.size;
                }
                pos = secPointer - size;
                MoveData(pos, size, pos + sizeDiff, binary);
                fseek(binary, totalSize, SEEK_SET);
                
                fwrite(buffer, member.size, 1, binary);
            }
            else
            {
                totalSize += tmp.size;
                firstPointer = totalSize;

                fseek(binary, 0, SEEK_END);
                secPointer = ftell(binary);

                size = secPointer - firstPointer;
                while(size > member.size)
                {
                    MoveData(firstPointer, member.size, firstPointer + sizeDiff, binary);
                    firstPointer += member.size;
                    size -= member.size;
                }
                MoveData(firstPointer, size, firstPointer + sizeDiff, binary);

                fseek(binary, totalSize - tmp.size, SEEK_SET);
                fwrite(buffer, member.size, 1, binary);

                //We need to truncate the archive now
                ftruncate(fdBinary, secPointer + sizeDiff);
            }
            //We already updated the new archive
            //Let's update the all the struct members after that
            fseek(binary, pointerPos + sizeDiff, SEEK_SET);
            fwrite(&member, sizeof(struct Member), 1, binary);
            
            origDiff = sizeDiff;
            foundMember = 1;
        }
        else if(foundMember == 1)
        {
            fseek(binary, -sizeof(struct Member), SEEK_CUR);
            tmp.pos = tmp.pos + sizeDiff;
            fwrite(&tmp, sizeof(struct Member), 1, binary);
        }

        totalSize += tmp.size;
    }

    if(foundMember == 1)
        return;

    directory.quantity += 1;
    memSize += 1;
    printf("%d - %c\n", directory.quantity, directory.name);

    //Nesse pedaco de memoria eu estou ajustando o ponteiro do arquivo
    //para escrever o membro antes do directory
    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    //Escrevendo o membro atras do diretorio
    fwrite(&member, sizeof(struct Member), 1, binary);
    //Reescrevendo o diretorio
    fwrite(&directory, sizeof(struct Directory), 1, binary);

    
    fseek(binary, 0, SEEK_END);
    int finalPointer = ftell(binary);

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);

    size = finalPointer - pointerPos;

    int pos;

    while(size > member.size)
    {
        pos = finalPointer - member.size;
        MoveData(pos, member.size, pos + member.size, binary);
        finalPointer -= member.size;
        size -= member.size;
    }
    pos = finalPointer - size;
    MoveData(pos, size, pos + member.size, binary);


    //Escrevendo conteudo do arquivo
    fseek(binary, pointerPos, SEEK_SET);
    fwrite(buffer, 1, member.size, binary);
}
