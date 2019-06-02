#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_
#include <atomic>
#include <thread>
#include <>


class Transceiver{
public:
    Transceiver();
    Transceiver(const Transceiver&) = default;
    Transceiver(Transceiver&&) = default;
    Transceiver& operator=(const Transceiver&) = default;
    Transceiver& operator=(Transceiver&&) = default;
    ~Transceiver();

    void TurnOn();
    void TurnOff();


private:
    std::atomic<bool> is_on_;
};

#endif // TRANSCEIVER_H_