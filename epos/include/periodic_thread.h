// EPOS Periodic Thread Abstraction Declarations

// Periodic threads are achieved by programming an alarm handler to invoke
// p() on a control semaphore after each job (i.e. task activation). Base
// threads are created in BEGINNING state, so the scheduler won't dispatch
// them before the associate alarm and semaphore are created. The first job
// is dispatched by resume() (thus the _state = SUSPENDED statement)

#ifndef __periodic_thread_h
#define __periodic_thread_h

#include <utility/handler.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

// Aperiodic Thread
typedef Thread Aperiodic_Thread;

// Periodic Thread
class Periodic_Thread: public Thread
{
protected:
    // Alarm Handler for periodic threads under static scheduling policies
    class Static_Handler: public Semaphore_Handler
    {
    public:
        Static_Handler(Semaphore * s, Periodic_Thread * t): Semaphore_Handler(s) {}
        ~Static_Handler() {}
    };

    // Alarm Handler for periodic threads under dynamic scheduling policies
    class Dynamic_Handler: public Semaphore_Handler
    {
    public:
        Dynamic_Handler(Semaphore * s, Periodic_Thread * t): Semaphore_Handler(s), _thread(t) {}
        ~Dynamic_Handler() {}

        void operator()() {
            _thread->criterion().update();

            Semaphore_Handler::operator()();
        }

    private:
        Periodic_Thread * _thread;
    };

    typedef IF<Criterion::dynamic, Dynamic_Handler, Static_Handler>::Result Handler;

public:
    typedef RTC::Microsecond Microsecond;

    enum { INFINITE = RTC::INFINITE };

    struct Configuration: public Thread::Configuration {
        Configuration(const Microsecond & p, int n = INFINITE, const State & s = READY, const Criterion & c = NORMAL, unsigned int ss = STACK_SIZE)
        : Thread::Configuration(s, c, ss), period(p), times(n) {}

        Microsecond period;
        int times;
    };

public:
    template<typename ... Tn>
    Periodic_Thread(const Configuration & conf, int (* entry)(Tn ...), Tn ... an)
    : Thread(Thread::Configuration(SUSPENDED, (conf.criterion != NORMAL) ? conf.criterion : Criterion(conf.period), conf.stack_size), entry, an ...),
      _semaphore(0), _handler(&_semaphore, this), _alarm(conf.period, &_handler, conf.times) {
        if((conf.state == READY) || (conf.state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = conf.state;
    }

    static volatile bool wait_next() {
        Periodic_Thread * t = reinterpret_cast<Periodic_Thread *>(running());

        if(t->_alarm._times)
            t->_semaphore.p();

        return t->_alarm._times;
    }

protected:
    Semaphore _semaphore;
    Handler _handler;
    Alarm _alarm;
};

class RT_Thread: public Periodic_Thread
{
public:
    enum {
        SAME        = Scheduling_Criteria::RT_Common::SAME,
        NOW         = Scheduling_Criteria::RT_Common::NOW,
        UNKNOWN     = Scheduling_Criteria::RT_Common::UNKNOWN,
        ANY         = Scheduling_Criteria::RT_Common::ANY
    };

public:
    RT_Thread(void (* function)(), const Microsecond & deadline, const Microsecond & period = SAME, const Microsecond & capacity = UNKNOWN, const Microsecond & activation = NOW, int times = INFINITE, unsigned int stack_size = STACK_SIZE)
    : Periodic_Thread(Configuration(activation ? activation : period ? period : deadline, activation ? 1 : times, SUSPENDED, Criterion(deadline, period ? period : deadline, capacity), stack_size), &entry, this, function, activation, times) {
        if(activation && Criterion::dynamic)
            // The priority of dynamic criteria will be adjusted to the correct value by the
            // update() in the operator()() of Handler
            const_cast<Criterion &>(_link.rank())._priority = Criterion::PERIODIC;
        resume();
    }

private:
    static int entry(RT_Thread * t, void (*function)(), const Microsecond activation, int times) {
        if(activation) {
            // Wait for activation time
            t->_semaphore.p();

            // Adjust alarm's period
            t->_alarm.~Alarm();
            new (&t->_alarm) Alarm(t->criterion()._period, &t->_handler, times);
        }

        // Periodic execution loop
        do {
            // Release job
            function();
        } while (wait_next());

        return 0;
    }
};

typedef Periodic_Thread::Configuration RTConf;

__END_SYS

#endif
