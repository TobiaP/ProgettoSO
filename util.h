#ifndef UTIL_H
#define UTIL_H

#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <float.h>
#include <limits.h>
#include <math.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

struct mesg_buffer { 
    long mesg_type; 
    char mesg_text[100]; 
} message; 

#define path_pipe "/tmp/ipc/"
#define path_L "bin/Led"
#define path_B "bin/Button"
#define path_S "bin/S"
#define path_Sw "bin/switc"
#define path_C "bin/Coda"
#define path_P "bin/Proc"
#define arg_0 "1"
#define MAX_BUFF_SIZE 1024

#endif
