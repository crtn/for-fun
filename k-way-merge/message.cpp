#include "message.h"
#include <string>
#include <sstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

Message::Message(Status sts, double val, const std::string &id) :
        status(sts), value(val), identifier(id) { }

std::string Message::dumps() const {
    std::stringstream strm;
    boost::archive::text_oarchive oa(strm);
    oa << status << value << identifier;
    return strm.str();
}

void Message::loads(const std::string &str) {
    std::stringstream strm(str);
    boost::archive::text_iarchive ia(strm);
    ia >> status >> value >> identifier;
}

Message::Message(const std::string &str) {
    this->loads(str);
}

Message::Status Message::get_status() const {
    return status;
}

double Message::get_value() const {
    return value;
}

std::string Message::get_identifier() const {
    return identifier;
}