#ifndef TRANSCEIVER_H_
#define TRANSCEIVER_H_
#include <atomic>
#include <thread>
#include <mutex>
#include <vector>
#include <map>
#include <string>
#include <iostream>
#include <sstream>
#include <memory>
#include "hostgui.h"

/* message format:
    [[message type][ ][N][ ][string 1][ ][string 2][]...[string N]]
*/

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
    bool is_on();
    void AddSubscriber(std::string msg_type, std::shared_ptr<Component> cpn);

private:
    std::thread thread_;
    void ThreadMain();
    std::mutex mtx_;
    std::atomic<bool> is_on_;
    std::map<std::string, std::vector<std::shared_ptr<Component>>> subscriber_register_;

    void Transmit();
    void Receive();
};

#endif // TRANSCEIVER_H_