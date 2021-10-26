#include <fcntl.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/types.h>

int main() {

    //Shm_fd == file descriptor of shared memory space
    //"table" is the name of the shared memory space
    //table is the size of int, which is 2 in this instance
    int shm_fd = shm_open("table", O_RDWR, 0666);
    int *table = mmap(0, sizeof(int), PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);

    //Creates three semaphores, data, empty and mutex
    sem_t *data = sem_open("data", O_CREAT, 0666, 0);
    sem_t *empty = sem_open("empty", O_CREAT, 0666, 2);
    sem_t *mutex = sem_open("mutex", O_CREAT, 0666, 1);

    int loop = 10;
    printf("\nConsumer ready to receive %d items.\n", loop);

    for(int i = 0; i < loop; ++i) {
        //Waits for the data flag
        sem_wait(data);

        //Sleeps for anywhere from 1 to 4 seconds
        int ms = rand() % 4 + 1;
        sleep(ms);

        //Waits until the shared memory isnt in use, decrements it, then releases it
        sem_wait(mutex);
        --(*table);
        sem_post(mutex);

        printf("Item consumed, %d remaining.\n", *table);

        //Posts that there is space available in the shared memory
        sem_post(empty);
    }

    //Closes access to the semaphores
    sem_close(data);
    sem_close(empty);
    sem_close(mutex);

    //Unlinks the semaphores
    sem_unlink("data");
    sem_unlink("empty");
    sem_unlink("mutex");

    //Releases the semaphores
    sem_destroy(data);
    sem_destroy(empty);
    sem_destroy(mutex);

    //Releases and unlinks the shared memory
    munmap(table, sizeof(int));
    shm_unlink("table");

    printf("Consumer cleaned up.\n");
    //exit(1);
    return 0;
}
