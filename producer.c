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
    //int *ptr, movable

    int tbl = shm_open("table", O_CREAT | O_RDWR, 0666);
    if(tbl == -1) {
        printf("Shared memory failed: %s\n", strerror(errno));
        exit(1);
    }
    //Config table size
    ftruncate(tbl,sizeof(int));
    //Mapping shared memory segment to address of the process
    int *table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, tbl, 0);
    if(table == MAP_FAILED) {
        printf("Map failed: %s\n", strerror(errno));
        exit(1);
    }

    sem_t *fill = sem_open("fill", O_CREAT, 0666, 0);
    sem_t *avail = sem_open("available", O_CREAT, 0666, 2);
    sem_t *mutex = sem_open("mutex", O_CREAT, 0666, 1);

    int loop = 10;
    printf("\nProducer ready to create %d items. \n", loop);
    for(int i=0; i < loop; i++) {
        sem_wait(avail);
        //printf("1");
        int ms = rand() % 2 + 1;
        sleep(ms);
        //printf("2");
        sem_wait(mutex);
        ++(*table);
        sem_post(mutex);

        printf("Item produced, there are now %d item(s) in the table.\n", *table);
        sem_post(fill);
    }

    sem_close(fill);
    sem_close(avail);
    sem_close(mutex);

    sem_unlink("fill");
    sem_unlink("available");
    sem_unlink("mutex");

    sem_destroy(fill);
    sem_destroy(avail);
    sem_destroy(mutex);

    munmap(table, sizeof(int));
    close(tbl);
    shm_unlink("table");

    printf("Producer cleaned up\n");
    exit(1);
    return 0;
}
