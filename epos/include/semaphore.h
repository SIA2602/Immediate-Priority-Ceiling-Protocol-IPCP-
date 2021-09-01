// EPOS Semaphore Abstraction Declarations

#ifndef __semaphore_h
#define __semaphore_h

#include <utility/handler.h>
#include <synchronizer.h>

__BEGIN_SYS

class Semaphore: protected Synchronizer_Common
{
public:
    Semaphore(int v = 1);
    ~Semaphore();

    void p();
    void v();

private:
    volatile int _value;
};


class Semaphore_IPCP : public Semaphore
{
public:
    
    typedef Ordered_Queue<Semaphore_IPCP> Queue_IPCP;
    
    Semaphore_IPCP(int v = 1, int ceiling=0);
    
    void p();
    void v();
    
    static int get_Teto_sistema(){ return Teto_sistema;}
    
    static Queue_IPCP * _locked;
    
private:
    int _value;
    int Recurso_ceiling;

    static int Teto_sistema;
    
    Queue_IPCP::Element _ptr;
};


// An event handler that triggers a semaphore (see handler.h)
class Semaphore_Handler: public Handler
{
public:
    Semaphore_Handler(Semaphore * h) : _handler(h) {}
    ~Semaphore_Handler() {}

    void operator()() { _handler->v(); }

private:
    Semaphore * _handler;
};

__END_SYS

#endif
