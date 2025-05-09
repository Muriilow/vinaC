#ifndef VINAC_H
#define VINAC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

struct Member
{
    //TODO: pos and order needs to be implemented
    char name[16];
    uid_t UID;
    int pos;
    unsigned int origSize;
    unsigned int size;
    unsigned int order;
    time_t modifData;
};
struct Directory 
{
    char name;
    int quantity;
};

//Start: the first byte to move
//End: The last byte to move
//Ref: The byte that will hold the block of information
//Archive: the archive that will have its data modified
void MoveData(long int start, long int size, long int pos, FILE* binary);

//Explain what the program does and how to use it.
void ExplainProg();

//Insert the arquive content into the binary and its information.
void InsertNormalArchive(FILE* archive, FILE* binary, char* name);

void InsertArchive(void* buffer, FILE* binary, struct Member member);

void InsertCompressedArchive(FILE* archive, FILE* binary, char* name);

//List all the members with its respective informations
void ListMembers(FILE* binary);
int CheckArchive(char* name, FILE* binary, struct Member* member);

void ExtractAllArchives(FILE* binary);
void PrintMember(struct Member member);
void ExtractArchive(struct Member member, FILE* binary);

void MoveMembers(char* name1, char* name2, FILE* binary);
#endif
