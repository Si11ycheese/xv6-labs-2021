#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include <stdbool.h>

bool is_prime(int num) {
    if (num <= 1) return false;
    for (int i = 2; i * i <= num; i++) {
        if (num % i == 0) return false;
    }
    return true;
}


int 
main(int argc, char *argv[]){
    int p[2];
    pipe(p);
    if(fork()==0){
        close(p[0]);
        for (int num = 2; num < 36; num++) { 
            if (is_prime(num)) {
                write(p[1], &num, sizeof(num)); 
            }
        }
        close(p[1]);
        exit(0);
    }
    else{
        close(p[1]);
        int prime;
        while(read(p[0],&prime,sizeof(prime))!=0){
            printf("prime %d\n",prime);
        }
        close(p[0]);
        wait(0);
        exit(0);
    }
}