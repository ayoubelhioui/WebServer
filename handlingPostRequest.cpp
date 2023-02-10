#include "parsing/parsing.hpp"

bool ifLocationSupportUpload(locationBlock &location)
{
    return !(location.UploadDirectoryPath.empty());
}

void writingToUploadFile(postRequestStruct &postRequest, int &boundarySize)
{
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
    while (true)
    {
        toWrite = (totalToWrite > 1024) ? 1024 : totalToWrite;
        totalToWrite -= toWrite;
        file.write(postRequest.client->request + readingIndex + postRequest.bodyIndex, toWrite);
        readingIndex += toWrite;
        if (toWrite < 1024)
            break ;
    }
    file.close();
}

bool postRequestIsValid(postRequestStruct &postRequest)
{
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
    return (true);
}

void handlingPostRequest(postRequestStruct &postRequest, int &boundarySize)
{
     if (!postRequestIsValid(postRequest))
        return ;
//    srand(time(NULL));
//    int nameGenerating = rand();
//    std::stringstream ss;
//    ss << nameGenerating;
//    std::ofstream file("uploads/" + ss.str() + get_real_format(postRequest.requestData["Content-Type:"].c_str()), std::ios::binary);
//    if (!file.is_open())
//        errorPrinting("Couldn't Open Upload File");
//    int readingIndex = 0;
//    int totalToWrite = postRequest.client->received - postRequest.bodyIndex - boundarySize - 4, toWrite = 0;
//    int i = 0;
//    while (true)
//    {
//        toWrite = (totalToWrite > 1024) ? 1024 : totalToWrite;
//        totalToWrite -= toWrite;
//        file.write(postRequest.client->request + readingIndex + postRequest.bodyIndex, toWrite);
//        readingIndex += toWrite;
//        if (toWrite < 1024)
//            break ;
//    }
//    file.close();
    writingToUploadFile(postRequest, boundarySize);
}