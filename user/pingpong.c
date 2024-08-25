#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"


int
main(int argc, char *argv[]){
    int p[2];
    pipe(p);
    if(fork()==0){
        printf("%d: received ping\n",getpid());
        close(p[0]);
        write(p[1],argv,argc);
        close(p[1]);
        exit(0);
    }
    else{
        wait(0);
        close(p[1]);
        read(p[0],argv,argc);
        close(p[0]);
        printf("%d: received pong\n",getpid());
        exit(0);

    }

}