#include "Request.hpp"
#include "libraryHeaders.hpp"


int main(){
    Request test;

    std::string requestText;
    // char buf[1024];
    // std::fstream fs;
    // fs.open("test.txt", std::fstream::out);
    // std::cin.getline(buf, 1024);
    // requestText = buf;
    // fs.close();
    // std::cout << requestText;
    // requestText = "GET  / HTTP/1.1\r\nHost:  http.maxkuznetsov.ru   \r\nContent-Type:   application/x-www-form-urlencoded    \r\n\r\nthis is body\r\n\r\n";
    requestText = "GET  / HTTP/1.1\r\nHost:http.maxkuznetsov.ru\r\nContent-Type:application/x-www-form-urlencoded\r\n\r\n";
    // requestText = "GET  / HTTP/1.1\nHost: http.maxkuznetsov.ru";
    test.parseFd(requestText);
}