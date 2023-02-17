#ifndef __SERVERCONFIGURATION_H__
# define __SERVERCONFIGURATION_H__

# include "locationBlockParse.hpp"
class serverConfiguration {
    public:
        serverConfiguration();
        std::string serverHost;
        std::string serverPort;
        unsigned int clientBodyLimit;
        bool isClosed;
        std::list<std::string> serverName;
        // std::list<std::pair<int, std::string> > errorInfo;
        std::list<locationBlockParse> Locations;
        void fillingDataFirstPart(std::string &);
        void listenKeywordFound(std::vector<std::string> &);
        void serverNameKeywordFound(std::vector<std::string> &);
        void errorPageKeywordFound(std::vector<std::string> &);
        void clientBodySizeKeywordFound(std::vector<std::string> &);
        static void     startParsingFile(std::list<std::string> &, std::list<serverConfiguration> &);
        static void     readingDataFromFile(std::list<std::string> &, std::string &);
        static void     printingParsingData(std::list<serverConfiguration> &);
};

#endif