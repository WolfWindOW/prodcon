# prodcon
 ## Producer consumer problem in C, using semaphores and shared memory. 
 
 ### Installation
 In order to compile, gcc must be installed on the chosen environment. The code is intended to be run on a linux based system. Before trying to install, it is recommended that it is ensure that gcc isn't already installed on the system. This can be done primarily by running the command 
 > gcc -v
 
 Installation can be done by running the following command for Debian or Ubuntu:
>sudo apt install build-essential

For any other Linux based systems, look it up yourself.

### Compilation
Compilation can be completed by running the following command with the "producer.c" and "consumer.c" files in the working directory.
>gcc producer.c -pthread -lrt -o producer
>gcc consumer.c -pthread -lrt -o consumer

If successful, two additional files will be created, called "producer" and "consumer".

### Running the program
In order to run the program, the following command must be used:
>./producer & ./consumer &
 
 ### Output
 Producer will create two items, then wait for the Consumer to consume an item in the shared memory, sending a flag to the Producer that it may produce another item. This will continue until 10 items are created and consumed.
 
 ![Producer Consumer Output](https://github.com/WolfWindOW/prodcon/blob/main/Prodcon%20outout.jpg)
 
