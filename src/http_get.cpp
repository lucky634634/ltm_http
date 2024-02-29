#include "http_get.h"



void ExtractURL(const std::string& url, std::string& hostname, std::string& path)
{
    size_t pos = url.find_last_of("/");
    if (pos == std::string::npos)
    {
        hostname = url;
        path = "/";
    }
    else
    {
        hostname = url.substr(0, pos);
        path = url.substr(pos);
    }
}

addrinfo* GetAddrInfo(const std::string& hostname)
{
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    struct addrinfo* result = NULL;

    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    int status = getaddrinfo(hostname.c_str(), "http", &hints, &result);
    if (status == 0)
    {
        return result;
    }

    std::cerr << "getaddrinfo: " << gai_strerror(status) << std::endl;
    freeaddrinfo(result);
    return NULL;
}

int CreateSocket(addrinfo* result)
{
    int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    if (sockfd == -1)
    {
        std::cerr << "Error creating socket" << std::endl;
        return -1;
    }
    return sockfd;
}

bool ConnectSocket(int sockfd, addrinfo* result)
{
    int status = connect(sockfd, result->ai_addr, result->ai_addrlen);
    if (status == -1)
    {
        std::cerr << "Error connecting socket" << std::endl;
        return false;
    }
    return true;
}

bool SendRequest(int sockfd, const std::string& hostname, const std::string& path)
{
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\n\r\n";
    int status = send(sockfd, request.c_str(), request.size(), 0);
    if (status == -1)
    {
        std::cerr << "Error sending request" << std::endl;
        return false;
    }
    return true;
}

std::string ReceiveResponse(int sockfd)
{
    std::string content;
    char buffer[1024];
    int bytes_received = 0;
    // do
    // {
    //     bytes_received = recv(sockfd, buffer, 1023, 0);
    //     if (bytes_received == -1)
    //     {
    //         std::cerr << "Error receiving response" << std::endl;
    //         return "";
    //     }
    //     content.append(buffer, bytes_received);
    // } while (bytes_received > 0);
    bytes_received = recv(sockfd, buffer, 1023, 0);
    if (bytes_received == -1)
    {
        std::cerr << "Error receiving response" << std::endl;
        return "";
    }
    content.append(buffer, bytes_received);
    return content;
}

