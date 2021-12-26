#include "static.h"

#include <boost/certify/https_verification.hpp>
#include <string>
#include <vector>

void hexchar(unsigned char c, unsigned char& hex1, unsigned char& hex2) {
    hex1 = c / 16;
    hex2 = c % 16;
    hex1 += hex1 <= 9 ? '0' : 'a' - 10;
    hex2 += hex2 <= 9 ? '0' : 'a' - 10;
}

std::string urlencode(std::string s) {
    const char* str = s.c_str();
    std::vector<char> v(s.size());
    v.clear();
    for (size_t i = 0, l = s.size(); i < l; i++) {
        char c = str[i];
        if ((c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') ||
            (c >= 'A' && c <= 'Z') || c == '-' || c == '_' || c == '.' ||
            c == '!' || c == '~' || c == '*' || c == '\'' || c == '(' ||
            c == ')') {
            v.push_back(c);
        } else if (c == ' ') {
            v.push_back('+');
        } else {
            v.push_back('%');
            unsigned char d1, d2;
            hexchar(c, d1, d2);
            v.push_back(d1);
            v.push_back(d2);
        }
    }

    return std::string(v.cbegin(), v.cend());
}

void load_ssl_certificates(boost::asio::ssl::context& ssl_context) {
#if defined(_WIN32) || defined(__APPLE__)
    boost::certify::enable_native_https_server_verification(ssl_context);
#else
    ssl_context.set_default_verify_paths();
#endif
}
