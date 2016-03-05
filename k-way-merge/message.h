#ifndef __MESSAGE_H__
#define __MESSAGE_H__
#include <string>

class Message {
public:
    enum class Status { MORE, DONE };

    Message() = delete;
    Message(Status sts, double val, const std::string& id);
    Message(const Message&) = default;
    Message(Message&&) = default;
    Message& operator=(const Message&) = default;
    Message& operator=(Message&&) = default;

    Message(const std::string&);

    std::string dumps() const;
    void loads(const std::string&);

    Status get_status() const;
    double get_value() const;
    std::string get_identifier() const;
private:
    Status status;
    double value;
    std::string identifier;
};

#endif // __MESSAGE_H__