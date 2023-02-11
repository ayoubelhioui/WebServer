#include "parsing/parsing.hpp"

bool getMatchedLocation(std::string &Uri, std::list<Parsing> &configFileData)
{
    std::list<Parsing>::iterator serverData = configFileData.begin();
    while (serverData != configFileData.end())
    {
        std::list<locationBlock>::iterator serverLocation = serverData->Locations.begin();
        while (serverLocation != serverData->Locations.end())
        {
            if (Uri == serverLocation->Location)
            {
                if (!isIndexExistInLocation(*serverLocation))
                    return (false);
                return (true);
            }
            serverLocation++;
        }
        serverData++;
    }
    return (false);
}

bool isLocationHaveRedirection(std::string &redirection)
{
    return (redirection.length() > 0);
}

bool isMethodAllowedInLocation(std::string &requestMethod, std::list<std::string> &allowedMethods)
{
    std::list<std::string>::iterator method = std::find(begin(allowedMethods), end(allowedMethods), requestMethod);
    return (method != allowedMethods.end() and (*method == "POST" || *method == "GET" || *method == "DELETE"));
}

bool isIndexExistInLocation(locationBlock &location)
{
    return !(location.indexFiles.empty());
}
