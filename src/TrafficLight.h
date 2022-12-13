#ifndef TRAFFICLIGHT_H
#define TRAFFICLIGHT_H

#include <mutex>
#include <deque>
#include <condition_variable>
#include "TrafficObject.h"

// forward declarations to avoid include cycle
class Vehicle;

/// @brief Possible traffic lights
enum TrafficLightPhase
{
    red,
    green
};

/// @brief Message queue that receives and sends messages asynchronously.
/// @tparam T generic message template
template <class T>
class MessageQueue
{
public:
    void send(T &&message);
    T receive();

private:
    std::deque<T> _queue;
    std::condition_variable _condition;
    std::mutex _mutex;
};

class TrafficLight : public TrafficObject
{
public:
    // constructor / destructor
    TrafficLight();
    ~TrafficLight();

    // getters / setters
    TrafficLightPhase getCurrentPhase();

    // typical behaviour methods
    void waitForGreen();
    void simulate();

private:
    // typical behaviour methods
    void cycleThroughPhases();

    // data variables
    TrafficLightPhase _currentPhase;
    std::shared_ptr<MessageQueue<TrafficLightPhase>> _messages;
    std::condition_variable _condition;
    std::mutex _mutex;
};

#endif