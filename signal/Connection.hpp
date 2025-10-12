#pragma once

#include <functional>
#include <mutex>
#include <shared_mutex>
#include <memory>

template<typename... SignalArgs>
class Signal;

template<typename... SignalArgs>
class Connection
{
    friend class Signal<SignalArgs...>;

public:
    Connection() = default;
    ~Connection() = default;

    void disconnect()
    {
        if(auto shared_con_data = m_connectionData.lock())
        {
            std::unique_lock<std::shared_mutex> lock(shared_con_data->signal_mutex);
            if(shared_con_data->connected)
            {
                shared_con_data->connected = false;
                if(shared_con_data->disconnect_callback)
                {
                    shared_con_data->disconnect_callback(shared_con_data->id);
                }
            }
        }
    }

    bool isConnected() const
    {
        auto shared_con_data = m_connectionData.lock();
        return shared_con_data && shared_con_data->connected;
    }

private:
    struct ConnectionData
    {
        bool connected = true;
        size_t id = 0;
        mutable std::shared_mutex signal_mutex;
        std::function<void(size_t)> disconnect_callback;
    };

private:
    std::weak_ptr<typename Connection::ConnectionData> m_connectionData;
};
