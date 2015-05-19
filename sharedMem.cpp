/* 
 * File:   sharedMem.cpp
 * Author: John Ward
 * Reference: http://digiassn.blogspot.com/2006/07/cc-unix-ipc-shared-memory-example.html
 * Incorporated by Dean M. Sands, III of Team DDD
 * Created on May 6, 2011, 5:20 PM
 */

#include "sharedMem.h"
void *SharedMem::getSharedMemorySegment(const int &size, const char *filename){
     //Variables for our Shared Memory Segment
     key_t key;
     int shmid;
     int err = 0;

     //This is our local pointer to the shared memory segment
     void *temp_shm_pointer;

     //Create our shared memory key for client connections
     if ((key = ftok(filename, 1 + (rand() % 255) )) == -1) 
                //error("Failed to FTOK");
          err = 1;
     
     //Connect to the shared memory segment and get the shared memory ID
     if ((shmid = shmget(key, size, IPC_CREAT | 0600)) == -1) 
     //error("shmget attempt failed");
     err = 1;
   
     // point the clients to the segment 
     temp_shm_pointer = (void *)shmat(shmid, 0, 0);
     if (temp_shm_pointer == (void *)(-1))
        //error("shmat failure");
     err = 1;

     //Return the pointer
     if (!err)
          return temp_shm_pointer;
     else
     return 0;
}

