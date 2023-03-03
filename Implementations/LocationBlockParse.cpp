#include "../Interfaces/LocationBlockParse.hpp"

LocationBlockParse::LocationBlockParse( void ) 
	: isDirectoryListingOn(false)
{}

void	LocationBlockParse::getPath(std::vector<std::string> &vec)
{
    if(vec.size() > 3) errorPrinting("error : unknown words at location declaration");
    if(vec[2] != "{")
        errorPrinting("error : expected {");
    this->Location = vec[1];
}

void	LocationBlockParse::findAllowMethods(std::vector<std::string> &vec)
{
    if(vec.size() > 4) errorPrinting("error : too many methods");
    for(size_t i = 1; i < vec.size(); i++)
        this->allowedMethods.push_back(vec[i]);
}

void	LocationBlockParse::fillRedirection(std::vector<std::string> &vec)
{
    if(vec.size() > 2) errorPrinting("error : only one redirection allowed");
    this->Redirection = vec[1];
}

void	LocationBlockParse::setDirlisting(std::vector<std::string> &vec){
    if(vec.size() > 2) errorPrinting("error : only one argument allowed; on or off");
    if(vec[1] == "on") this->isDirectoryListingOn = 1;
    else if (vec[1] == "off") this->isDirectoryListingOn = 0;
    else errorPrinting("error : you must enter on or off when choosing directory listing");
}

void	LocationBlockParse::setRoot(std::vector<std::string> &vec){
    if(vec.size() > 2) errorPrinting("error : too many roots for this location block");
    if(vec[1].length() > 1 && vec[1][0] == '/')
    {
        vec[1] = '.' + vec[1];
    }
    if(vec[1].length() > 1 && vec[1][0] != '/' && vec[1][0] != '.')
    {
        vec[1] = "./" + vec[1];
    }
    if(vec[1].length() > 2 && vec[1][0] == '.' && vec[1][1] != '/')
    {
        vec[1].insert(1, "/");
    }
    this->Root = vec[1];
}

void	LocationBlockParse::setIndexes(std::vector<std::string> &vec){
    for(size_t i = 1; i < vec.size(); i++){
        if(vec[i][0] == '/')
            errorPrinting("indexes must be pure without slash at the beginning");
        this->indexFiles.push_back(vec[i]);
    }
}

void	LocationBlockParse::setCgi(std::vector<std::string> &vec){
    if(vec.size() > 3) errorPrinting("too many arguments for the CGI");
    this->CGI.push_back(make_pair(vec[1], vec[2]));
}

void	LocationBlockParse::setUploadFolder(std::vector<std::string> &vec){
    if(vec.size() > 2) errorPrinting("only the upload file is allowed as argument");
    this->UploadDirectoryPath = vec[1];
}

std::string white_spacenumber(std::string str)
{
    size_t whitespace = 0;
    for(whitespace = 0; whitespace < str.length(); whitespace++){
        if(!isspace(str[whitespace])) break;
    }
    return str.substr(whitespace);
}

bool    all_empty(std::string str)
{
    for(size_t i = 0; i < str.length(); i++){
        if(!isspace(str[i])) return 0;
    }
    return 1;
}

void	LocationBlockParse::locationParse( std::list<std::string>::iterator &it)
{
	while(white_spacenumber(*it).compare("}"))
    {
        if (all_empty(*it) || (*it).length() == 0)
        {
            it++;
            continue;
        }
        std::stringstream line(*it++);
        std::string word;
        std::vector <std::string> vec;
        while (line >> word)
            vec.push_back(word);
        if (vec[0] == "#" || vec[0][0] == '#')
        {
//            if ((vec[0] == "#" && vec[1] == "location") || vec[0] == "#location")
//                errorPrinting("error in location keyword");
            continue ;
        }
        if (vec[0].compare("location") == 0)
            getPath(vec);
        else if (vec[0].compare("allow_methods") == 0)
            findAllowMethods(vec);
        else if (vec[0].compare("return") == 0)
            fillRedirection(vec);
        else if (vec[0].compare("autoindex") == 0)
            setDirlisting(vec);
        else if (vec[0].compare("root") == 0)
            setRoot(vec);
        else if (vec[0].compare("index") == 0)
            setIndexes(vec);
        else if (vec[0].compare("cgi_pass") == 0)
            setCgi(vec);
        else if (vec[0].compare("upload_pass") == 0)
            setUploadFolder(vec);
        else {
            std::string error_msg = "directive " + vec[0] + " is unknown";
            errorPrinting(error_msg.c_str());
        }
    }
    it++;
    while(all_empty(*it)) it++;
}