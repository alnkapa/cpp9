#ifndef PUB_SUB_H
#define PUB_SUB_H
#include <memory>
#include <vector>

template <typename T>
class Subscriber
{
public:
    using value_type = T;
    virtual void callback(const value_type &message) = 0;
    virtual ~Subscriber() = default;
};

template <typename T>
class Publisher
{
    using subscriber_type = Subscriber<T>;
    using value_type = Subscriber<T>::value_type;

public:
    virtual ~Publisher() = default;
    void subscribe(const std::weak_ptr<subscriber_type> in)
    {
        subscribers_array.push_back(in);
    }
    void unsubscribe(const std::weak_ptr<subscriber_type> &in)
    {
        subscribers_array.erase(
            std::remove_if(
                subscribers_array.begin(),
                subscribers_array.end(),
                [&in](const std::weak_ptr<subscriber_type> &v)
                {
                    return v.expired() || in.lock() == v.lock();
                }),
            subscribers_array.end());
    }
    void notify(const value_type &in)
    {
        for (
            auto it = subscribers_array.begin();
            it != subscribers_array.end();)
        {
            if (auto l = it->lock())
            {
                l->callback(in);
                ++it;
            }
            else
            {
                it = subscribers_array.erase(it);
            }
        }
    }

private:
    std::vector<std::weak_ptr<subscriber_type>> subscribers_array;
};

#endif