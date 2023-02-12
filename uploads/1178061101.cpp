#include "parsing/parsing.hpp"
#include "includes/postRequest.hpp"

void searchForBoundary(std::map<std::string, std::string> &requestData, int &bodyIndex, char *clientRequest, int &boundarySize)
{

    std::map<std::string, std::string>::iterator content = requestData.find("Content-Type:");
    if (content == requestData.end()) {
        return;
    }
    if (content->second.find("boundary=") == std::string::npos) {
        return ;
    }
    std::string boundarySavior = content->second;
    std::string newString(clientRequest);
    int newContentIndex = newString.find("filename=");
    int dotPosition = newString.find(".", newContentIndex);
    int DoubleQuotePosition = newString.find("\"", dotPosition);
    requestData["Content-Type:"] = get_mime_format(newString.substr(dotPosition, DoubleQuotePosition - dotPosition).c_str());
    int newBodyIndex = newString.find("Content-Disposition:");
    newBodyIndex += ret_index(clientRequest + newBodyIndex) + 4;
    bodyIndex = newBodyIndex;
    boundarySize = boundarySavior.length() - boundarySavior.find("=") + 3;
}

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
//bool isPostMethodAllowed(postRequestStruct &postRequest)
//{
//    std::list<std::string>::iterator allowedMethods = postRequest.configFileData.
//}

void isValidPostRequest(postRequestStruct &postRequest)
{
    if(isNotValidPostRequest(postRequest.requestData))
    {
        error_400(postRequest.clientData, postRequest.clientDataIterator);
        throw postRequestExceptions("Bad Request Exception");
    }
    if(isTransferEncodingNotChunked(postRequest.requestData))
    {
        error_501(postRequest.clientData, postRequest.clientDataIterator);
        throw postRequestExceptions("Transfer Encoding Exception");
    }
    std::map<std::string, std::string>::iterator content = postRequest.requestData.find("Content-Length:");
    if(content != postRequest.requestData.end())
    {
        int body_size = std::stoi(postRequest.requestData["Content-Length:"]);
        if(isBodySizeBigger(postRequest.configFileData, body_size, postRequest.client))
        {
            error_413(postRequest.clientData, postRequest.clientDataIterator);
            throw postRequestExceptions("Body Size Exception");
        }
    }
}

//void SuccessfulPostRequest()
//{
//
//}

void handlingPostRequest(postRequestStruct &postRequest, int &boundarySize)
{
     isValidPostRequest(postRequest);
     writingToUploadFile(postRequest, boundarySize);
//    SuccesfulPostRequest();
}