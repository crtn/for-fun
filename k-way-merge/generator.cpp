#include <iostream>
#include <vector>
#include <chrono>
#include <random>
#include <thread>
#include <mutex>
#include <algorithm>
#include <iterator>
#include <functional>
#include <uuid/uuid.h>
#include <zmq.hpp>
#include "message.h"

std::mutex print_mutex;

class Generator {
public:
    Generator(const std::string addr, unsigned count) noexcept;
    /* make it noexcept, if any exception is thrown, just simply terminate */
    Generator(const Generator&) = delete;
    Generator(Generator&&) = default;
    Generator& operator=(const Generator&) = delete;
    Generator& operator=(Generator&&) = default;
    void run();
private:
    std::string addr;
    unsigned count;
    std::vector<double> data;
    std::string identifier;
    std::unique_ptr<zmq::context_t> context;
    std::unique_ptr<zmq::socket_t> socket;
};

Generator::Generator(const std::string addr, unsigned count) noexcept :
        addr(addr), count(count) {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::normal_distribution<double> dist(5.0, 3.0);    // N(5.0, 3.0)
    data.resize(count);
    std::generate_n(data.begin(), count, std::bind(dist, std::ref(eng)));
    std::sort(data.begin(), data.end());

    uuid_t uuid;
    uuid_generate_random(uuid);
    identifier.resize(36);
    uuid_unparse_lower(uuid, (char *)identifier.data());

    context.reset(new zmq::context_t);
    socket.reset(new zmq::socket_t(*context, ZMQ_REP));
    socket->bind(addr.c_str());

    {
        std::lock_guard<std::mutex> _{print_mutex};
        std::clog << "generator [" << identifier << "] initilized ["
            << count << "] values @ [" << addr << "]" << std::endl;
        std::cout << "generator [" << identifier << "]: " << std::endl;
        std::copy(data.begin(), data.end(), std::ostream_iterator<double>(std::cout, ", "));
        std::cout << std::endl << "--------------------------------" << std::endl;
    }
}

void Generator::run() {
    for(unsigned i = 0; i < count; ++i) {
        Message message((i + 1 == count ) ? Message::Status::DONE : Message::Status::MORE,
                        data[i], identifier);
        auto serialized = message.dumps();
        zmq::message_t msg(serialized.begin(), serialized.end());
        zmq::message_t received;
        socket->recv(&received);
        socket->send(msg);
        {
            std::lock_guard<std::mutex> _{print_mutex};
            std::clog << "generator [" << identifier << "] sent [" << i << "]th value ["
                << data[i] << "] with tag [" << ((i + 1 == count) ? "DONE" : "MORE")
                << "]" << std::endl;
        }
    }
    {
        std::lock_guard<std::mutex> _{print_mutex};
        std::clog << "generator [" << identifier << "] done." << std::endl;
    }
}

int main() {
    std::random_device rd;
    std::mt19937 eng(rd());
    std::uniform_int_distribution<int> dist(10, 25);
    int port = 9000;

    std::vector<std::thread> threads;
    for(int i = 0; i < 10; ++i) {
        int count = dist(eng);
        std::string addr = "tcp://*:" + std::to_string(port++);
        threads.emplace_back([](const std::string &address, int cnt) {
            Generator generator(address, cnt);
            generator.run();
        }, addr /* dont't use std::ref(addr), otherwise racing condition */, count);
    }
    for(auto &thread : threads) {
        thread.join();
    }

    return 0;
}