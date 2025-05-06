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
    unsigned int comprSize;
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
void moveData(long int start, long int size, long int pos, FILE* binary);

//Explain what the program does and how to use it.
void ExplainProg();

//Insert the arquive content into the binary and its information.
void InsertArquive(FILE* archive, FILE* binary, char* name);
#endif
