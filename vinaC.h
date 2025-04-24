#ifndef VINAC_H
#define VINAC_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "vinaC.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

struct Member
{
    //TODO: pos and order needs to be implemented
    char*  name;
    uid_t UID;
    int pos;
    unsigned int origSize;
    unsigned int comprSize;
    unsigned int order;
    time_t modifData;
};

//Explain what the program does and how to use it.
void ExplainProg();

//Insert the arquive content into the binary and its information.
void InsertArquive(FILE* arquive, FILE* binary);
#endif
