// EPOS Periodic Thread Abstraction Test Program

#include <utility/ostream.h>
#include <periodic_thread.h>
#include <chronometer.h>

using namespace EPOS;

Semaphore_IPCP *sem; //criando variavel Semaphore_IPCP

const unsigned int iterations = 1;
const unsigned int period_a = 100; // ms
const unsigned int period_b = 80; // ms
const unsigned int period_c = 60; // ms
const unsigned int period_d = 150; // ms
const unsigned int wcet_a = 50; // ms
const unsigned int wcet_b = 20; // ms
const unsigned int wcet_c = 10; // ms
const unsigned int wcet_d = 60; // ms

int func_a();
int func_b();
int func_c();
int func_d();
//((a >= b) && (a >= c)) ? a : ((b >= a) && (b >= c) ? b : c)
//((a >= b) && (a >= c) && (a >= d)) ? a : ((b >= a) && (b >= c) && (b >= d) ? b : ((c >= a) && (c >= b) && (c >= d)) ? c : d))
long max(unsigned int a, unsigned int b, unsigned int c, unsigned int d) { return (((a >= b) && (a >= c) && (a >= d)) ? a : ((b >= a) && (b >= c) && (b >= d) ? b : ((c >= a) && (c >= b) && (c >= d)) ? c : d)); }

OStream cout;
Chronometer chrono;
Periodic_Thread * thread_a;
Periodic_Thread * thread_b;
Periodic_Thread * thread_c;
Periodic_Thread * thread_d;

inline void exec(char c, unsigned int time = 0) // in miliseconds
{
    // Delay was not used here to prevent scheduling interference due to blocking
    Chronometer::Microsecond elapsed = chrono.read() / 1000;

    cout << "\n" << elapsed << "\t" << c
         << "\t[p(A)=" << thread_a->priority()
         << ", p(B)=" << thread_b->priority() 
         << ", p(C)=" << thread_c->priority()
         << ", p(D)=" << thread_d->priority() << "]";

    if(time) {
        for(Chronometer::Microsecond end = elapsed + time, last = end; end > elapsed; elapsed = chrono.read() / 1000)
            if(last != elapsed) {
                cout << "\n" << elapsed << "\t" << c
                    << "\t[p(A)=" << thread_a->priority()
                    << ", p(B)=" << thread_b->priority()
                    << ", p(C)=" << thread_c->priority()
                    << ", p(D)=" << thread_d->priority() << "]";
                last = elapsed;
            }
    }
}


int main()
{
    cout << "Periodic Thread Abstraction Test" << endl;

    cout << "\nThis test consists in creating three periodic threads as follows:" << endl;
    cout << "- Every " << period_a << "ms, thread A execs \"a\", waits for " << wcet_a << "ms and then execs another \"a\";" << endl;
    cout << "- Every " << period_b << "ms, thread B execs \"b\", waits for " << wcet_b << "ms and then execs another \"b\";" << endl;
    cout << "- Every " << period_c << "ms, thread C execs \"c\", waits for " << wcet_c << "ms and then execs another \"c\";" << endl;
    cout << "- Every " << period_d << "ms, thread D execs \"d\", waits for " << wcet_d << "ms and then execs another \"d\";" << endl;

    cout << "Threads will now be created and I'll wait for them to finish..." << endl;

    thread_a = new Periodic_Thread(RTConf(period_a * 1000, iterations), &func_a);
    thread_b = new Periodic_Thread(RTConf(period_b * 1000, iterations), &func_b);
    thread_c = new Periodic_Thread(RTConf(period_c * 1000, iterations), &func_c);
    thread_d = new Periodic_Thread(RTConf(period_d * 1000, iterations), &func_d);

    sem = new(Semaphore_IPCP); //Alocando variavel sem como Semaphore_IPCP

    exec('M');

    chrono.start();

    int status_a = thread_a->join();
    int status_b = thread_b->join();
    int status_c = thread_c->join();
    int status_d = thread_d->join();

    chrono.stop();

    exec('M');

    cout << "\n... done!" << endl;
    cout << "\n\nThread A exited with status \"" << char(status_a)
         << "\", thread B exited with status \"" << char(status_b)
         << "\", thread C exited with status \"" << char(status_c)
         << "\" and thread D exited with status \"" << char(status_d) << "." << endl;

    cout << "\nThe estimated time to run the test was "
         << max(period_a, period_b, period_c, period_d) * iterations
         << " ms. The measured time was " << chrono.read() / 1000 <<" ms!" << endl;

    cout << "I'm also done, bye!" << endl;

    return 0;
}

int func_a()
{
    exec('A');

    do {        
        sem->p();
        exec('a', wcet_a);
        sem->v();
    } while (Periodic_Thread::wait_next());

    exec('A');

    return 'A';
}

int func_b()
{
    exec('B');

    do {        
        sem->p();
        exec('b', wcet_b);
        sem->v();
    } while (Periodic_Thread::wait_next());

    exec('B');

    return 'B';
}

int func_c()
{
    exec('C');

    do {        
        sem->p();
        exec('c', wcet_c);
        sem->v();
    } while (Periodic_Thread::wait_next());

    exec('C');

    return 'C';
}

int func_d()
{
    exec('D');

    do {        
        sem->p();
        exec('d', wcet_d);
        sem->v();
    } while (Periodic_Thread::wait_next());

    exec('D');

    return 'D';
}
