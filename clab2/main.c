//
// Created by martin on 10/16/25.
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define TEMP_MAX 35
#define TEMP_MIN (-10)
#define frequency_Hz 1
#define period_s (1/frequency_Hz)



// EXERCISE 5
int main(void) {
    while (1) {
        time_t now = time(NULL);
        srand(now);
        float r = (float)rand() / RAND_MAX * (TEMP_MAX - TEMP_MIN) + TEMP_MIN;
        char timestr[64];
        strftime(timestr, sizeof(timestr), "%Y-%m-%d %H:%M:%S", localtime(&now));
        printf("Temperature = %1.2f @%s \n", r, timestr);
        sleep(period_s);
    }
    return 0;
}

//EXERCISE 4

// void swap_pointers( void **pp , void **pq ) {
//     /*
//      * POWER OF POINTERS: WHEN WORKING WITH HELPER FUNCTIONS WHICH ARE MEANT TO MODIFY THE MEMORY OF MAIN (i.e. THe values of certain variables in main),
//      * ONE CAN PASS A POINTER TO THE VARIABLE WE WANT TO CHANGE IN "main()" AS PARAMETER AND THEN ACCESS THE CONTENT OF THIS POINTER IN THE HELPER FUNCTION.
//      */
//     void *copy_p = *pp; // Copy necessary because if you omit, then pointer p is already overwritten with q, and the old value of p is lost.
//     void *copy_q = *pq;
//     *pp = copy_q;
//     *pq = copy_p;
// }
//
//  * int main( void ) {
//     int a = 1;
//     int b = 2;
//     // for testing we use pointers to integers
//
//     /* STATIC MEMORY ALLOCATION (STACK) */
//     int *p = &a;
//     int *q = &b;
//
//     /* DYNAMIC MEMORY ALLOCATION (HEAP): ADDRESSES ARE MUCH HIGHER
//     int *p = malloc(sizeof(int));
//     int *q = malloc(sizeof(int));
//     *p = a;
//     *q = b;
//      */
//
//     printf("address of p = %p and q = %p\n", p, q);
//     // prints p = &a and q = &b
//     swap_pointers(&p , &q );
//     printf("address of p = %p and q = %p\n", p, q);
//     // prints p = &b and q = &a
//      // free(p)
//      // free(q)
// }