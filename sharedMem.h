/* 
 * File:   sharedMem.h
 * Author: John Ward
 * Reference: http://digiassn.blogspot.com/2006/07/cc-unix-ipc-shared-memory-example.html
 * Incorporated by Dean Sands
 * Created on May 6, 2011, 5:20 PM
 */

#ifndef SHAREDMEM_H
#define	SHAREDMEM_H
#include <sys/shm.h>
#include <sys/ipc.h>

#include <cstdlib>
///////////////////////////////////////////////////////////////////////////////////////
//This will allocate the Shared Memory Segment and return a pointer to that segment
namespace SharedMem{
void *getSharedMemorySegment(const int &size, const char *filename);
}



#endif	/* SHAREDMEM_H */

