#ifndef CLOUD_EVENT_INLINE
#define CLOUD_EVENT_INLINE

template <class SENDER>
Cloud::Event<SENDER>::Event()
:m_subscribers(1, 1)
{
}

template <class SENDER>
Cloud::Event<SENDER>::~Event()
{
    for (int i = 0; i < m_subscribers.Count(); ++i)
    {
        if (m_subscribers[i] != 0)
        {
            delete m_subscribers[i];
        }
    }
}

template <class SENDER>
void Cloud::Event<SENDER>::operator+=(const BaseDelegate<SENDER>* handler)
{
    Register(handler);
}

template <class SENDER>
void Cloud::Event<SENDER>::operator-=(const BaseDelegate<SENDER>* handler)
{
    Unregister(handler);
}

template <class SENDER>
void Cloud::Event<SENDER>::Call(const SENDER* sender, void* parameter)
{
    for (int i = 0; i < m_subscribers.Count(); ++i)
    {
        m_subscribers[i]->Call(sender, parameter);
    }
}

template <class SENDER>
void Cloud::Event<SENDER>::Register(const BaseDelegate<SENDER>* handler)
{
    CL_ASSERT(handler != 0, "Can't register handler in event.");

    for (int i = 0; i < m_subscribers.Count(); ++i)
    {
        if (m_subscribers[i]->Equals(handler))
        {
            return;
        }
    }

    m_subscribers.Add(const_cast<BaseDelegate<SENDER>*>(handler));
}

template <class SENDER>
void Cloud::Event<SENDER>::Unregister(const BaseDelegate<SENDER>* handler)
{
    CL_ASSERT(handler != 0, "Can't unregister handler in event.");

    for (int i = 0; i < m_subscribers.Count(); ++i)
    {
        if (m_subscribers[i]->Equals(handler))
        {
            m_subscribers.RemoveAtIndex(i);
            return;
        }
    }
}

#endif