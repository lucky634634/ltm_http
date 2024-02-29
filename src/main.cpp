#include "http_get.h"

int main(int argc, char* argv[])
{
    std::string hostname = "www.example.com";
    std::string path = "/";
    int sockfd = -1;
    if (argc == 3)
    {
        ExtractURL(argv[1], hostname, path);
        std::cout << hostname << path << std::endl;
    }

    struct addrinfo* result = GetAddrInfo(hostname);
    if (result == NULL)
    {
        freeaddrinfo(result);
        return 1;
    }

    sockfd = CreateSocket(result);
    if (sockfd == -1)
    {
        freeaddrinfo(result);
        return 1;
    }

    if (!ConnectSocket(sockfd, result))
    {
        freeaddrinfo(result);
        return 1;
    }

    if (!SendRequest(sockfd, hostname, path))
    {
        close(sockfd);
        freeaddrinfo(result);
        return 1;
    }

    std::string content = ReceiveResponse(sockfd);
    std::cout << content << std::endl;
    close(sockfd);
    freeaddrinfo(result);


    return 0;
}