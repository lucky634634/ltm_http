#include "http_get.h"



void ExtractURL(const std::string& url, std::string& hostname, std::string& path)
{
    std::string temp = url;
    size_t http = url.find("http://");
    if (http != std::string::npos)
    {
        temp = url.substr(http + 7, url.size() - 7);
        path = "/";
    }
    size_t pos = temp.find_first_of("/");
    if (pos == std::string::npos)
    {
        hostname = temp;
        path = "/";
    }
    else
    {
        hostname = temp.substr(0, pos);
        path = temp.substr(pos);
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

void ReadResponse(int sockfd, std::string& header, char*& content, size_t& content_size, bool& binary)
{
    char buffer[BUFFER_SIZE];
    char* temp = NULL;
    size_t bytes_content = 0;
    bool content_len = false;
    binary = false;
    do
    {
        int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1)
        {
            std::cerr << "Error reading response" << std::endl;
            return;
        }
        else if (bytes_received == 0)
        {
            break;
        }

        if (strstr(buffer, "\r\n\r\n") != NULL)
        {
            size_t prefix = strstr(buffer, "\r\n\r\n") - buffer;
            header.append(buffer, prefix);
            if (header.find("Content-Length: ") != std::string::npos)
            {
                content_len = true;
                content_size = atoi(header.substr(header.find("Content-Length: ") + 16).c_str());
            }
            if (header.find("Content-Type: ") != std::string::npos)
            {
                if (header.find("Content-Type: text") == std::string::npos)
                {
                    binary = true;
                }
            }
            bytes_content = bytes_received - prefix - 4;
            temp = new char[bytes_content];
            memcpy(temp, buffer + prefix + 4, bytes_content);
            break;
        }
        header.append(buffer, bytes_received);
    } while (true);

    if (content_len)
    {
        content = new char[content_size];
        memcpy(content, temp, bytes_content);
        delete[] temp;
        while (bytes_content < content_size)
        {
            int bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (bytes_received == -1)
            {
                std::cerr << "Error reading response" << std::endl;
                return;
            }
            memcpy(content + bytes_content, buffer, bytes_received);
            bytes_content += bytes_received;
        }
    }
    else
    {
        int chuckLength = strtol(temp, NULL, 16);
        if (chuckLength == 0)
        {
            content = NULL;
            content_size = 0;
            delete[] temp;
            return;
        }
        std::vector<char> v;
        v.assign(temp, temp + bytes_content);
        bytes_content -= (temp - strstr(temp, "\r\n\r\n")) + 4;

        int bytes_received = recv(sockfd, buffer, chuckLength - bytes_content, 0);
        if (bytes_received == -1)
        {
            std::cerr << "Error reading response" << std::endl;
            return;
        }
        v.insert(v.end() + 1, buffer, buffer + bytes_received);
    }
}

void Test(int sockfd, std::string& header, std::vector<char>& content, bool& binary)
{
    char buffer[BUFFER_SIZE] = { 0 };
    int bytes_received = 0;
    std::vector<char> temp;
    binary = false;
    do
    {
        bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
        if (bytes_received == -1)
        {
            std::cerr << "Error reading response" << std::endl;
            header = "";
            content.clear();
            return;
        }
        else if (bytes_received == 0)
        {
            break;
        }

        temp.insert(temp.end(), buffer, buffer + bytes_received);
        char* tempChar = temp.data();
        if (strstr(tempChar, "\r\n\r\n") != NULL)
        {
            size_t prefix = strstr(tempChar, "\r\n\r\n") - tempChar;
            header = std::string(temp.begin(), temp.begin() + prefix);
            temp.erase(temp.begin(), temp.begin() + header.size() + 4);
            break;

        }
    } while (bytes_received > 0);

    if (header.find("Content-Type: ") != std::string::npos)
    {
        if (header.find("Content-Type: text") == std::string::npos)
        {
            binary = true;
        }
    }

    if (temp.size() == 0)
    {
        return;
    }

    if (header.find("Content-Length: ") != std::string::npos)
    {
        size_t content_size = strtol(header.substr(header.find("Content-Length: ") + 16).c_str(), NULL, 10);
        content = temp;
        while (content.size() < content_size)
        {
            bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
            if (bytes_received == -1)
            {
                std::cerr << "Error reading response" << std::endl;
                header = "";
                content.clear();
                return;
            }
            content.insert(content.end(), buffer, buffer + bytes_received);
        }
    }
    else
    {
        int chunkLength = 0;
        do
        {
            do
            {
                chunkLength = strtol(temp.data(), NULL, 16);
                if (chunkLength == 0)
                {
                    return;
                }
                if (chunkLength > temp.size() - 2)
                {
                    break;
                }

                char* tempChar = temp.data();
                size_t prefix = strstr(tempChar, "\r\n") - tempChar;
                temp.erase(temp.begin(), temp.begin() + prefix + 2);
                content.insert(content.end(), temp.begin(), temp.begin() + chunkLength);
                temp.erase(temp.begin(), temp.begin() + chunkLength + 2);
            } while (temp.size() > 0);

            int bytes_received = recv(sockfd, buffer, chunkLength, 0);
            if (bytes_received == -1)
            {
                std::cerr << "Error reading response" << std::endl;
                header = "";
                content.clear();
                return;
            }
            temp.insert(temp.end(), buffer, buffer + bytes_received);

        } while (temp.size() > 0);

    }


}



