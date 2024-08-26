#ifndef PUB_SUB_H
#define PUB_SUB_H
#include <memory>
#include <vector>
#include <algorithm>

namespace pubsub
{

    template <typename T>
    class Subscriber
    {
    public:
        using value_type = T;
        virtual void callback(value_type message) = 0;
        virtual ~Subscriber() = default;
    };

    template <typename T>
    class Publisher
    {
        using subscriber_type = Subscriber<T>;
        using value_type = Subscriber<T>::value_type;
        using subscriber_ptr_type = std::weak_ptr<subscriber_type>;

    public:
        virtual ~Publisher() = default;
        void subscribe(subscriber_ptr_type in) { subscribers_array.push_back(in); }
        void unsubscribe(subscriber_ptr_type in)
        {
            auto in_locked = in.lock();
            subscribers_array.erase(
                std::remove_if(
                    subscribers_array.begin(),
                    subscribers_array.end(),
                    [&in_locked](subscriber_ptr_type v)
                    {
                        return v.expired() || in_locked == v.lock();
                    }),
                subscribers_array.end());
        }
        void notify(value_type in)
        {
            for (auto it = subscribers_array.begin(); it != subscribers_array.end();)
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
        std::vector<subscriber_ptr_type> subscribers_array;
    };
}
#endif
