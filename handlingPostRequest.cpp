#include "parsing/parsing.hpp"

bool ifLocationSupportUpload(locationBlock &location)
{
    return !(location.UploadDirectoryPath.empty());
}

bool handlingPostRequest(postRequestStruct &postRequest, int &boundarySize)
{
//    std::cout << "**************************" << std::endl;
//    for (int i = 0; i < strlen(postRequest.client->request); i++)
//        std::cout << postRequest.client->request[i];
//    std::cout << std::endl;
//    std::cout << "**************************" << std::endl;
    if(isNotValidPostRequest(postRequest.requestData))
    {
        error_400(postRequest.clientData, postRequest.clientDataIterator);
        close(postRequest.client->socket);
        std::list<client_info *>::iterator temp_it = postRequest.clientDataIterator;
        postRequest.clientDataIterator++;
        postRequest.clientData.erase(temp_it);
        return (false);
    }
    if(isTransferEncodingNotChunked(postRequest.requestData))
    {
        error_501(postRequest.clientData, postRequest.clientDataIterator);
        close(postRequest.client->socket);
        std::list<client_info *>::iterator temp_it = postRequest.clientDataIterator;
        postRequest.clientDataIterator++;
        postRequest.clientData.erase(temp_it);
        return (false);
    }
    std::map<std::string, std::string>::iterator content = postRequest.requestData.find("Content-Length:");
    if(content != postRequest.requestData.end())
    {
        int body_size = std::stoi(postRequest.requestData["Content-Length:"]);
        if(isBodySizeBigger(postRequest.configFileData, body_size, postRequest.client))
        {
            error_413(postRequest.clientData, postRequest.clientDataIterator);
            close(postRequest.client->socket);
            std::list<client_info *>::iterator temp_it = postRequest.clientDataIterator;
            postRequest.clientDataIterator++;
            postRequest.clientData.erase(temp_it);
            return (false);
        }
    }
    srand(time(NULL));
    int nameGenerating = rand();
     std::stringstream ss;
    ss << nameGenerating;
    std::ofstream file("uploads/" + ss.str() + get_real_format(postRequest.requestData["Content-Type:"].c_str()), std::ios::binary);
    if (!file.is_open())
        errorPrinting("Couldn't Open Upload File");
    int readingIndex = 0;
    int totalToWrite = postRequest.client->received - postRequest.bodyIndex - boundarySize - 4, toWrite = 0;
    int i = 0;
    while (1)
    {
        toWrite = (totalToWrite > 1024) ? 1024 : totalToWrite;
        totalToWrite -= toWrite;
        std::cout << "i will write : " << toWrite << std::endl;
        file.write(postRequest.client->request + readingIndex + postRequest.bodyIndex, toWrite);
        readingIndex += toWrite;
        if (toWrite < 1024)
            break ;
    }
    file.close();
    return (true);
}