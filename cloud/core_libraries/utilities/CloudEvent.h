#ifndef CLOUD_EVENT_HEADER
#define CLOUD_EVENT_HEADER

#include "CloudDelegate.h"
#include "DynamicArray.h"

namespace Cloud
{
    template <class SENDER>
    class Event
    {
    public:
        Event();
        ~Event();

        void operator+=(const BaseDelegate<SENDER>* handler);
        void operator-=(const BaseDelegate<SENDER>* handler);
        void operator()(const SENDER* sender, void* parameter)
        {
            Call(sender, parameter);
        }

        void Call(const SENDER* Sender, void* parameter);

    private:
        Utils::DynamicArray<BaseDelegate<SENDER>*> m_subscribers;

        void Register(const BaseDelegate<SENDER>* handler);
        void Unregister(const BaseDelegate<SENDER>* handler);
    };
}

#include "CloudEvent.inl"

#endif // CLOUD_EVENT_HEADER