/*
* startuję usługe -> usługa radzi sobie sama
* - mogę wysłać sygnał usłudzę
* - mogę odebrać sygnał jako usługa
* - mogę wysłać sygnał jako usługa
* 
* sygnały lądują w kolejce. każda usługa ma swoją kolejkę. 
* sygnały powinne być odbierane jako:
* while(signal.poll_signal()) { ... }
* sygnały wysyłane z usługi: this->send(application->get_service<service_type>(), data_type)
* powyższe działanie, powinno podpowiadać typ sygnału w IDE
* sygnały odbierane z usługi signal = this->receive<data_type>()
* signal:
*  - sender,
*  - data_type
* 
* service_derived:
*  - receive
* 
* 
*/

#pragma once 
#include <cstdint>

namespace agl
{
}