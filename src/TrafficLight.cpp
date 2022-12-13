#include <iostream>
#include <random>
#include <chrono>
#include <future>
#include "TrafficLight.h"

/// @brief Receive new message.
/// @tparam T message
/// @return new message
template <typename T>
T MessageQueue<T>::receive()
{
    // lock access
    std::unique_lock<std::mutex> lock(_mutex);

    // wait for new messages
    _condition.wait(lock, [this]
                    { return !_queue.empty(); });

    // pull message from the queue
    auto youve_got_mail = std::move(_queue.back());

    // clear queue
    _queue.clear();

    // return new messages, ideally play AOL's "you've got mail!" out loud.
    return youve_got_mail;
}

/// @brief Send new message.
/// @tparam T message
/// @param message message to be sent
template <typename T>
void MessageQueue<T>::send(T &&message)
{
    // create a lock guard to the resource
    std::lock_guard<std::mutex> lock(_mutex);

    // push message into the queue
    _queue.push_back(std::move(message));

    // notify new message to the condition variable
    _condition.notify_one();
}

/// @brief Traffic Light constructor.
TrafficLight::TrafficLight()
{
    // initialize traffic light to red
    _currentPhase = TrafficLightPhase::red;

    // initialize message queue
    _messages = std::make_shared<MessageQueue<TrafficLightPhase>>();
}

/// @brief Traffic light destructor.
TrafficLight::~TrafficLight() {}

void TrafficLight::waitForGreen()
{
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto traffic_light_phase = _messages->receive();
        if (traffic_light_phase == green)
            return;
    }
}

/// @brief Get current phase of the traffic light.
/// @return current phase
TrafficLightPhase TrafficLight::getCurrentPhase()
{
    return _currentPhase;
}

/// @brief Initialize a traffic light simulation in a thread.
void TrafficLight::simulate()
{
    threads.emplace_back(std::thread(&TrafficLight::cycleThroughPhases, this));
}

// virtual function which is executed in a thread
void TrafficLight::cycleThroughPhases()
{
    // generate a random double distribution in the interval from 4 to 6
    std::default_random_engine generator(std::random_device{}());
    std::uniform_int_distribution<int> distribution(4000, 6000);

    // infinite loop for phase cycling
    while (true)
    {
        // sleep a sampled semaphore tick, save tons of cpu usage
        int cycle_duration = distribution(generator);
        std::this_thread::sleep_for(std::chrono::milliseconds(cycle_duration));

        // update current phase
        _currentPhase = (_currentPhase == red) ? green : red;

        // send off an update message
        auto message = _currentPhase;
        auto f_send = std::async(std::launch::async,
                                 &MessageQueue<TrafficLightPhase>::send,
                                 _messages, std::move(message));
        f_send.wait();
    }
}
