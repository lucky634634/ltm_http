#include "http_get.h"

int main(int argc, char* argv[])
{
    std::string hostname = "www.example.com";
    std::string path = "/";
    std::string output = "output.txt";
    int sockfd = -1;
    if (argc == 3)
    {
        ExtractURL(argv[1], hostname, path);
        std::cout << hostname << std::endl << path << std::endl;
        output = argv[2];
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

    std::string header;
    char* content = NULL;
    size_t content_size = 0;
    bool binary = false;
    ReadResponse(sockfd, header, content, content_size, binary);
    std::cout << header << std::endl;
    // std::cout << content_size << std::endl;

    if (binary)
    {
        std::ofstream file(output, std::ios::binary);
        file.write(content, content_size);
        file.close();
    }
    else
    {
        std::ofstream file(output);
        file.write(content, content_size);
        file.close();
    }

    close(sockfd);
    freeaddrinfo(result);


    return 0;
}