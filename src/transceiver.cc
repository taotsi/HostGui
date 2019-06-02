#include "transceiver.h"
#include <sstream>


Transceiver::Transceiver(){
    thread_ = std::thread{&Transceiver::ThreadMain, this};
}

Transceiver::~Transceiver(){
    if(thread_.joinable()){
        thread_.join();
    }
}

bool Transceiver::is_on(){
    return static_cast<bool>(is_on_);
}
void Transceiver::TurnOn(){
    is_on_ = true;
}

void Transceiver::TurnOff(){
    is_on_ = false;
}

// TODO:
void Transceiver::Transmit(){

}
// TODO:
void Transceiver::Receive(){

}

void Transceiver::ThreadMain(){
    std::string line;
    // TODO: the getline loop is an example, you should use socket or something like that
    while(getline(std::cin, line) && is_on_){
        if(line == "exit"){
            TurnOff();
        }
        /* collect from subscribers and transmit them to server*/
        for(auto& [name, cpns] : subscriber_register_){
            for(auto &it : cpns){
                // TODO: std::cout is an example, you should use socket or something like that
                std::cout << it->Read() << "\n";
            }
        }

        /* receive a message from server and forward to subscribers */
        std::stringstream ss{line};
        std::string name;
        if(ss.good()){
            ss >> name;
        }else{
            std::cout << "wrong format of message!\n";
            continue;
        }
        unsigned int n;
        if(ss.good()){
            ss >> n;
            if(n>10){
                std::cout << "WARNING: is N too big?\n";
            }
        }else{
            std::cout << "wrong format of message!\n";
            continue;
        }
        if(!ss.good()){
            std::cout << "wrong format of message!\n";
            continue;
        }
        auto cpn_itr = subscriber_register_.find(name);
        if(cpn_itr != subscriber_register_.end()){
            bool data_broken = false;
            while(ss.good()){
                std::string msg_temp;
                std::string msg;
                for(int i=0; i<n; i++){
                    if(ss.good()){
                        ss >> msg_temp;
                        msg.append(" ");
                        msg.append(msg_temp);
                    }else{
                        std::cout << "wrong format of message!\n";
                        data_broken = true;
                        break;
                    }
                }
                if(!data_broken){
                    for(auto &itr : cpn_itr->second){
                        itr->Write(msg);
                    }
                }
            }
        }else{
            std::cout << "no such a message type as " << name << "!\n";
        }
    }
}