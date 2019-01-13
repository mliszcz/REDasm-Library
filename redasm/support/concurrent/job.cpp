#include "job.h"
#include "../../redasm_runtime.h"

#define JOB_BASE_INTERVAL 1 // 1ms

namespace REDasm {

Job::Job(): m_state(Job::InactiveState), m_interval(JOB_BASE_INTERVAL) { }

Job::~Job()
{
    m_state = Job::InactiveState;

    if(!m_thread.joinable())
        return;

    m_cv.notify_one();
    m_thread.join();
}

size_t Job::state() const { return m_state; }
bool Job::active() const { return m_state > Job::SleepState; }

void Job::start()
{
    if(m_state == Job::ActiveState)
        return;

    m_state = Job::ActiveState;

    if(REDasm::Runtime::sync())
        this->doWorkSync();
    else
        m_cv.notify_one();
}

void Job::stop()
{
    if(m_state <= Job::SleepState)
        return;

    m_state = Job::SleepState;
    stateChanged(this);
}

void Job::pause()
{
    if(!this->active() || (m_state != Job::ActiveState))
        return;

    m_state = Job::PausedState;
    stateChanged(this);
}

void Job::resume()
{
    if(!this->active() || (m_state != Job::PausedState))
        return;

    m_state = Job::ActiveState;
    stateChanged(this);
}

void Job::work(JobCallback cb, bool deferred)
{
    if(this->active())
        return;

    m_interval = std::chrono::milliseconds(JOB_BASE_INTERVAL);
    m_state = deferred ? Job::SleepState : Job::ActiveState;
    m_jobcallback = cb;
    stateChanged(this);

    if(REDasm::Runtime::sync())
    {
        this->doWorkSync();
        return;
    }

    if(m_thread.joinable())
        m_cv.notify_one();
    else
        m_thread = std::thread(&Job::doWork, this);
}

void Job::doWork()
{
    std::unique_lock<std::mutex> lock(m_mutex);

    for( ; ; )
    {
        while(m_state != Job::ActiveState)
        {
            m_cv.wait(lock);

            if(m_state == Job::InactiveState)
                return;
        }

        if(m_state == Job::ActiveState)
        {
            auto start = std::chrono::steady_clock::now();
            m_jobcallback(this);
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - start);
            m_interval = ((m_interval + elapsed) / 2);
        }

        std::this_thread::sleep_for(m_interval);
    }
}

void Job::doWorkSync()
{
    while(this->active())
    {
        if(m_state == Job::ActiveState)
            m_jobcallback(this);
    }
}

} // namespace REDasm
