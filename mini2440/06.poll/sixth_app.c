#include <stdio.h>
#include <fcntl.h>      /* O_RDWR */
#include <pthread.h>    /* pthread_create */
#include <stdlib.h>

#define RW_BUFF_SIZE (5)
#define NUM_THREADS 2

/* 线程函数 */
void *wirite_share_mem(void *threadid)
{
        long tid;
        tid = (long)threadid;

        unsigned char *wrbuf;
        unsigned char *rdbuf;
        int row_count = 32;
        int i;
        int fd;

        wrbuf = malloc(RW_BUFF_SIZE);
        rdbuf = malloc(RW_BUFF_SIZE);

        fd = open("/dev/sixth_drv_device", O_RDWR);
        if(fd < 0)
        {
                printf("[sixth_app] share mem node open fail\n");
                return -1;
        }

        /* 线程0作为读线程，线程1作为写线程 */
        switch(tid)
        {
                case 1:
                        for(i = 0; i < RW_BUFF_SIZE; i++)
                                *(wrbuf + i) = tid + 'A';
                        lseek(fd, 0, 0);
                        write(fd, wrbuf, RW_BUFF_SIZE);
                        break;
                case 0:
                        lseek(fd, 0, 0);
                        read(fd, rdbuf, RW_BUFF_SIZE);
                        for(i = 0; i < RW_BUFF_SIZE; i++)
                        {
                                if(i == 0)
                                        printf("%*d:", 2, 1);
                                printf("%c  ", *(rdbuf + i));
                                if(i % row_count == (row_count - 1))
                                        printf("\n%*d:", 2, i / row_count + 2);
                        }
                        printf("\n");
        }

        close(fd);
        pthread_exit(NULL);
}

int main(int argc, char *argv[])
{
        pthread_t threads[NUM_THREADS];
        int rc;
        long t;

        /* 循环创建 2 个线程 */
        for(t = 0 ; t < NUM_THREADS; t++)
        {
                printf("[sixth_app] creating thread %ld\n", t);
                rc = pthread_create(&threads[t], NULL, wirite_share_mem, (void *)t);
                if (rc)
                {
                        printf("[sixth_app] ERROR; return code from pthread_create() is %d\n", rc);
                        exit(-1);
                }
                sleep(2);
        }
        printf("[sixth_app] main: exit!\n");
        pthread_exit(NULL);
        return 0;
}
