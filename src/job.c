#include <stdio.h>
#include "job.h"
void initialize_job_manager(JobManager *m){if(m)m->count=0;}
const char *job_status_string(JobStatus s){switch(s){case JOB_NEW:return "NEW";case JOB_READY:return "READY";case JOB_RUNNING:return "RUNNING";case JOB_COMPLETED:return "COMPLETED";default:return "UNKNOWN";}}
int add_job(JobManager *m,int id,int arrival,int burst,int priority){
 if(!m||m->count>=MAX_JOBS||id<=0||arrival<0||burst<=0||priority<0){fprintf(stderr,"Invalid job or job limit reached.\n");return -1;}
 for(int i=0;i<m->count;i++)if(m->jobs[i].id==id){fprintf(stderr,"Duplicate job ID %d.\n",id);return -1;}
 m->jobs[m->count++]=(Job){.id=id,.arrival_time=arrival,.burst_time=burst,.remaining_time=burst,.priority=priority,.status=JOB_NEW};
 printf("Added job J%d\n",id);return 0;
}
void display_jobs(const JobManager *m){if(!m)return;printf("ID Arrival Burst Priority Status\n");for(int i=0;i<m->count;i++){const Job *j=&m->jobs[i];printf("%d %d %d %d %s\n",j->id,j->arrival_time,j->burst_time,j->priority,job_status_string(j->status));}}
void reset_jobs(JobManager *m){if(!m)return;for(int i=0;i<m->count;i++){Job *j=&m->jobs[i];j->remaining_time=j->burst_time;j->completion_time=j->turnaround_time=j->waiting_time=0;j->status=JOB_NEW;}}
