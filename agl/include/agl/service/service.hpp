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
	class signal_base
	{
	public:
		signal_base(std::uint64_t constexpr data_type) = default;
		signal_base(signal_base&&) = default;
		signal_base(signal_base const&) = delete;
		signal_base& operator=(signal_base&&) = default;
		signal_base& operator=(signal_base const&) = delete;
		virtual ~signal_base() = default;

		std::uint64_t constexpr get_data_type() const;
	private:
		std::uint64_t constexpr m_data_type;
	};

	template <typename T>
	class signal
	{
	public:
		signal();
		signal(signal&&) = default;
		signal(signal const&) = delete;
		signal& operator=(signal&&) = default;
		signal& operator=(signal const&) = delete;

	private:

	};


	class service
	{
	public:
		service() = default;
		service(service&&) = default;
		service(service const&) = delete;
		service& operator=(service&&) = default;
		service& operator=(service const&) = delete;
		virtual ~service() = default;

		template <typename T>
		void send_signal(T&& data);

	protected:

	private:
	};
}