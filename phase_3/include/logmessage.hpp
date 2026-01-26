#pragma once
#include <iostream>
#include <string>

class logmessage
{
private:
    std::string name;
    std::string time;
    std::string context;
    std::string severity;
    std::string text;

public:
    // Constructors
    logmessage(
        const std::string& name,
        const std::string& time,
        const std::string& context,
        const std::string& severity,
        const std::string& text);
    
    logmessage() = default;
    ~logmessage() = default;
    
    std::string getName() const;
    std::string getTime() const;
    std::string getContext() const;
    std::string getSeverity() const;
    std::string getText() const;
    
    void setName(const std::string& n);
    void setTime(const std::string& t);
    void setContext(const std::string& c);
    void setSeverity(const std::string& s);
    void setText(const std::string& txt);
    
  
   friend std::ostream& operator<<(std::ostream& os ,const logmessage& msg );
};