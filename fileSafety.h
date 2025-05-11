#ifndef SAFETY_H
#define SAFETY_H

#include <stdio.h> 
#include <stdlib.h> 

/*
 * Write the file checking for any errors to avoid problems.
 * buffer: Content being saved;
 * size: Size of the content being saved;
 * count: Number of "sizes" being wrote;
 * file: File that is going to be edited;
*/
void SafeWriteFile(void *buffer, size_t size, size_t count, FILE *file);

/*
 * Read the file checking for any errors to avoid problems.
 * buffer: Content going to save the data;
 * size: Size of the content being read;
 * count: Number of "sizes" being read;
 * file: File that is going to be read;
*/
void SafeReadFile(void *buffer, size_t size, size_t count, FILE *file);

/*
 * Allocate memory checking for any errors.
 * size: The size of data being alllocated;
*/
void* SafeMalloc(size_t size);
#endif
