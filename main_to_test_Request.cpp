#include "Request.hpp"
#include "headers.hpp"


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
    requestText = "GET  / HTTP/1.1\nHost: http.maxkuznetsov.ru\nContent-Type: application/x-www-form-urlencoded";
    // requestText = "GET  / HTTP/1.1\nHost: http.maxkuznetsov.ru";
    test.parseFd(requestText);
}