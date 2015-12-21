#ifndef THREAD_POOL_T_H_INCLUDED
#define THREAD_POOL_T_H_INCLUDED

//*************************************************************************//

#include <deque>
#include <functional>
#include <chrono>

#include <future>
#include <thread>
#include <mutex>
#include <atomic>

//*************************************************************************//

template<typename T>
class Thread_Pool {

private:

    size_t thread_number;

    // holds worker threads
    std::deque<std::thread> workers;
    
    // used to signify the time to clean up
    std::atomic_bool done;

    // threads can work from the front or the back
    char work_end;

    // time to sleep if a task is unavailable
    unsigned int time_to_sleep;

    // mutex for submitting and doing tasks
    mutable std::mutex muter;

    // hold tasks in a deque
    std::deque<std::packaged_task<T()>> tasks;

    // joins threads on destruction
    class Thread_Joiner {

    private:

        std::deque<std::thread>& workers;

    public:

        explicit Thread_Joiner(std::deque<std::thread>& w) : workers(w) {}

        ~Thread_Joiner() {

            for(size_t i = 0; i < workers.size(); ++i) {

                if(workers[i].joinable()) { workers[i].join(); }
            }
        }

    };

    Thread_Joiner joiner;
    
    // causes a thread to repetitively perform tasks
    void looper() {
        
        while(!done) {

            if(work_end == 'f') {

                if(!work_front()) {

                    std::this_thread::sleep_for(

                        std::chrono::milliseconds(time_to_sleep)
                    );
                }

            } else {

                if(!work_back()) {

                    std::this_thread::sleep_for(

                        std::chrono::milliseconds(time_to_sleep)
                    );
                }
            }
        }
    }
    
public:
    
    // basic constructors
    Thread_Pool(size_t t_n, char w_e,
                unsigned int t)

        : thread_number(t_n)
        , done(false)
        , work_end(w_e)
        , time_to_sleep(t)
        , joiner(workers) {
        
        try {
            
            for(size_t i = 0; i < thread_number; ++i) {
                
                workers.push_back(std::thread(&Thread_Pool::looper, this));
            }
            
        } catch(...) {
            
            done = true;

            throw;
        }
    }

    Thread_Pool()

        : thread_number(2)
        , done(false)
        , work_end('f')
        , time_to_sleep(1)
        , joiner(workers) {
        
        try {
            
            for(size_t i = 0; i < thread_number; ++i) {
                
                workers.push_back(std::thread(&Thread_Pool::looper, this));
            }
            
        } catch(...) {
            
            done = true;

            throw;
        }
    }

    // deleted constructors
    Thread_Pool(const Thread_Pool&) = delete;

    Thread_Pool(Thread_Pool&&) = delete;

    // deleted assingment operators
    Thread_Pool& operator = (const Thread_Pool&) = delete;

    Thread_Pool& operator = (Thread_Pool&&) = delete;
    
    // destructor
    ~Thread_Pool() { done = true; }

    // set work end
    void set_work_end(char end) { 

        muter.lock();

        work_end = end;

        muter.unlock();
    }

    // set work end
    void set_time_to_sleep(unsigned int t) {

        muter.lock();

        time_to_sleep = t;

        muter.unlock();
    }
    
    // add task to front of deque to work
    template<typename F>
    std::future<T> load_front(F f) {
        
        // create a packaged task of the return type of the function
        std::packaged_task<T()> task(std::move(f));

        // use a future for the result
        std::future<T> result(task.get_future());

        // lock mutex while submitting
        muter.lock();

        // push task onto deque
        tasks.push_front(std::move(task));

        // unlock mutex
        muter.unlock();

        // return future with return type
        return result;
    }

    // add task to back of deque to work
    template<typename F>
    std::future<T> load_back(F f) {
        
        // create a packaged task of the return type of the function
        std::packaged_task<T()> task(std::move(f));

        // use a future for the result
        std::future<T> result(task.get_future());

        // lock mutex while submitting
        muter.lock();

        // push task onto deque
        tasks.push_back(std::move(task));

        // unlock mutex
        muter.unlock();

        // return future with return type
        return result;
    }

    bool work_front() {

        // lock task mutex while taking task to do
        muter.lock();

        // determine if task is available
        if(!tasks.empty()) {

            // move task into packaged task
            std::packaged_task<T()> task(std::move(tasks.front()));

            // pop it off the queue
            tasks.pop_front();

            // unlock mutex
            muter.unlock();

            // perform it
            task();

            return true;
        }

        // unlock mutex
        muter.unlock();

        return false;
    }

    bool work_back() {

        // lock task mutex while taking task to do
        muter.lock();

        // determine if task is available
        if(!tasks.empty()) {

            // move task into packaged task
            std::packaged_task<T()> task(std::move(tasks.back()));

            // pop it off the queue
            tasks.pop_back();

            // unlock mutex
            muter.unlock();

            // perform it
            task();

            return true;
        }

        // unlock mutex
        muter.unlock();

        return false;
    }

    // test if task queue is empty
    bool is_empty() {

        // lock task mutex while determining emptiness
        muter.lock();

        // determine if deque is empty or not
        bool empty = tasks.empty();

        // unlock mutex
        muter.unlock();

        // return status
        return empty;
    }

};

//*************************************************************************//

#endif // THREAD_POOL_T_H_INCLUDED
