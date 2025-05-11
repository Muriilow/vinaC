#include "vinaC.h"
#include "fileSafety.h"
#include "lz.h"

/*-----------Print functions-----------*/

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

void PrintMember(struct Member member)
{
    char buffer[80];
    struct tm *timeInfo = localtime(&member.modifData);

    if (timeInfo == NULL) 
    {
        fprintf(stderr, "Error formating the modification time\n");
        exit(EXIT_FAILURE);
    }

    /* Formatting the data text */
    strftime(buffer, sizeof(buffer), "%d/%m/%Y %H:%M:%S", timeInfo);


    printf("Name: %s \n", member.name);
    printf("Order: %d\n", member.order);
    printf("Position: %d\n", member.pos);
    printf("UID: %d\n", member.UID);
    printf("Original size: %d\n", member.origSize);
    printf("Disk size: %d\n", member.size);
    printf("Modified data: %s\n", buffer);
};

void ListMembers(FILE* binary)
{
    struct Directory directory = {0};
    struct Member member = {0};
    int memSize;

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    SafeReadFile(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

    /* Going to the first struct and iterate through everything */
    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    for(int i = 0; i < memSize; i++)
    {
        SafeReadFile(&member, sizeof(struct Member), 1, binary);
        printf("---------------------------\n");
        PrintMember(member);
    }

    printf("---------------------------\n");
};

/*-----------Support functions-----------*/

/*
 * Move the binary data to the specified position.
 * start: The position that starts the piece of data going to be moved;
 * size: Size of the data going to be moved;
 * pos: The position that data is going to be moved;
 * binary: The file going to be modified;
 * */
void MoveData(long int start, long int size, long int pos, FILE* binary)
{
    char* buffer = SafeMalloc(size);

    fseek(binary, start , SEEK_SET);
    SafeReadFile(buffer, 1, size, binary);

    fseek(binary, pos, SEEK_SET);
    SafeWriteFile(buffer, 1, size, binary);
    free(buffer);
};

/*
 * Moving in pieces the binary data to the specified position.
 * The buffer will only have a limited size.
 * start: The position that starts the piece of data going to be moved;
 * size: Size of the data going to be moved;
 * pos: The position that data is going to be moved;
 * maxSize: The maximum size that the buffer holds;
 * binary: The file going to be modified;
 * */
void MoveFixedData(long int start, long int size, long int pos, long int maxSize, FILE* binary)
{
    int end, sizeDiff;

    //If we want to move the data backwards 
    if(start > pos)
    {
        while(size > maxSize)
        {
            MoveData(start, maxSize, pos, binary);
            start += maxSize;
            pos += maxSize;
            size -= maxSize;
        }
        MoveData(start, size, pos, binary);
        return;
    }
    
    //If we want to move the data forward
    end = start + size;
    sizeDiff = pos - start;

    while(size > maxSize)
    {
        pos = end - maxSize;
        end -= maxSize;
        MoveData(pos, maxSize, pos + sizeDiff, binary);
        size -= maxSize;
    }
    pos = end - size;
    MoveData(pos, size, pos + sizeDiff, binary);
};

/*
 * Write the buffer and struct into the binary.
 * buffer: Pointer that is going to have it's information saved;
 * binary: The binary archive going to be modified;
 * member: The struct going to be saved;
*/
void InsertArchive(void* buffer, FILE* binary, struct Member member)
{
    unsigned int totalSize;
    struct Directory directory = {0};
    int memSize, fdBinary, foundMember, size, finalPointer;
    long pointerPos;

    foundMember = 0;
    totalSize = 0; 
    fdBinary = fileno(binary);
    memSize = member.order;
    
    if(fdBinary == -1)
    {
        fprintf(stderr, "Error getting file descriptor\n");
        return;
    }

    /* Getting the directory */
    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    SafeReadFile(&directory, sizeof(struct Directory), 1, binary);

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);

    for(int i = 0; i < memSize; i++)
    {
        struct Member tmp;
        int sizeDiff;
        int firstPointer, secPointer;
        
        pointerPos = ftell(binary);
        SafeReadFile(&tmp, sizeof(struct Member), 1, binary);


        /* If the archive already exists in the binary, we are going to replace the old one */ 
        if(strcmp(member.name, tmp.name) == 0 && foundMember == 0)
        {
            sizeDiff = member.size - tmp.size;

            member.order = tmp.order;
            member.pos = tmp.pos;

            fseek(binary, 0, SEEK_END);
            secPointer = ftell(binary);

            /* If the archive increased */
            if(sizeDiff >= 0)
            {
                firstPointer = totalSize;

                size = secPointer - firstPointer;

                MoveFixedData(firstPointer, size, firstPointer + sizeDiff, member.size, binary);
                fseek(binary, totalSize, SEEK_SET);
                
                SafeWriteFile(buffer, member.size, 1, binary);
            }
            else
            {
                /* The archive got smaller */
                totalSize += tmp.size;
                firstPointer = totalSize;

                size = secPointer - firstPointer;

                MoveFixedData(firstPointer, size, firstPointer + sizeDiff, member.size, binary);

                fseek(binary, totalSize - tmp.size, SEEK_SET);

                SafeWriteFile(buffer, member.size, 1, binary);

                /* We need to truncate the archive now */
                ftruncate(fdBinary, secPointer + sizeDiff);
            }

            /* Writing the new struct */
            fseek(binary, pointerPos + sizeDiff, SEEK_SET);
            SafeWriteFile(&member, sizeof(struct Member), 1, binary);
            
            origDiff = sizeDiff;
            foundMember = 1;
        }
        else if(foundMember == 1)
        {
            /* We already updated the new archive
            *  Let's update the all the struct members after that */
            fseek(binary, -sizeof(struct Member), SEEK_CUR);
            tmp.pos = tmp.pos + sizeDiff;
            SafeWriteFile(&tmp, sizeof(struct Member), 1, binary);
        }

        totalSize += tmp.size;
    }

    /* We already replaced the member */
    if(foundMember == 1)
        return;

    /* The member is new, let's write it in the end of the archive */
    directory.quantity += 1;
    memSize += 1;

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    SafeWriteFile(&member, sizeof(struct Member), 1, binary);

    /* Updating directory */
    SafeWriteFile(&directory, sizeof(struct Directory), 1, binary);

    
    fseek(binary, 0, SEEK_END);
    finalPointer = ftell(binary);

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);

    size = finalPointer - pointerPos;

    /* Opening space to put the new member content */
    MoveFixedData(pointerPos,
            sizeof(struct Directory) + memSize * sizeof(struct Member),
            pointerPos + member.size,
            member.size,
            binary);

    fseek(binary, pointerPos, SEEK_SET);
    SafeWriteFile(buffer, 1, member.size, binary);
};

/*
 * Check if the binary has *this* archive saved on it,
 * if it has return the member.
 * name: Name of the archive we are searching for;
 * binary: The file that is going to be checked;
 * member: The address to get the member;
 * RETURN: 0 if the archive exist, 1 if it doesn't;
*/
int CheckArchive(char* name, FILE* binary, struct Member *member)
{
    struct Directory directory = {0};
    int memSize;


    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    fread(&directory, sizeof(struct Directory), 1, binary);
    
    memSize = directory.quantity;

    /* Loop through all the members and check if any has the name */
    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);

    for(int i = 0; i < memSize; i++)
    {
        struct Member tmp;
        fread(&tmp, sizeof(struct Member), 1, binary);

        if(strcmp(name, tmp.name) == 0)
        {
            /* We found the member, let's assign the pointer */
            *member = tmp; 
            return 0;
        }
    }
    
    fprintf(stderr, "Archive doesn't exist...\n");
    return 1;
};

/*-----------Main functions-----------*/
void RemoveMember(char* name, FILE* binary)
{
    struct Member member = {0};
    struct Directory directory = {0};
    int fdBinary, endBinary, isAfter, structPos, offset;

    if(CheckArchive(name, binary, &member) != 0)
        return;

    fdBinary = fileno(binary);

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);


    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    SafeReadFile(&directory, sizeof(struct Directory), 1, binary);
    
    /* Move remove the member content */
    offset = member.pos + member.size;

    fseek(binary, member.pos, SEEK_SET);
    MoveFixedData(offset, endBinary - offset, member.pos, member.size, binary);

    ftruncate(fdBinary, endBinary - member.size);

    /* Iterate through all the structs and modify the structs after the one being destroyed */
    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    isAfter = 0;

    for(int i = 0; i < directory.quantity; i++)
    {
        struct Member tmp;

        SafeReadFile(&tmp, sizeof(struct Member), 1, binary);

        if(strcmp(member.name, tmp.name) == 0)
        {
            structPos = ftell(binary);
            isAfter = 1;
            continue;
        }
        if(isAfter == 1)
        {
            /* If the struct is after the one being destroyed, change its position*/
            tmp.order--;
            tmp.pos -= member.size;
            fseek(binary, -sizeof(struct Member), SEEK_CUR);
            SafeWriteFile(&tmp, sizeof(struct Member), 1, binary);
        }
    }

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    /* Deleting the struct */
    MoveFixedData(structPos, endBinary - structPos, structPos - sizeof(struct Member), member.size, binary);
    ftruncate(fdBinary, endBinary - sizeof(struct Member));

    /* The member count decreased, so let's change the directory */
    directory.quantity -= 1;
    fseek(binary, -sizeof(struct Directory), SEEK_END);
    SafeWriteFile(&directory, sizeof(struct Directory), 1, binary);
};

void MoveMembers(char* name1, char* name2, FILE* binary)
{
    struct Member member1 = {0};
    struct Member member2 = {0};
    struct Directory directory = {0};
    int firstStruct, struct1Pos, struct2Pos;
    int endBinary, fdBinary, offset, isAfter;
    unsigned char *buffer;

    /*Check if the members exist and getting the information */
    if(CheckArchive(name1, binary, &member1) == 0)
    {
        if(name2 != NULL)
        {
            if(CheckArchive(name2, binary, &member2) != 0)
            {
                fprintf(stderr, "%s doesn't exist\n", name2);
                exit(EXIT_FAILURE);
            }
        }
    }
    
    /* Getting some information */
    fdBinary = fileno(binary);

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    fseek(binary, member1.pos, SEEK_SET);
    member1Pos = ftell(binary);

    buffer = SafeMalloc(member1.size);
    SafeReadFile(buffer, member1.size, 1, binary);

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    SafeReadFile(&directory, sizeof(struct Directory), 1, binary);

    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    firstStruct = ftell(binary);

    /* If the user has passed a second member to move infront */
    if(name2 != NULL)
    {
        /* Opening space after the member2 to write member1 */
        offset = member2.pos + member2.size;
        MoveFixedData(offset, endBinary - offset, offset + member1.size, member1.size, binary);

        fseek(binary, offset, SEEK_SET);
        SafeWriteFile(buffer, member1.size, 1, binary);

        fseek(binary, 0, SEEK_END);
        endBinary = ftell(binary);
        
        /* If the member1 was infront of member2 */
        if(member1.order > member2.order)
        {
            offset = member1.pos + member1.size*2;
            MoveFixedData(offset, endBinary - offset, member1.pos + member1.size, member1.size, binary);
        }
        else 
        {
            offset = member1.pos + member1.size;
            MoveFixedData(offset, endBinary - offset, member1.pos, member1.size, binary);
        }

        ftruncate(fdBinary, endBinary - member1.size);

        /* Updating the structs */
        fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
        isAfter = 0;

        for(int i = 0; i < directory.quantity; i++)
        {
            struct Member tmp;

            SafeReadFile(&tmp, sizeof(struct Member), 1, binary);

            /* Checking the order again */
            if(member1.order> member2.order)
            {
                if(strcmp(member2.name, tmp.name) == 0)
                {
                    struct2Pos = ftell(binary);
                    isAfter =1;
                    continue;
                }
                if(strcmp(member1.name, tmp.name) == 0)
                {
                    struct1Pos = ftell(binary);
                    struct1Pos += sizeof(struct Member);

                    member1.order = member2.order + 1;
                    isAfter = 0;
                    continue;
                }
                /* Changing all the members between member1 and member2 */
                if(isAfter == 1)
                {
                    tmp.order++;
                    tmp.pos += member1.size;

                    fseek(binary, -sizeof(struct Member), SEEK_CUR);
                    SafeWriteFile(&tmp, sizeof(struct Member), 1, binary);
                }
            }
            else
            {
                if(strcmp(member2.name, tmp.name) == 0)
                {
                    struct2Pos = ftell(binary);

                    member2.order -= 1;
                    member2.pos -= member1.size;

                    isAfter = 0;

                    fseek(binary, -sizeof(struct Member), SEEK_CUR);
                    SafeWriteFile(&member2, sizeof(struct Member), 1, binary);
                    continue;
                }
                if(strcmp(member1.name, tmp.name) == 0)
                {
                    struct1Pos = ftell(binary);
                    member1.order = member2.order;
                    isAfter =1;

                    continue;
                }
                /* Changing all the members between member1 and member2 */
                if(isAfter == 1)
                {
                    tmp.order--;
                    tmp.pos -= member1.size;

                    fseek(binary, -sizeof(struct Member), SEEK_CUR);
                    SafeWriteFile(&tmp, sizeof(struct Member), 1, binary);
                }

            }
        }
        
        member1.pos = member2.pos + member2.size;

        fseek(binary, 0, SEEK_END);
        endBinary = ftell(binary);

        /* My archive got bigger, so I need to update the pointers  */
        MoveFixedData(struct2Pos, endBinary - struct2Pos, struct2Pos + sizeof(struct Member), member1.size, binary);

        /* The end pointer also needs to be updated */
        fseek(binary, 0, SEEK_END);
        endBinary = ftell(binary);


        fseek(binary, struct2Pos, SEEK_SET);
        SafeWriteFile(&member1, sizeof(struct Member), 1, binary);

        MoveFixedData(struct1Pos, endBinary - struct1Pos, struct1Pos - sizeof(struct Member), member1.size, binary);
        ftruncate(fdBinary, endBinary - sizeof(struct Member));
        
        free(buffer);
        return;
    }

    /* Moving the data to the start of the function */
    MoveFixedData(0, endBinary, member1.size, member1.size, binary);

    /* Getting some information */
    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    fseek(binary, 0, SEEK_SET);
    SafeWriteFile(buffer, member1.size, 1, binary);

    offset = member1.pos + member1.size*2;
    MoveFixedData(offset, endBinary - offset, offset - member1.size, member1.size, binary);
    
    ftruncate(fdBinary, endBinary - member1.size);
    
    /* Updating the structs */
    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    for(int i = 0; i < directory.quantity; i++)
    {
        struct Member tmp;
        SafeReadFile(&tmp, sizeof(struct Member), 1, binary);

        /* If the archive already exists in the binary */
        if(strcmp(member1.name, tmp.name) == 0)
        {
            member1.pos = 0;
            member1.order = 0;

            struct1Pos = ftell(binary);
            break;
        }
        else
        {
            tmp.order++;
            tmp.pos += member1.size;

            fseek(binary, -sizeof(struct Member), SEEK_CUR);
            SafeWriteFile(&tmp, sizeof(struct Member), 1, binary);
        }
    }

    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    /* My archive got bigger, so I need to update the pointers */
    MoveFixedData(firstStruct, endBinary - firstStruct, firstStruct + sizeof(struct Member), member1.size, binary);
    struct1Pos += sizeof(struct Member);

    /* The end pointer also needs to be updated */
    fseek(binary, 0, SEEK_END);
    endBinary = ftell(binary);

    fseek(binary, firstStruct, SEEK_SET);
    SafeWriteFile(&member1, sizeof(struct Member), 1, binary);

    MoveFixedData(struct1Pos, endBinary - struct1Pos, struct1Pos - sizeof(struct Member), member1.size, binary);

    ftruncate(fdBinary, endBinary - sizeof(struct Member));

    free(buffer);
};

void ExtractAllArchives(FILE* binary)
{
    struct Directory directory = {0};
    int pointerPos;

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);
    SafeReadFile(&directory, sizeof(struct Directory), 1, binary);
    
    fseek(binary, -(sizeof(struct Directory) + directory.quantity * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);
    
    /* Iterate through all the members and extract them */
    for(int i = 0; i < directory.quantity; i++)
    {
        struct Member tmp = {0};

        fseek(binary, pointerPos, SEEK_SET);
        SafeReadFile(&tmp, sizeof(struct Member), 1, binary);

        pointerPos = ftell(binary);

        ExtractArchive(tmp.name, binary);
    }
};

void ExtractArchive(char* name, FILE* binary)
{
    struct Member member = {0};
    unsigned char *bufferOut, *bufferIn;
    FILE* archive;

    if(CheckArchive(name, binary, &member) != 0)
        return;

    archive = fopen(member.name, "wb+");
    if(!archive)
    {
        fprintf(stderr, "Error opening archive\n");
        return;
    }

    bufferIn = SafeMalloc(member.size);

    fseek(binary, member.pos, SEEK_SET);

    SafeReadFile(bufferIn, member.size, 1, binary);

    /* If the archive is compressed */
    if(member.origSize > member.size)
    {
        /* Uncompress it */
        bufferOut = SafeMalloc(member.origSize);
        LZ_Uncompress(bufferIn, bufferOut, member.size);
       
        fseek(archive, 0, SEEK_SET);
        SafeWriteFile(bufferOut, member.origSize, 1, archive);

        free(bufferOut);
        free(bufferIn);
        fclose(archive);
        return;
    }

    /* Write the buffer into the file */
    fseek(archive, 0, SEEK_SET);
    SafeWriteFile(bufferIn, member.size, 1, archive);

    free(bufferIn);
    fclose(archive);
};

void InsertNormalArchive(FILE* archive, FILE* binary, char* name)
{
    long int pointerPos;
    int fd, memSize;
    struct stat stats = {0};
    struct Member member = {0};
    struct Directory directory = {0};
    char* buffer;

    fd = fileno(archive);
    
    /* Using fstat() to get the file description/info */
    if(fd == 1 || fstat(fd, &stats)!= 0)
    {
        fprintf(stderr, "Error when getting the stats of the archive\n");
        return; 
    }

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    SafeReadFile(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

    /* Creating the member */
    strncpy(member.name, name, sizeof(member.name)-1);
    member.UID = stats.st_uid;
    member.order = memSize;
    member.origSize = stats.st_size;
    member.modifData = stats.st_mtime;
    member.size = stats.st_size;

    fseek(binary, -(sizeof(struct Directory) + memSize * sizeof(struct Member)), SEEK_END);
    pointerPos = ftell(binary);
    member.pos = pointerPos;

    /* Getting the buffer */
    buffer = SafeMalloc(member.size);
    
    SafeReadFile(buffer, 1, member.size, archive);

    InsertArchive(buffer, binary, member);

    free(buffer);
    buffer = NULL;
};

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
    
    /* Using fstat() to get the file description/info */
    if(fd == -1 || fstat(fd, &stats) != 0)
    {
        fprintf(stderr, "Erro getting stats or file descriptor\n");
        return;
    }
    
    /* Allocing both buffers */
    bufferIn = SafeMalloc(stats.st_size);
    bufferOut = SafeMalloc(stats.st_size * 1.004 + 1);
    
    SafeReadFile(bufferIn, stats.st_size, 1, archive);

    comprData = LZ_Compress(bufferIn, bufferOut, stats.st_size);

    if(comprData >= stats.st_size)
    {
        //Armazenar os dados descomprimidos 
        fseek(archive, 0, SEEK_SET);
        InsertNormalArchive(archive, binary, name);

        free(bufferIn);
        free(bufferOut);
        bufferIn = NULL;
        bufferOut = NULL;
        return;
    };

    fseek(binary, -(sizeof(struct Directory)), SEEK_END);

    SafeReadFile(&directory, sizeof(struct Directory), 1, binary);
    memSize = directory.quantity;

    /* Creating member */
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
};
