#ifndef _LOCATION_BLOCK_PARSE_HPP_
#define _LOCATION_BLOCK_PARSE_HPP_

#include <iostream>
#include <list>
//#inclu
class locationBlockParse{
    public:
        locationBlockParse();
        std::list<std::string> indexFiles;
        std::list<std::pair<std::string, std::string> > CGI;
        std::list<std::string> allowedMethods;
        std::string Location;
        std::string Redirection;
        std::string Root;
        std::string UploadDirectoryPath;
        std::string Path_Info;
        std::string answerFileForDirectory;
        bool    isDirectoryListingOn;
        void	locationParse(std::list<std::string>::iterator &it);
        void	getPath(std::vector<std::string> &vec);
        void	findAllowMethods(std::vector<std::string> &vec);
        void	fillRedirectionirection(std::vector<std::string> &vec);
        void	setDirlisting(std::vector<std::string> &vec);
        void	setRoot(std::vector<std::string> &vec);
        void	setIndexes(std::vector<std::string> &vec);
        void	setCgi(std::vector<std::string> &vec);
        void	setUploadFolder(std::vector<std::string> &vec);
};

#endif