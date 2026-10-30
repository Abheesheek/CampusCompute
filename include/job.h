#ifndef JOB_H
#define JOB_H
#define MAX_JOBS 100
typedef enum { JOB_NEW, JOB_READY, JOB_RUNNING, JOB_COMPLETED } JobStatus;
typedef struct {int id,arrival_time,burst_time,remaining_time,priority,completion_time,turnaround_time,waiting_time; JobStatus status;} Job;
typedef struct {Job jobs[MAX_JOBS];int count;} JobManager;
void initialize_job_manager(JobManager *m);
int add_job(JobManager *m,int id,int arrival,int burst,int priority);
void display_jobs(const JobManager *m);
void reset_jobs(JobManager *m);
const char *job_status_string(JobStatus s);
#endif
