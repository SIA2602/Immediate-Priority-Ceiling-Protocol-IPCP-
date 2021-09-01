// EPOS Semaphore Abstraction Implementation

#include <semaphore.h>

__BEGIN_SYS

int Semaphore_IPCP::Teto_sistema = 0;
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




Semaphore_IPCP::Semaphore_IPCP(int v, int ceiling) : Semaphore(v),_value(v), Recurso_ceiling(ceiling),_ptr(this,ceiling)
{
    db<Synchronizer>(TRC) << "Semaphore_IPCP(ceiling=" << Recurso_ceiling << ") => " << this << endl;
    
    if(Recurso_ceiling > Teto_sistema )
        Teto_sistema= Recurso_ceiling;
}



void Semaphore_IPCP::p()
{
    db<Synchronizer>(TRC) << "Semaphore_IPCP::p(this=" << this << ",ceiling=" << Recurso_ceiling << ")" << endl;

    begin_atomic();
    Recurso_ceiling = (_ptr.object())->get_Teto_sistema(); //acessa prioridade do recurso
    if( fdec(_value) < 1){

        if(Teto_sistema > Recurso_ceiling ){ //verifica mais alta prioridade efetiva 

            _locked->insert(&_ptr);    //insere semaforo na fila de semaforos bloqueados
            sleep();        
        }

        if(Teto_sistema < Recurso_ceiling ){ //verifica mais alta prioridade efetiva
            Teto_sistema = (_locked->head()->object())->Teto_sistema;  //atualiza teto do sistema caso prioridade do recurso seja maior q a do sistema
        }
       
    }

    else
    {
        Teto_sistema = Recurso_ceiling;
        end_atomic();
    }

}


void Semaphore_IPCP::v()
{
    db<Synchronizer>(TRC) << "Semaphore_srp::v(this=" << this << ",ceiling=" << Recurso_ceiling << ")" << endl;

    begin_atomic();
    
    if(finc(_value) < 0){ 

        _locked->remove(&_ptr);    //remove semaforo da lista de semaforos bloqueados

        if(!_locked->empty()) //verifica se há taferas bloqueadas
        {
            Teto_sistema  = (_locked->head()->object())->Teto_sistema;       
        }

        else
        {
            Teto_sistema   = 0;
        }
        
        wakeup();  // implicit end_atomic()
    } 

    else
        end_atomic();
}


__END_SYS
