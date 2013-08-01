#ifndef CLOUD_DELEGATE_HEADER
#define CLOUD_DELEGATE_HEADER

#include "DefinesAsserts.h"

namespace Cloud
{
    template <class SENDER>
    class BaseDelegate
    {
    public:
        virtual ~BaseDelegate() {}

        void operator()(const SENDER* sender, void* parameter)
        {
            Call(sender, parameter);
        }

        virtual void Call(const SENDER* sender, void* parameter) = 0;
        virtual bool Equals(const BaseDelegate<SENDER>* other) = 0;
    };

    template <class SUBSCRIBER, class SENDER>
    class Delegate : public BaseDelegate<SENDER>
    {
        typedef void (SUBSCRIBER::*Function)(const SENDER*, void* parameter);

    public:
        Delegate(SUBSCRIBER* subscriber, Function function)
            :m_subscriber(subscriber)
            ,m_function(function)
        {
        }

        virtual ~Delegate() { }

        virtual void Call(const SENDER* sender, void* parameter)
        {
            CL_ASSERT(sender != 0, "Lacking sender!");
            (m_subscriber->*m_function)(sender, parameter);
        }

        bool Equals(const BaseDelegate<SENDER>* abstractHandler)
        {
            const Delegate<SUBSCRIBER, SENDER>* handler = static_cast<const Delegate<SUBSCRIBER, SENDER>*>(abstractHandler);

            CL_ASSERT(handler != 0, "Delegate null or of wrong sub-type!");
            CL_ASSERT(m_subscriber != 0, "Subscriber not initiated!");

            return (handler->m_subscriber == m_subscriber) && handler->m_function == m_function;
        }

    private:
        Function m_function;
        SUBSCRIBER* m_subscriber;

    };
}

#endif // CLOUD_DELEGATE_HEADER