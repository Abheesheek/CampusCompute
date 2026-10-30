#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/resource.h>
#include <unistd.h>
#include <time.h>
#include "cc_features.h"
#define CAP 4
#define EVENTS 12
typedef struct {int data[CAP],head,tail,count,done;pthread_mutex_t lock;pthread_cond_t not_full,not_empty;FILE *file;} LogQueue;
static void *producer(void *arg){LogQueue *q=arg;for(int i=1;i<=EVENTS;i++){pthread_mutex_lock(&q->lock);while(q->count==CAP)pthread_cond_wait(&q->not_full,&q->lock);q->data[q->tail]=i;q->tail=(q->tail+1)%CAP;q->count++;printf("Producer queued event %d (queue=%d)\n",i,q->count);pthread_cond_signal(&q->not_empty);pthread_mutex_unlock(&q->lock);}pthread_mutex_lock(&q->lock);q->done=1;pthread_cond_broadcast(&q->not_empty);pthread_mutex_unlock(&q->lock);return NULL;}
static void *consumer(void *arg){LogQueue *q=arg;for(;;){pthread_mutex_lock(&q->lock);while(q->count==0&&!q->done)pthread_cond_wait(&q->not_empty,&q->lock);if(q->count==0&&q->done){pthread_mutex_unlock(&q->lock);break;}int e=q->data[q->head];q->head=(q->head+1)%CAP;q->count--;fprintf(q->file,"Consumed event %d\n",e);printf("Consumer logged event %d (queue=%d)\n",e,q->count);pthread_cond_signal(&q->not_full);pthread_mutex_unlock(&q->lock);}return NULL;}
int demo_logger(void){LogQueue q={0};pthread_t p,c;int p_started=0,c_started=0;int rc=0;q.file=fopen("campuscompute.log","w");if(!q.file){perror("fopen");return -1;}if(pthread_mutex_init(&q.lock,NULL)){fclose(q.file);return -1;}if(pthread_cond_init(&q.not_full,NULL)){pthread_mutex_destroy(&q.lock);fclose(q.file);return -1;}if(pthread_cond_init(&q.not_empty,NULL)){pthread_cond_destroy(&q.not_full);pthread_mutex_destroy(&q.lock);fclose(q.file);return -1;}
 if(pthread_create(&c,NULL,consumer,&q)==0)c_started=1;else rc=-1;
 if(c_started&&pthread_create(&p,NULL,producer,&q)==0)p_started=1;else rc=-1;
 if(!p_started){pthread_mutex_lock(&q.lock);q.done=1;pthread_cond_broadcast(&q.not_empty);pthread_mutex_unlock(&q.lock);}if(p_started)pthread_join(p,NULL);if(c_started)pthread_join(c,NULL);
 pthread_cond_destroy(&q.not_empty);pthread_cond_destroy(&q.not_full);pthread_mutex_destroy(&q.lock);fclose(q.file);printf("Logger threads joined; output: campuscompute.log\n");return rc;}
int demo_ipc(void){char shm_name[80],sem_name[80];snprintf(shm_name,sizeof shm_name,"/cc_shm_%ld",(long)getpid());snprintf(sem_name,sizeof sem_name,"/cc_sem_%ld",(long)getpid());int fd=-1,rc=-1,created_shm=0,created_sem=0;char *mem=MAP_FAILED;sem_t *sem=SEM_FAILED;pid_t pid=-1;
 fd=shm_open(shm_name,O_CREAT|O_EXCL|O_RDWR,0600);if(fd<0){perror("shm_open");goto cleanup;}created_shm=1;if(ftruncate(fd,256)<0){perror("ftruncate");goto cleanup;}mem=mmap(NULL,256,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);if(mem==MAP_FAILED){perror("mmap");goto cleanup;}sem=sem_open(sem_name,O_CREAT|O_EXCL,0600,0);if(sem==SEM_FAILED){perror("sem_open");goto cleanup;}created_sem=1;pid=fork();if(pid<0){perror("fork");goto cleanup;}if(pid==0){snprintf(mem,256,"Shared message from child PID %ld",(long)getpid());if(sem_post(sem)<0)_exit(2);_exit(0);}
 while(sem_wait(sem)<0){if(errno!=EINTR){perror("sem_wait");goto cleanup;}}printf("Parent PID %ld read: %s\n",(long)getpid(),mem);int status;if(waitpid(pid,&status,0)<0){perror("waitpid");pid=-1;goto cleanup;}pid=-1;if(!WIFEXITED(status)||WEXITSTATUS(status)!=0){fprintf(stderr,"IPC child failed\n");goto cleanup;}rc=0;
cleanup:if(pid>0)waitpid(pid,NULL,0);if(sem!=SEM_FAILED)sem_close(sem);if(created_sem){if(sem_unlink(sem_name)<0)perror("sem_unlink");else printf("Semaphore unlinked\n");}if(mem!=MAP_FAILED)munmap(mem,256);if(fd>=0)close(fd);if(created_shm){if(shm_unlink(shm_name)<0)perror("shm_unlink");else printf("Shared memory unlinked\n");}return rc;}
/* Standard 5-process, 3-resource Banker's example. */
int demo_banker(int safe){const int n=5,m=3;int allocation[5][3]={{0,1,0},{2,0,0},{3,0,2},{2,1,1},{0,0,2}};int maximum[5][3]={{7,5,3},{3,2,2},{9,0,2},{2,2,2},{4,3,3}};int available[3]={3,3,2};if(!safe){available[0]=0;available[1]=0;available[2]=0;}int work[3],finished[5]={0},sequence[5],count=0;memcpy(work,available,sizeof work);printf("Available: [%d %d %d]\n",work[0],work[1],work[2]);printf("Process | Allocation | Maximum | Need\n");for(int i=0;i<n;i++)printf("P%d | %d %d %d | %d %d %d | %d %d %d\n",i,allocation[i][0],allocation[i][1],allocation[i][2],maximum[i][0],maximum[i][1],maximum[i][2],maximum[i][0]-allocation[i][0],maximum[i][1]-allocation[i][1],maximum[i][2]-allocation[i][2]);
 while(count<n){int progress=0;for(int i=0;i<n;i++){if(finished[i])continue;int can=1;for(int j=0;j<m;j++)if(maximum[i][j]-allocation[i][j]>work[j])can=0;if(can){for(int j=0;j<m;j++)work[j]+=allocation[i][j];finished[i]=1;sequence[count++]=i;progress=1;}}if(!progress)break;}
 if(count==n){printf("SAFE state. Safe sequence:");for(int i=0;i<n;i++)printf(" P%d",sequence[i]);printf("\n");return 0;}printf("UNSAFE state: no complete safe sequence exists (not necessarily deadlocked).\n");return 0;}
int demo_worker(int fail){pid_t pid=fork();if(pid<0){perror("fork");return -1;}if(pid==0){printf("Child PID %ld started; %s\n",(long)getpid(),fail?"SIMULATED failure":"normal completion");fflush(stdout);_exit(fail?42:0);}int status;if(waitpid(pid,&status,0)<0){perror("waitpid");return -1;}if(WIFEXITED(status))printf("Parent detected child %ld exit code %d (%s)\n",(long)pid,WEXITSTATUS(status),fail?"controlled simulation":"normal run");else if(WIFSIGNALED(status))printf("Parent detected signal %d\n",WTERMSIG(status));return 0;}
int demo_limit(void){pid_t pid=fork();if(pid<0){perror("fork");return -1;}if(pid==0){struct rlimit old,limit;if(getrlimit(RLIMIT_NOFILE,&old)<0){perror("getrlimit");_exit(2);}limit=old;if(limit.rlim_cur>32)limit.rlim_cur=32;if(setrlimit(RLIMIT_NOFILE,&limit)<0){perror("setrlimit");_exit(3);}printf("Child RLIMIT_NOFILE soft limit: %llu (original: %llu)\n",(unsigned long long)limit.rlim_cur,(unsigned long long)old.rlim_cur);int fds[128],count=0;while(count<128){int fd=open("/dev/null",O_RDONLY);if(fd<0){printf("open() stopped after %d extra descriptors: %s\n",count,strerror(errno));break;}fds[count++]=fd;}for(int i=0;i<count;i++)close(fds[i]);fflush(stdout);_exit(count==128?4:0);}int status;if(waitpid(pid,&status,0)<0){perror("waitpid");return -1;}printf("Resource-limit worker exit status: %d\n",WIFEXITED(status)?WEXITSTATUS(status):-1);return WIFEXITED(status)&&WEXITSTATUS(status)==0?0:-1;}
