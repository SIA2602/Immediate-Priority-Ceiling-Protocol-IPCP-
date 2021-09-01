// EPOS Semaphore Abstraction Implementation

#include <semaphore.h>

__BEGIN_SYS

int Semaphore_IPCP::system_ceiling = 0;
Semaphore_IPCP::Queue_IPCP* Semaphore_IPCP::_locked;

Semaphore::Semaphore(int v): _value(v)
{
    db<Synchronizer>(TRC) << "Semaphore(value=" << _value << ") => " << this << endl;
}


Semaphore::~Semaphore()
{
    db<Synchronizer>(TRC) << "~Semaphore(this=" << this << ")" << endl;
}


void Semaphore::p()
{
    db<Synchronizer>(TRC) << "Semaphore::p(this=" << this << ",value=" << _value << ")" << endl;

    begin_atomic();
    if(fdec(_value) < 1)
        sleep(); // implicit end_atomic()
    else
        end_atomic();
}


void Semaphore::v()
{
    db<Synchronizer>(TRC) << "Semaphore::v(this=" << this << ",value=" << _value << ")" << endl;

    begin_atomic();
    if(finc(_value) < 0)
        wakeup();  // implicit end_atomic()
    else
        end_atomic();
}

/*Semaphore_IPCP::Semaphore_IPCP(int v): _value(v)
{
    db<Synchronizer>(TRC) << "Semaphore(value=" << _value << ") => " << this << endl;
}*/

Semaphore_IPCP::Semaphore_IPCP(int v, int ceiling) : Semaphore(v),_value(v), resource_ceiling(ceiling),_link(this,ceiling)
{
    db<Synchronizer>(TRC) << "Semaphore_IPCP(ceiling=" << resource_ceiling << ") => " << this << endl;
    if(resource_ceiling > system_ceiling)
    {
        system_ceiling = resource_ceiling;
    }
}


/*Semaphore_IPCP::~Semaphore_IPCP()
{
    db<Synchronizer>(TRC) << "~Semaphore(this=" << this << ")" << endl;
}*/


/*void Semaphore_IPCP::p()
{
    db<Synchronizer>(TRC) << "Semaphore::p(this=" << this << ",value=" << _value << ")" << endl;

    begin_atomic();
    if(fdec(_value) < 1)
        sleep(); // implicit end_atomic()
    else
        end_atomic();
}*/

void Semaphore_IPCP::p()
{
    db<Synchronizer>(TRC) << "Semaphore_IPCP::p(this=" << this << ",ceiling=" << resource_ceiling << ")" << endl;

    begin_atomic();
    //_locked->insert(&_link);
    resource_ceiling = (_link.object())->get_SystemCeiling(); //atualiza o teto do recurso
    if(fdec(_value) < 1){
        _locked->insert(&_link);    //insere semaforo atual na fila de semaforos bloqueados
        system_ceiling = (_locked->head()->object())->system_ceiling; //atualiza o teto do sistema
        sleep();        //implicit end_atomic()
    }else{
        system_ceiling = 0;
        end_atomic();
    }
}

/*void Semaphore_IPCP::v()
{
    db<Synchronizer>(TRC) << "Semaphore::v(this=" << this << ",value=" << _value << ")" << endl;

    begin_atomic();
    if(finc(_value) < 0)
        wakeup();  // implicit end_atomic()
    else
        end_atomic();
}*/

void Semaphore_IPCP::v()
{
    db<Synchronizer>(TRC) << "Semaphore_IPCP::v(this=" << this << ",ceiling=" << resource_ceiling << ")" << endl;

    begin_atomic();
    
    if(finc(_value) < 0){ 
        _locked->remove(&_link);    //remove semaforo atual da fila de semaforos bloqueados
        if(!_locked->empty()){
            system_ceiling = (_locked->head()->object())->system_ceiling;       //atualiza o teto do sistema
        }else{
            system_ceiling = 0;
        }
        
        wakeup();  // implicit end_atomic()
    } else
        system_ceiling = 0;
        end_atomic();
}


__END_SYS
