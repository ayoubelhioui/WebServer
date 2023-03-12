#ifndef _LOCATION_BLOCK_PARSE_HPP_
#define _LOCATION_BLOCK_PARSE_HPP_

#include <iostream>
#include <list>
#include <vector>
#include <sstream>
#include <map>


class LocationBlockParse{
    public:
        LocationBlockParse( void );
        std::list<std::string> indexFiles;
        std::list<std::pair<std::string, std::string> > CGI;
        std::list<std::string> allowedMethods;
        std::string Location;
        std::string Redirection;
        std::string Root;
        std::string UploadDirectoryPath;
        std::string Path_Info;
        bool    isDirectoryListingOn;
        void	locationParse(std::list<std::string>::iterator &);
        void	getPath(std::vector<std::string> &);
        void	findAllowMethods(std::vector<std::string> &);
        void	fillRedirection(std::vector<std::string> &);
        void	setDirlisting(std::vector<std::string> &);
        void	setRoot(std::vector<std::string> &);
        void	setIndexes(std::vector<std::string> &);
        void	setCgi(std::vector<std::string> &);
        void	setUploadFolder(std::vector<std::string> &);
};

bool    isValidNumber(std::string &);
void    errorPrinting(const char *);
#endif