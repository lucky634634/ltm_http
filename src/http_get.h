#ifndef HTTP_GET_H
#define HTTP_GET_H

#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <netdb.h>
#include <string.h>
void ExtractURL(const std::string& url, std::string& hostname, std::string& path);
struct addrinfo* GetAddrInfo(const std::string& hostname);
int CreateSocket(struct addrinfo* result);
bool ConnectSocket(int sockfd, struct addrinfo* result);
bool SendRequest(int sockfd, const std::string& hostname, const std::string& path);
std::string ReceiveResponse(int sockfd);

#endif