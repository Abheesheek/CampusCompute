#include <stdio.h>
#include <limits.h>
#include "scheduler.h"
static int segment(ScheduleResult *r,int id,int a,int b){if(a>=b)return 0;if(r->segment_count>=MAX_TIMELINE_SEGMENTS){fprintf(stderr,"Timeline limit exceeded\n");return -1;}r->segments[r->segment_count++]=(TimelineSegment){id,a,b};return 0;}
static int init(JobManager *m,ScheduleResult *r){if(!m||!r||m->count<1||m->count>MAX_JOBS)return -1;reset_jobs(m);r->segment_count=0;r->average_waiting_time=r->average_turnaround_time=0;return 0;}
static void finish(Job *j,int time,long long *w,long long *t){j->remaining_time=0;j->completion_time=time;j->turnaround_time=time-j->arrival_time;j->waiting_time=j->turnaround_time-j->burst_time;j->status=JOB_COMPLETED;*w+=j->waiting_time;*t+=j->turnaround_time;}
static void averages(ScheduleResult *r,int n,long long w,long long t){r->average_waiting_time=(double)w/n;r->average_turnaround_time=(double)t/n;}
int schedule_fcfs(JobManager *m,ScheduleResult *r){if(init(m,r))return -1;int time=0;long long w=0,t=0;
 for(int done=0;done<m->count;done++){int k=-1;for(int i=0;i<m->count;i++){Job *j=&m->jobs[i];if(j->status==JOB_COMPLETED)continue;if(k<0||j->arrival_time<m->jobs[k].arrival_time||(j->arrival_time==m->jobs[k].arrival_time&&j->id<m->jobs[k].id))k=i;}
 Job *j=&m->jobs[k];if(time<j->arrival_time){if(segment(r,-1,time,j->arrival_time))return -1;time=j->arrival_time;}if(j->burst_time>INT_MAX-time)return -1;j->status=JOB_RUNNING;int start=time;time+=j->burst_time;if(segment(r,j->id,start,time))return -1;finish(j,time,&w,&t);}
 averages(r,m->count,w,t);return 0;}
int schedule_priority(JobManager *m,ScheduleResult *r){if(init(m,r))return -1;int time=0;long long w=0,t=0;
 for(int done=0;done<m->count;){int k=-1,next=INT_MAX;for(int i=0;i<m->count;i++){Job *j=&m->jobs[i];if(j->status==JOB_COMPLETED)continue;if(j->arrival_time>time){if(j->arrival_time<next)next=j->arrival_time;continue;}if(k<0||j->priority<m->jobs[k].priority||(j->priority==m->jobs[k].priority&&(j->arrival_time<m->jobs[k].arrival_time||(j->arrival_time==m->jobs[k].arrival_time&&j->id<m->jobs[k].id))))k=i;}
 if(k<0){if(next==INT_MAX||segment(r,-1,time,next))return -1;time=next;continue;}Job *j=&m->jobs[k];if(j->burst_time>INT_MAX-time)return -1;j->status=JOB_RUNNING;int start=time;time+=j->burst_time;if(segment(r,j->id,start,time))return -1;finish(j,time,&w,&t);done++;}
 averages(r,m->count,w,t);return 0;}
int schedule_round_robin(JobManager *m,ScheduleResult *r,int quantum){if(init(m,r)||quantum<=0)return -1;int n=m->count,order[MAX_JOBS],queue[MAX_JOBS+1],head=0,tail=0,count=0,next=0,done=0,time=0;long long w=0,t=0;
 for(int i=0;i<n;i++){order[i]=i;}
 for(int i=1;i<n;i++){int key=order[i],j=i-1;while(j>=0&&(m->jobs[order[j]].arrival_time>m->jobs[key].arrival_time||(m->jobs[order[j]].arrival_time==m->jobs[key].arrival_time&&m->jobs[order[j]].id>m->jobs[key].id))){order[j+1]=order[j];j--;}order[j+1]=key;}
 while(done<n){while(next<n&&m->jobs[order[next]].arrival_time<=time){int k=order[next++];m->jobs[k].status=JOB_READY;queue[tail]=k;tail=(tail+1)%(MAX_JOBS+1);count++;}
 if(!count){if(next>=n)return -1;int nt=m->jobs[order[next]].arrival_time;if(segment(r,-1,time,nt))return -1;time=nt;continue;}
 int k=queue[head];head=(head+1)%(MAX_JOBS+1);count--;Job *j=&m->jobs[k];j->status=JOB_RUNNING;int slice=j->remaining_time<quantum?j->remaining_time:quantum;if(slice>INT_MAX-time)return -1;int start=time;time+=slice;j->remaining_time-=slice;if(segment(r,j->id,start,time))return -1;
 while(next<n&&m->jobs[order[next]].arrival_time<=time){int a=order[next++];m->jobs[a].status=JOB_READY;queue[tail]=a;tail=(tail+1)%(MAX_JOBS+1);count++;}
 if(j->remaining_time==0){finish(j,time,&w,&t);done++;}else{j->status=JOB_READY;queue[tail]=k;tail=(tail+1)%(MAX_JOBS+1);count++;}}
 averages(r,n,w,t);return 0;}
void display_schedule(const JobManager *m,const ScheduleResult *r,const char *name){if(!m||!r)return;printf("\n%s scheduling (timeline):\n",name);for(int i=0;i<r->segment_count;i++){TimelineSegment s=r->segments[i];if(s.job_id<0)printf("[%d-%d IDLE] ",s.start_time,s.end_time);else printf("[%d-%d J%d] ",s.start_time,s.end_time,s.job_id);}printf("\nID Arrival Burst Priority Completion Turnaround Waiting\n");for(int i=0;i<m->count;i++){Job j=m->jobs[i];printf("%d %d %d %d %d %d %d\n",j.id,j.arrival_time,j.burst_time,j.priority,j.completion_time,j.turnaround_time,j.waiting_time);}printf("Average waiting: %.2f | Average turnaround: %.2f\n",r->average_waiting_time,r->average_turnaround_time);}
