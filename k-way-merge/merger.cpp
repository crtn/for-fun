#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <queue>
#include <memory>
#include <tuple>
#include <functional>
#include <algorithm>
#include <iterator>
#include <zmq.hpp>
#include "message.h"

int main() {
    // ports are $ take 10 [10000..]
    zmq::context_t context;
    std::vector<std::shared_ptr<zmq::socket_t>> sockets;
    int port = 9000;
    for(int i = 0; i < 10; ++i) {
        std::shared_ptr<zmq::socket_t> socket(new zmq::socket_t(context, ZMQ_REQ));
        std::string addr = "tcp://localhost:" + std::to_string(port++);
        socket->connect(addr.c_str());
        sockets.push_back(socket);
    }
    using T = std::tuple<double, Message::Status, std::string, std::shared_ptr<zmq::socket_t>>;
    auto cmp = [](const T &x, const T &y) {
        return (std::get<0>(y) < std::get<0>(x));
    };

    std::priority_queue<T, std::vector<T>, std::function<bool(T, T)>> prio(cmp);
    zmq::message_t greeting((void *)"GET", 3, nullptr);
    for(int i = 0; i < 10; ++i) {
        sockets[i]->send(greeting);
        zmq::message_t msg;
        sockets[i]->recv(&msg);
        std::string str(msg.size(), '\0');
        std::memcpy((void *)str.data(), (const void *)msg.data(), msg.size());
        Message message(str);
        auto rec = std::make_tuple(message.get_value(), message.get_status(),
                                   message.get_identifier(), sockets[i]);
        prio.push(rec);
    }

    std::vector<double> mergedData;

    while(!prio.empty()) {
        auto rec = prio.top();
        prio.pop();
        std::clog << "extract value [" << std::get<0>(rec) << "] from generator ["
                << std::get<2>(rec) << "]" << std::endl;
        mergedData.push_back(std::get<0>(rec));
        auto status = std::get<1>(rec);
        if(status == Message::Status::DONE) {
            std::clog << "generator [" << std::get<2>(rec) << "] done." << std::endl;
        } else {
            zmq::message_t msg;
            auto socket = std::get<3>(rec);
            socket->send(greeting);
            socket->recv(&msg);
            std::string str(msg.size(), '\0');
            std::memcpy((void *)str.data(), (const void *)msg.data(), msg.size());
            Message message(str);
            auto rec = std::make_tuple(message.get_value(), message.get_status(),
                                       message.get_identifier(), socket);
            prio.push(rec);
        }
    }

    std::cout << "merged value: " << std::endl;
    std::copy(mergedData.begin(), mergedData.end(),
              std::ostream_iterator<double>(std::cout, "\n"));

    return 0;
}