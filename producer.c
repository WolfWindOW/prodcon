#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <errno.h>

int main() {

    //name == "table" filename
    //SIZE == sizeof(int) == 2, file size

    //tbl == file descriptor
    //table == base address
    int tbl = shm_open("table", O_CREAT | O_RDWR, 0666);
    //Errors if unable to create a shared memory space
    if(tbl == -1) {
        printf("Shared memory failed: %s\n", strerror(errno));
        exit(1);
    }
    //Config table size
    ftruncate(tbl,sizeof(int));
    //Mapping shared memory segment to address of the process
    int *table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, tbl, 0);
    //Errors if the segment isnt able to be mapped
    if(table == MAP_FAILED) {
        printf("Map failed: %s\n", strerror(errno));
        exit(1);
    }

    //Creates three semaphores, data, empty, and mutex
    //Data is initialized with a value of 0, the amount of data starting in the table
    //Empty is intialized with a value of 2, the amount of data able to be put into the table at the beginning
    //Mutex is intialized with a value of 0, as no one is currently manipulating the CV
    sem_t *data = sem_open("data", O_CREAT, 0666, 0);
    sem_t *empty = sem_open("empty", O_CREAT, 0666, 2);
    sem_t *mutex = sem_open("mutex", O_CREAT, 0666, 1);

    int loop = 10;
    printf("\nProducer ready to create %d items. \n", loop);
    for(int i=0; i < loop; i++) {
        //Waits until the memory space has space available for manipulation
        sem_wait(empty);

        //Sleeps 1 second
        int ms = rand() % 2 + 1;
        sleep(ms);

        //Waits until the memory segment is not in use, then increments, and releases
        sem_wait(mutex);
        ++(*table);
        sem_post(mutex);

        printf("Item produced, there are now %d item(s) in the table.\n", *table);
        //Sends signal that the table has an item to be consumed
        sem_post(data);
    }

    //Closes access
    sem_close(data);
    sem_close(empty);
    sem_close(mutex);

    //Unlinks semaphores
    sem_unlink("data");
    sem_unlink("empty");
    sem_unlink("mutex");

    //Releases resources
    sem_destroy(data);
    sem_destroy(empty);
    sem_destroy(mutex);

    //Unmaps the table and unlinks.
    //Closes the table **unique to producer
    if(munmap(table, sizeof(int))==-1){
        printf("Unmap failed: %s\n", strerror(errno));
        exit(1);
    }
    if(close(tbl)==-1){
        printf("Error closing: %s\n", strerror(errno));
        exit(1);
    }
    if(shm_unlink("table")==-1){
        printf("Error removing %s: %s\n", "table", strerror(errno));
        exit(1);
    }

    printf("Producer cleaned up\n");
    //exit(1);
    return 0;
}
