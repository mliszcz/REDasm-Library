#include "jobmanager.h"
#include <impl/support/job/jobmanager_impl.h>
#include <algorithm>
#include <thread>
#include <cmath>

namespace REDasm {

size_t JobManager::concurrency() { return JobManagerImpl::m_numthreads.load(); }
bool JobManager::initialized() { return JobManagerImpl::m_initialized; }
bool JobManager::last() { return (JobManagerImpl::m_currentlabel - JobManagerImpl::m_finishedlabel.load()) == 1; }
bool JobManager::busy() { return JobManagerImpl::m_finishedlabel.load() < JobManagerImpl::m_currentlabel; }
void JobManager::wait() { while(JobManager::busy()) JobManagerImpl::poll(); }
void JobManager::dispatch(const JobManager::JobDispatchCallback& cb) { JobManager::dispatch(JobManagerImpl::m_numthreads.load(), cb); }
void JobManager::dispatch(size_t jobcount, const JobManager::JobDispatchCallback& cb) { JobManager::dispatch(jobcount, 1, cb); }

/*
 * Divide a job onto multiple jobs and execute in parallel.
 * - jobcount  : How many jobs to generate for this task.
 * - groupsize : How many jobs to execute per thread. Jobs inside a group execute serially.
 */
void JobManager::dispatch(size_t jobcount, size_t groupsize, const JobManager::JobDispatchCallback& cb)
{
    if(!jobcount || !groupsize) return;

    size_t groupcount = static_cast<size_t>(std::ceil((jobcount + groupsize - 1) / groupsize));
    JobManagerImpl::m_currentlabel += groupcount;

    for(size_t groupindex = 0; groupindex < groupcount; groupindex++)
    {
        auto jobgroup = [jobcount, groupsize, cb, groupindex]() {
            size_t groupjoboffset = groupindex * groupsize;
            size_t groupjobend = std::min(groupjoboffset + groupsize, jobcount);

            JobDispatchArgs args;
            args.groupIndex = groupindex;

            for(size_t i = groupjoboffset; i < groupjobend; ++i) {
                args.jobIndex = i;
                cb(args);
            }
        };

        while(!JobManagerImpl::m_jobpool.push_back(jobgroup))
            JobManagerImpl::poll();

        JobManagerImpl::m_wakecondition.notify_one(); // wake one thread
    }
}

void JobManager::execute(const JobManager::JobCallback& cb)
{
    JobManagerImpl::m_currentlabel += 1;

    // Try to push a new job until it is pushed successfully:
    while(!JobManagerImpl::m_jobpool.push_back(cb))
        JobManagerImpl::poll();

    JobManagerImpl::m_wakecondition.notify_one();
}

void JobManager::deinitialize()
{
    JobManagerImpl::m_initialized.store(false); // Notify threads for deinitialization
    JobManager::wait();                         // Wait for running jobs
    JobManagerImpl::waitTerminate();            // Wait for thread termination
}

void JobManager::initialize()
{
    JobManagerImpl::m_numthreads.store(std::max(1u, std::thread::hardware_concurrency()));
    JobManagerImpl::m_jobpool.reset();
    JobManagerImpl::m_currentlabel = 0;
    JobManagerImpl::m_finishedlabel.store(0);
    JobManagerImpl::m_initialized.store(true);

    for(size_t tid = 0; tid < JobManagerImpl::m_numthreads; tid++)
    {
        std::thread worker([] {
            JobCallback job;

            while(JobManagerImpl::m_initialized) {
                if(JobManagerImpl::m_jobpool.pop_front(job)) {
                    job();
                    JobManagerImpl::m_finishedlabel.fetch_add(1);
                    continue;
                }

                JobManagerImpl::job_lock lock(JobManagerImpl::m_wakemutex);
                JobManagerImpl::m_wakecondition.wait(lock);
            }

            JobManagerImpl::m_numthreads.fetch_sub(1);
        });

        worker.detach();
    }
}

} // namespace REDasm
