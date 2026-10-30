
#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "job.h"

#define MAX_TIMELINE_SEGMENTS 10000

typedef struct
{
    int job_id;
    int start_time;
    int end_time;
} TimelineSegment;

typedef struct
{
    TimelineSegment segments[MAX_TIMELINE_SEGMENTS];

    int segment_count;

    double average_waiting_time;
    double average_turnaround_time;
} ScheduleResult;

int schedule_fcfs(JobManager *m, ScheduleResult *r);

int schedule_round_robin(JobManager *m, ScheduleResult *r, int quantum);

int schedule_priority(JobManager *m, ScheduleResult *r);

void display_schedule(const JobManager *m, const ScheduleResult *r, const char *name);

#endif
