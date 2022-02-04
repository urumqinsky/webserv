#include <stdio.h>

int main()
{
	printf("HTTP/1.1 200 OK\r\nServer: HelloWorld\r\nConnection:Keep-Alive\r\nContent-length: 101\r\nContent-Type: text/html\r\n\r\n");

	printf("<!DOCTYPE html><html><head><title></title></head><body><h1>CGI <br>XYZ crew<br>webserver</h1></body></html>");
	return 0;
}
