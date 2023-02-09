#include "parsing.hpp"
void	get_path(std::vector<std::string> &vec, locationBlock &loc)
{
    if(vec.size() > 3) errorPrinting("error : unknown words at location declaration");
    if(vec[2] != "{")
        errorPrinting("error : expected {");
    loc.Location = vec[1];
}

void	fill_allow_methods(std::vector<std::string> &vec, locationBlock &loc)
{
    if(vec.size() > 4) errorPrinting("error : too many methods");
    for(size_t i = 1; i < vec.size(); i++)
        loc.allowedMethods.push_back(vec[i]);
}

void	fill_redirection(std::vector<std::string> &vec, locationBlock &loc)
{
    if(vec.size() > 2) errorPrinting("error : only one redirection allowed");
    loc.Redirection = vec[1];
}
void	set_dirlisting(std::vector<std::string> &vec, locationBlock &loc){
    if(vec.size() > 2) errorPrinting("error : only one argument allowed; on or off");
    if(vec[1] == "on") loc.isDirectoryListingOn = 1;
    else if (vec[1] == "off") loc.isDirectoryListingOn = 0;
    else errorPrinting("error : you must enter on or off when choosing directory listing");
}
void	set_root(std::vector<std::string> &vec, locationBlock &loc){
    if(vec.size() > 2) errorPrinting("error : too many roots for this location block");
    loc.Root = vec[1];
}
void	set_indexes(std::vector<std::string> &vec, locationBlock &loc){
    for(size_t i = 1; i < vec.size(); i++)
        loc.indexFiles.push_back(vec[i]);
}
void	set_cgi(std::vector<std::string> &vec, locationBlock &loc){
    if(vec.size() > 3) errorPrinting("too many arguments for the CGI");
    loc.CGI.push_back(make_pair(vec[1], vec[2]));
}
void	set_upload_file(std::vector<std::string> &vec, locationBlock &loc){
    if(vec.size() > 2) errorPrinting("only the upload file is allowed as argument");
    loc.UploadDirectoryPath = vec[1];
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
void	location_parse(std::list<std::string>::iterator &it, locationBlock &loc)
{
	while(white_spacenumber(*it).compare("}"))
    {
        if (all_empty(*it) || (*it).length() == 0) {
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
            if ((vec[0] == "#" && vec[1] == "location") || vec[0] == "#location")
                errorPrinting("error in location keyword");
            continue ;
        }
        if (vec[0].compare("location") == 0)
            get_path(vec, loc);
        else if (vec[0].compare("allow_methods") == 0)
            fill_allow_methods(vec, loc);
        else if (vec[0].compare("return") == 0)
            fill_redirection(vec, loc);
        else if (vec[0].compare("autoindex") == 0)
            set_dirlisting(vec, loc);
        else if (vec[0].compare("root") == 0)
            set_root(vec, loc);
        else if (vec[0].compare("index") == 0)
            set_indexes(vec, loc);
        else if (vec[0].compare("cgi_pass") == 0)
            set_cgi(vec, loc);
        else if (vec[0].compare("upload_pass") == 0)
            set_upload_file(vec, loc);
        else {
            std::string error_msg = "directive " + vec[0] + " is unknown";
            errorPrinting(error_msg.c_str());
        }
    }
    it++;
    while(all_empty(*it)) it++;
}