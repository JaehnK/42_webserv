#pragma once
#include "webserv.hpp"

class ConfigDirective
{
    private:
        std::string                 _key;
        std::vector<std::string>    _vals;
    
    public:
        
        ConfigDirective();
        ConfigDirective(const ConfigDirective &rhs);
        ConfigDirective&    operator=(const ConfigDirective &rhs);
        ~ConfigDirective();

        void    setKey(const std::string key);
        void    setValue(const std::string val);
        void    setValue(const std::vector<std::string> vals);
        void    setDirective(const std::pair<std::string, std::vector<std::string> > dir);

        std::string                                         getKey() const;
        std::vector<std::string>                            getValues() const;
        std::pair<std::string, std::vector<std::string> >   getDirective() const;

        // _value의 길이와 인덱스에 해당하는 값을 반환
        int                 len() const;
        const std::string&  operator[](int idx) const;

};

std::ostream& operator<< (std::ostream& os, const ConfigDirective& configDir);