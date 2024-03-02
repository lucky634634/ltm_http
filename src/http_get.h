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
#include <fstream>
#include <string.h>
#include <vector>

#define BUFFER_SIZE 1024

void ExtractURL(const std::string& url, std::string& hostname, std::string& path);
struct addrinfo* GetAddrInfo(const std::string& hostname);
int CreateSocket(struct addrinfo* result);
bool ConnectSocket(int sockfd, struct addrinfo* result);
bool SendRequest(int sockfd, const std::string& hostname, const std::string& path);
void ReadResponse(int sockfd, std::string& header, char*& content, size_t& content_size, bool& binary);
void Test(int sockfd, std::string& header, std::vector<char>& content, bool& binary);
#endif