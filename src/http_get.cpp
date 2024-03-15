#include "http_get.h"



void ExtractURL(const std::string& url, std::string& hostname, std::string& path)
{
    // Extracts the hostname and path from the given URL

    // Remove unnecessary copy of the original URL
    std::string temp = url;

    // Remove check for "http://" and adjust temp and path accordingly
    size_t httpPos = url.find("http://");
    if (httpPos != std::string::npos)
    {
        temp = url.substr(httpPos + 7);
        path = "/";
    }

    // Find the position of the first "/" in the adjusted temp string
    size_t slashPos = temp.find('/');
    if (slashPos == std::string::npos)
    {
        // If no "/", set the hostname and path accordingly
        hostname = temp;
        path = "/";
    }
    else
    {
        // Extract the hostname and path based on the position of the "/"
        hostname = temp.substr(0, slashPos);
        path = temp.substr(slashPos);
    }
}

addrinfo* GetAddrInfo(const std::string& host)
{
    // Set up hints for getaddrinfo
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));

    // Set address family and socket type
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    struct addrinfo* result = NULL;
    int status = getaddrinfo(host.c_str(), "http", &hints, &result);

    // Return result if status is 0 (success)
    if (status == 0)
    {
        return result;
    }

    // Clean up and return NULL if an error occurred
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
    // Construct the HTTP request
    std::string request = "GET " + path + " HTTP/1.1\r\nHost: " + hostname + "\r\nUser-Agent: curl/8.4.0"+ "\r\n\r\n";

    // Send the request
    int status = send(sockfd, request.c_str(), request.size(), 0);

    // Check for errors
    if (status == -1)
    {
        std::cerr << "Error sending request" << std::endl;
        return false;
    }

    // Request sent successfully
    return true;
}

// Receives the response from the server and processes it
void ReceiveResponse(int sockfd, std::string& header, std::vector<char>& content, bool& binary)
{
    char buffer[BUFFER_SIZE] = { 0 };
    int bytes_received = 0;
    std::vector<char> temp;
    binary = false;

    // Receive the response from the server
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
        // Process the response header
        if (strstr(tempChar, "\r\n\r\n") != NULL)
        {
            size_t prefix = strstr(tempChar, "\r\n\r\n") - tempChar;
            header = std::string(temp.begin(), temp.begin() + prefix);
            temp.erase(temp.begin(), temp.begin() + header.size() + 4);
            break;

        }
    } while (bytes_received > 0);
    // Check if the response is binary
    if (header.find("Content-Type: ") != std::string::npos && header.find("Content-Type: text") == std::string::npos)
    {
        binary = true;
    }

    // Return if the response is empty
    if (temp.empty())
    {
        return;
    }
    // Process the response content
    if (header.find("Content-Length: ") != std::string::npos)
    {
        // Process the response content with Content-Length
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
        // Process the response content with Transfer-Encoding: chunked
        size_t chunkLength = 0;
        do
        {
            do
            {
                chunkLength = strtol(temp.data(), NULL, 16);
                if (chunkLength == 0)
                {
                    return;
                }
                char* tempChar = temp.data();
                size_t prefix = strstr(tempChar, "\r\n") - tempChar;
                size_t size = temp.size() - prefix - 4;
                if (chunkLength > size)
                {
                    break;
                }

                temp.erase(temp.begin(), temp.begin() + prefix + 2);
                content.insert(content.end(), temp.begin(), temp.begin() + chunkLength);
                temp.erase(temp.begin(), temp.begin() + chunkLength + 2);
            } while (temp.size() > 0);

            bytes_received = recv(sockfd, buffer, BUFFER_SIZE, 0);
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



