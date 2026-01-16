#include "logmessage.hpp"


logmessage::logmessage(
    const std::string& name,
    const std::string& time,
    const std::string& context,
    const std::string& severity,
    const std::string& text)
    : name(name),          
      time(time),
      context(context),
      severity(severity),
      text(text)
{
    std::cout <<"logmessege constructor\n" ;
}


std::string logmessage::getName() const {
    return name;
}

std::string logmessage::getTime() const 
{
    return time;
}

std::string logmessage::getContext() const 
{
    return context;
}

std::string logmessage::getSeverity() const
{
    return severity;
}

std::string logmessage::getText() const 
{
    return text;
}

void logmessage::setName(const std::string& n) 
{
    name = n;
}

void logmessage::setTime(const std::string& t) 
{
    time = t;
}

void logmessage::setContext(const std::string& c) {
    context = c;
}

void logmessage::setSeverity(const std::string& s) 
{
    severity = s;
}

void logmessage::setText(const std::string& txt)
 {
    text = txt;
}


std::ostream& operator<<(std::ostream& os , const logmessage& msg)
 {

    os << "[" << msg.getTime() << "] "
       << "[" << msg.getSeverity() << "] "
       << msg.getName() << " (" << msg.getContext() << "): "
       << msg.getText();

    return os;
}