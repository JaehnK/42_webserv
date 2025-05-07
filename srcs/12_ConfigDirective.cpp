#include "webserv.hpp"

ConfigDirective::ConfigDirective()
{
    _vals = std::vector<std::string> ();
}

ConfigDirective::ConfigDirective(const ConfigDirective &rhs)
{
    *this = rhs;
}

ConfigDirective&    ConfigDirective::operator=(const ConfigDirective &rhs)
{
    if (this != &rhs)
    {
        this->_key = getKey();
        this->_vals = getValues();
    }
    return (*this);
}

ConfigDirective::~ConfigDirective()
{}

void    ConfigDirective::setKey(const std:: string key)
{
    this->_key = key;
}

void    ConfigDirective::setValue(const std::string val)
{
    this->_vals.push_back(val);
}

void    ConfigDirective::setValue(const std::vector<std::string>& vals)
{
    this->_vals.insert(this->_vals.end(), vals.begin(), vals.end());
}


void    ConfigDirective::setDirective(const std::pair<std::string, std::vector<std::string>> &dir)
{
    this->_key = dir.first;
    this->_vals = dir.second;
}

std::string ConfigDirective::getKey() const
{
    return (this->_key);
}

std::vector<std::string> ConfigDirective::getValues() const
{
    return (this->_vals);
}

std::pair<std::string, std::vector<std::string> > \
        ConfigDirective::getDirective() const
{
    std::pair<std::string, std::vector<std::string> > directive;

    directive.first = this->_key;
    directive.second = this->_vals;
    return (directive);
}

int ConfigDirective::len() const
{
    return (this->_vals.size());
}

std::string& ConfigDirective::operator[](int idx) const
{
    return (this->_vals[idx]);
}

std::ostream& operator<< (std::ostream& os, const ConfigDirective& configDir)
{
    os << "_Key:" << configDir.getKey() << "Value Count: " << configDir.len() << "\n";
    for (int i = 0; i < configDir.len(); i++)
    {
        os << i < " : " << configDir[i] << "\n";
    }
    os << "----------" << std::endl;
    return (os);
}
