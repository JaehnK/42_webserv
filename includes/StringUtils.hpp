#pragma once
#include <string>
#include <vector>

class StringUtils {
    public:
        // 문자열 trim (앞뒤 공백 제거)
        static std::string trim(const std::string& str);
        static std::string trimLeft(const std::string& str);
        static std::string trimRight(const std::string& str);
        
        // 대소문자 변환
        static std::string toLower(const std::string& str);
        static std::string toUpper(const std::string& str);
        
        // 문자열 분할
        static std::vector<std::string> split(const std::string& str, char delimiter);
        static std::vector<std::string> split(const std::string& str, const std::string& delimiter);
        
        // 문자열 검사
        static bool startsWith(const std::string& str, const std::string& prefix);
        static bool endsWith(const std::string& str, const std::string& suffix);
        static bool contains(const std::string& str, const std::string& substr);
        static bool isEmpty(const std::string& str);
        static bool isWhitespace(const std::string& str);
        
        // URL 인코딩/디코딩
        static std::string urlEncode(const std::string& str);
        static std::string urlDecode(const std::string& str);
        
        // HTML 인코딩/디코딩
        static std::string htmlEncode(const std::string& str);
        static std::string htmlDecode(const std::string& str);
        
        // 문자열 치환
        static std::string replace(const std::string& str, const std::string& from, const std::string& to);
        static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
        
        // 숫자 변환
        static int toInt(const std::string& str, int defaultValue = 0);
        static long toLong(const std::string& str, long defaultValue = 0);
        static double toDouble(const std::string& str, double defaultValue = 0.0);
        static std::string toString(int value);
        static std::string toString(long value);
        static std::string toString(double value);
        
        // 문자열 조인
        static std::string join(const std::vector<std::string>& strings, const std::string& delimiter);
        
    private:
        // URL 인코딩/디코딩 헬퍼
        static char hexToChar(char c);
        static std::string charToHex(unsigned char c);
        static bool isUnreserved(unsigned char c);
};