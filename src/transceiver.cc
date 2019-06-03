#include "transceiver.h"
#include <sstream>


Transceiver::Transceiver(){
    // thread_ = std::thread{&Transceiver::ThreadMain, this};
}

Transceiver::~Transceiver(){
    if(thread_.joinable()){
        thread_.join();
    }
}
void Transceiver::Loop(){
    thread_ = std::thread{&Transceiver::ThreadMain, this};
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
void Transceiver::AddSubscriber(std::string msg_type, std::shared_ptr<Component> cpn){
    subscriber_register_[msg_type].push_back(cpn);
}
void Transceiver::AddPublisher(std::string msg_type, std::shared_ptr<Component> cpn){

}

void Transceiver::ThreadMain(){
    std::string line;
    // TODO: the getline loop is an example, you should use socket or something like that
    while(getline(std::cin, line) && is_on_){
        if(line == "exit"){
            TurnOff();
            break;
        }
        /* collect from subscribers and transmit them to server*/
        for(auto& [msg_type, cpns] : subscriber_register_){
            std::string msg;
            std::cout << "sub: message type: " << msg_type << "\n";
            int c = 0;
            for(auto &it : cpns){
                std::cout << "try reading from component " << c++ << "\n";
                // TODO: std::cout is an example, you should use socket or something like that
                if(it->Read(msg)){
                    std::cout << msg << "\n";
                    // TODO: and forward
                }
            }
        }

        /* receive a message from server and forward to subscribers */
        std::stringstream ss{line};
        std::string msg_type;
        if(ss.good()){
            ss >> msg_type;
        }else{
            std::cout << "wrong format of message!\n";
            continue;
        }
        unsigned int n = 0;
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
        std::cout << "message healthy\n";
        auto cpn_itr = subscriber_register_.find(msg_type);
        if(cpn_itr != subscriber_register_.end()){
            bool data_broken = false;
            while(ss.good()){
                std::string msg_temp;
                std::string msg;
                for(unsigned int i=0; i<n; i++){
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
            std::cout << "no such a message type as " << msg_type << "!\n";
        }
    }
    TurnOff();
}