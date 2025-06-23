#include "StringUtils.hpp"
#include <algorithm>
#include <sstream>
#include <limits.h>

std::string StringUtils::trim(const std::string& str) {
    return trimLeft(trimRight(str));
}

std::string StringUtils::trimLeft(const std::string& str) {
    size_t start = str.find_first_not_of(" \t\r\n");
    return (start == std::string::npos) ? "" : str.substr(start);
}

std::string StringUtils::trimRight(const std::string& str) {
    size_t end = str.find_last_not_of(" \t\r\n");
    return (end == std::string::npos) ? "" : str.substr(0, end + 1);
}

// 대소문자 변환
std::string StringUtils::toLower(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::tolower);
    return result;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string result = str;
    std::transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// 문자열 분할
std::vector<std::string> StringUtils::split(const std::string& str, char delimiter) {
    std::vector<std::string> result;
    std::stringstream ss(str);
    std::string item;
    
    while (std::getline(ss, item, delimiter)) {
        result.push_back(item);
    }
    
    return result;
}

std::vector<std::string> StringUtils::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);
    
    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    
    result.push_back(str.substr(start));
    return result;
}

// 문자열 검사
bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.length() >= prefix.length() && 
           str.compare(0, prefix.length(), prefix) == 0;
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.length() >= suffix.length() && 
           str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0;
}

bool StringUtils::contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

bool StringUtils::isEmpty(const std::string& str) {
    return str.empty();
}

bool StringUtils::isWhitespace(const std::string& str) {
    return str.find_first_not_of(" \t\r\n") == std::string::npos;
}

// URL 인코딩/디코딩
std::string StringUtils::urlEncode(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 3); // 최대 3배까지 늘어날 수 있음
    
    for (size_t i = 0; i < str.length(); ++i) {
        unsigned char c = static_cast<unsigned char>(str[i]);
        
        if (isUnreserved(c)) {
            result += c;
        } else {
            result += '%';
            result += charToHex(c);
        }
    }
    
    return result;
}

std::string StringUtils::urlDecode(const std::string& str) {
    std::string result;
    result.reserve(str.length());
    
    for (size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '%' && i + 2 < str.length()) {
            char hex1 = str[i + 1];
            char hex2 = str[i + 2];
            
            if (std::isxdigit(hex1) && std::isxdigit(hex2)) {
                char decoded = (hexToChar(hex1) << 4) | hexToChar(hex2);
                result += decoded;
                i += 2; // %XX를 건너뛰기
            } else {
                result += str[i];
            }
        } else if (str[i] == '+') {
            result += ' '; // + 는 공백으로 디코딩
        } else {
            result += str[i];
        }
    }
    
    return result;
}

// HTML 인코딩/디코딩
std::string StringUtils::htmlEncode(const std::string& str) {
    std::string result;
    result.reserve(str.length() * 2); // 대략적인 크기 예약
    
    for (size_t i = 0; i < str.length(); ++i) {
        switch (str[i]) {
            case '<':  result += "&lt;"; break;
            case '>':  result += "&gt;"; break;
            case '&':  result += "&amp;"; break;
            case '"':  result += "&quot;"; break;
            case '\'': result += "&#39;"; break;
            default:   result += str[i]; break;
        }
    }
    
    return result;
}

std::string StringUtils::htmlDecode(const std::string& str) {
    std::string result = str;
    
    result = replaceAll(result, "&lt;", "<");
    result = replaceAll(result, "&gt;", ">");
    result = replaceAll(result, "&amp;", "&");
    result = replaceAll(result, "&quot;", "\"");
    result = replaceAll(result, "&#39;", "'");
    
    return result;
}

// 문자열 치환
std::string StringUtils::replace(const std::string& str, const std::string& from, const std::string& to) {
    std::string result = str;
    size_t pos = result.find(from);
    
    if (pos != std::string::npos) {
        result.replace(pos, from.length(), to);
    }
    
    return result;
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return str;
    }
    
    std::string result = str;
    size_t pos = 0;
    
    while ((pos = result.find(from, pos)) != std::string::npos) {
        result.replace(pos, from.length(), to);
        pos += to.length();
    }
    
    return result;
}

// 숫자 변환
int StringUtils::toInt(const std::string& str, int defaultValue) {
    if (str.empty()) {
        return defaultValue;
    }
    
    char* endptr;
    long value = std::strtol(str.c_str(), &endptr, 10);
    
    // 변환이 실패했거나 범위를 벗어난 경우
    if (*endptr != '\0' || value < INT_MIN || value > INT_MAX) {
        return defaultValue;
    }
    
    return static_cast<int>(value);
}

long StringUtils::toLong(const std::string& str, long defaultValue) {
    if (str.empty()) {
        return defaultValue;
    }
    
    char* endptr;
    long value = std::strtol(str.c_str(), &endptr, 10);
    
    if (*endptr != '\0') {
        return defaultValue;
    }
    
    return value;
}

double StringUtils::toDouble(const std::string& str, double defaultValue) {
    if (str.empty()) {
        return defaultValue;
    }
    
    char* endptr;
    double value = std::strtod(str.c_str(), &endptr);
    
    if (*endptr != '\0') {
        return defaultValue;
    }
    
    return value;
}

std::string StringUtils::toString(int value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string StringUtils::toString(long value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

std::string StringUtils::toString(double value) {
    std::ostringstream oss;
    oss << value;
    return oss.str();
}

// 문자열 조인
std::string StringUtils::join(const std::vector<std::string>& strings, const std::string& delimiter) {
    if (strings.empty()) {
        return "";
    }
    
    std::ostringstream oss;
    oss << strings[0];
    
    for (size_t i = 1; i < strings.size(); ++i) {
        oss << delimiter << strings[i];
    }
    
    return oss.str();
}

// 헬퍼 함수들
char StringUtils::hexToChar(char c) {
    if (c >= '0' && c <= '9') return c - '0';
    if (c >= 'A' && c <= 'F') return c - 'A' + 10;
    if (c >= 'a' && c <= 'f') return c - 'a' + 10;
    return 0;
}

std::string StringUtils::charToHex(unsigned char c) {
    const char* hex = "0123456789ABCDEF";
    std::string result;
    result += hex[c >> 4];
    result += hex[c & 0x0F];
    return result;
}

bool StringUtils::isUnreserved(unsigned char c) {
    return std::isalnum(c) || c == '-' || c == '.' || c == '_' || c == '~';
}