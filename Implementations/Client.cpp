#include "../Interfaces/Client.hpp"
	
ClientInfo::ClientInfo( void ) : isFirstRead(true) , addressLength(sizeof(this->address)), inReadCgiOut(0), isErrorOccured(false), isServing(false)
, stillWaiting(0), isFirstCgiRead(0), PostFinishedCgi(0), isNotUpload(0) { }

ClientInfo::~ClientInfo( void ){ }


void        ClientInfo::parseQueryString( void ) {
    std::string	word = this->parsedRequest.requestDataMap["path"];
    size_t	foundQuery = word.find('?');
    if(foundQuery != std::string::npos)
    {
        this->parsedRequest.requestDataMap["path"] = word.substr(0, foundQuery);
        this->parsedRequest.queryString = word.substr(foundQuery + 1);
    }
}

ClientInfo	&ClientInfo::operator= ( const ClientInfo &obj )
{
	(void)obj;
	// if (this == &obj)
	// 	return (*this);
	// return (*this);
	return (*this);
}

ClientInfo::ClientInfo ( const ClientInfo &obj )
{
	(void)obj;
	// std::cout << " Yes i got called\n";
	// *this = obj;
}

std::string    ClientInfo::generateRandString ( void )
{
    std::string    randString;
    int            n;

    std::srand(std::time(0));
    const std::string    chars =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz"
        "0123456789";
    n = rand() % 8 + 5;
    for (int i = 0; i < n; ++i) {
        randString += chars[(rand()) % chars.size()];
    }
    return randString;
}

void	ClientInfo::parsingCgiLine(std::string line){
	std::stringstream str(line);
    std::string word;
    str >> word;
    std::string save = word;
    std::string last;
    while (str >> word)
    {
        last += " ";
        last += word;
    }
    last.erase(0, 1);
    cgiMap[save] = last;
}

void	ClientInfo::parseCgiHeader(std::string &header){
	std::string	headerPart(header), line;
    std::size_t found = headerPart.find("\r\n");
    while(found != std::string::npos)
    {
      line = headerPart.substr(0, found);
      parsingCgiLine(line);
      headerPart = headerPart.substr(found + 2);
      found = headerPart.find("\r\n");
    }
	headerPart = headerPart.substr(found + 1);
    found = headerPart.find("\r\n");
    line = headerPart.substr(0, found);
    parsingCgiLine(line);
}

bool    isSlash(char a){
    return (a == '/');
}

void    locationSplit(std::string &currentPath, std::string &pathOffset){
   std::string  splittedOffset;
    size_t foundSlash = currentPath.rfind('/');
    if(foundSlash)
    {
        splittedOffset = currentPath.substr(foundSlash);
        currentPath = currentPath.substr(0, foundSlash);
    }
    else
    {
        splittedOffset = currentPath;
        currentPath = "/";

    }
    pathOffset = splittedOffset + pathOffset;
}

bool    ClientInfo::isThereFileLast(std::string &path,
                        bool &is_file_last,
                        int &index_last)
{
    bool point = 0;
    for(int len = index_last; len >= 0; len--) {
        if (path[len] == '.')
            point = 1;
        if (path[len] == '/' && point) {
            is_file_last = 1;
            index_last = len;
            break;
        }
        else if (path[len] == '/' && !point){
            return 0;
        }
    }
    if(!point){
        return 0;
    }
    return 1;
}

void    ClientInfo::searchForCgi(ClientInfo *client, std::list<LocationBlockParse>::iterator &beg,
                                 std::string &currentPath)
{
    client->_currentLocation = *beg;
    const char *cgi_format = strrchr(currentPath.c_str(), '.') + 1;
    client->cgiIterator = client->_currentLocation.CGI.begin();
    for (; client->cgiIterator != client->_currentLocation.CGI.end(); client->cgiIterator++)
    {
        if (!strcmp(client->cgiIterator->first.c_str(), cgi_format) && !strcmp(cgi_format, "php"))
        {
            client->inReadCgiOut = 1;
            client->servedFileName = currentPath;
            return;
        }
        else if (!strcmp(client->cgiIterator->first.c_str(), cgi_format) && !strcmp(cgi_format, "py"))
        {
            // python cgi
        }
    }
    client->servedFileName = currentPath;
}

bool isCgi(std::pair<std::string, std::string> cgi)
{
    return (cgi.first == "php");
}

void    ClientInfo::checkPathValidation(ClientInfo *client, ServerConfiguration &serverConfig, std::string &currentPath)
{
    std::string pathOffset = "";
    if (currentPath.back() == '/') {
        currentPath.pop_back();
    }
    if (currentPath.front() != '/') {
        currentPath = '/' + currentPath;
    }
    int SlashCounter = std::count_if(currentPath.begin(), currentPath.end(), isSlash) + 1;
    for (int i = 0; i < SlashCounter; i++) // looping through every splitted part of the path
    {
        for (std::list<LocationBlockParse>::iterator beg = serverConfig.Locations.begin();
             beg != serverConfig.Locations.end(); beg++)
        {
            std::string insideLocationPath = (*beg).Location;
            if (insideLocationPath.back() == '/') {
                insideLocationPath.pop_back();
            }
            if (insideLocationPath.front() != '/') {
                insideLocationPath = '/' + insideLocationPath;
            }
            if (insideLocationPath != currentPath) {
                continue;
            }
//            if(client->parsedRequest.requestDataMap["method"] == "POST")
//            {
//                client->_currentLocation = beg;
//                client->cgiIterator = std::find_if((*beg).CGI.begin(), (*beg).CGI.end(), isCgi);
//                return ;
////                for (; client->cgiIterator != (*beg).CGI.end(); client->cgiIterator++)
////                    if (client->cgiIterator->first == "php")
////                        return;
//            }
            client->cgiIterator = (*beg).CGI.end();
            bool is_file_last = 0;
            int len = currentPath.length() - 1;
            int index_last = len;
            if (client->parsedRequest.requestDataMap["method"] != "POST"
                    && isThereFileLast(currentPath, is_file_last, index_last)) // checking the case where the path is already a file and the location exists
            {
                if (currentPath.front() != '.')
                    currentPath = '.' + currentPath;
                std::ifstream fileCheck(currentPath);
                if (fileCheck)
                {
                    this->searchForCgi(client, beg, currentPath);
                }
                fileCheck.close();
                return ;
            }
            else
            { // the case where the path is directory and must be joined with the offset (file or dir)

                if ((*beg).Root.back() == '/' && (*beg).Root.length() > 1)
                    (*beg).Root.pop_back();
                currentPath = (*beg).Root + pathOffset;
                if(client->parsedRequest.requestDataMap["method"] == "POST")
                {
                    client->servedFileName = currentPath;
                    if(client->servedFileName.back() != '/') {
                        client->servedFileName += '/';
                    }
                    client->_currentLocation = *beg;
                    client->cgiIterator = std::find_if((*beg).CGI.begin(), (*beg).CGI.end(), isCgi);
                    return ;
                }
                int rootLength = currentPath.length() - 1;
                if (isThereFileLast(currentPath, is_file_last, rootLength))
                {
                    std::ifstream fileCheck(currentPath);
                    if (fileCheck)
                    {
                        this->searchForCgi(client, beg, currentPath);
                    }
                    fileCheck.close();
                    return ;
                }
                else // handle the case where the path + offset is a directory (must loop through indexes and check if there is a valid path)
                {
                    // if((*beg).isDirectoryListingOn && client->parsedRequest.requestDataMap["method"] == "GET")
                    //     client->servedFileName = directoryListing((*beg).Root, currentPath, client);
                    // else
                    // {
                        for (std::list<std::string>::iterator index_it = (*beg).indexFiles.begin();
                             index_it != (*beg).indexFiles.end(); index_it++)
                        {
                            std::string final_path = currentPath + '/' + (*index_it);
                            std::ifstream check_file(final_path, std::ios::binary);
                            if (check_file)
                            {
                                this->searchForCgi(client, beg, currentPath);
                                return ;
                            }
                            check_file.close();
                        }
                        return ;
                }

            }
        }
        locationSplit(currentPath, pathOffset);
        //	std::string path = client->parsedRequest.requestDataMap["path"];
//	for (std::list<LocationBlockParse>::iterator beg = serverConfig.Locations.begin(); beg != serverConfig.Locations.end(); beg++){
//		LocationBlockParse loc = *beg;
//		std::string res = loc.Location;
//		int len = path.length() - 1;
//		int	index_last = len;
//		if(path[len] == '/')
//			len--;
//		bool is_file_last = 0, point = 0;
//		for(; len >= 0; len--){
//			if(path[len] == '.')
//				point = 1;
//			if(path[len] == '/' && point){
//				is_file_last = 1;
//				index_last = len;
//				break;
//			}
//			else if (path[len] == '/' && !point) break;
//		}
//		if(res[res.length() - 1] != '/') res += '/';
//		std::string full_path = path.substr(0, index_last + 1);
//		if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
//		if(full_path != res) continue;
//		if(loc.isDirectoryListingOn && !is_file_last){
//            std::string root = loc.Root;
//            if(root[root.length() - 1] != '/') root += '/';
//            if(root[0] != '.') root = '.' + root;
//            if(full_path[0] != '.') full_path = '.' + full_path;
//            client->servedFileName = directoryListing(root, full_path, client);
//            return ;
//        }
//        else{
//		    std::string file = path.substr(index_last + 1);
//		    if(!is_file_last && full_path[full_path.length() - 1] != '/') full_path += '/';
//		    if(is_file_last && file[file.length() - 1] == '/') file.erase(file.length() - 1);
//		    std::string root = loc.Root;
//		    if(root[root.length() - 1] != '/') root += '/';
//		    if(file == ""){
//		    	for(std::list<std::string>::iterator index_it = loc.indexFiles.begin(); index_it != loc.indexFiles.end(); index_it++)
//		    	{
//		    		std::string final_path = root + (*index_it);
//		    		if(final_path[0] == '/') final_path = '.' + final_path;
//		    		std::ifstream check_file(final_path, std::ios::binary);
//		    		if(check_file){
//					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
//					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
//                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
//                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
//                            client->cgiContentLength =  "0";
//                            client->cgiContentType = "";
//                            client->CGIexecutedFile(final_path, client, serverConfig, CGIit);
//                            return ;
//                        }
//                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
//                            // python cgi
//                        }
//                    }
//                    client->servedFileName = final_path;
//					return ;
//				}
//		    	else ;
//		    	}
//		    }
//		    else{
//		    	std::string final_path = root + file;
//		    	if(final_path[0] == '/') final_path = '.' + final_path;
//		    	std::ifstream check_file(final_path, std::ios::binary);
//		    	if(check_file){
//					const char *cgi_format = strrchr(final_path.c_str(), '.') + 1;
//					std::list<std::pair<std::string, std::string> >::iterator CGIit = loc.CGI.begin();
//                    for( ; CGIit != loc.CGI.end(); CGIit++ ){
//                        if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "php")){
//                            client->CGIexecutedFile(final_path, client, serverConfig, CGIit);
//                            return ;
//                        }
//                        else if(!strcmp(CGIit->first.c_str(), cgi_format) && !strcmp(cgi_format, "py")){
//                            // python cgi
//                        }
//                    }
//                    client->servedFileName = final_path;
//					return ;
//				}
//		    	else ;
//		    }
//        }
//	}
    }
}

void			ClientInfo::returnPathWithoutFile(std::string &TempPath)
{
    if(TempPath.back() == '/')
        TempPath.pop_back();
    bool point = 0;
    for(int len = TempPath.length() - 1; len >= 0; len--) {
        if (TempPath[len] == '.')
            point = 1;
        if (TempPath[len] == '/' && point) {
            if(len == 0)
                len++;
            this->cgiFileEnd = TempPath.substr(len + 1);
            TempPath = TempPath.substr(0, len);
            return ;
        }
        else if (TempPath[len] == '/' && !point){
            return ;
        }
    }
    return ;
}

void    ClientInfo::CGIexecutedFile( ClientInfo *client, ServerConfiguration &server )
{
    int     pid = 0;
    
    const char * request_method = client->parsedRequest.requestDataMap["method"].c_str(); // POST or GET
    const char * script_name = client->cgiIterator->second.c_str(); // php cgi inside location
    const char * query_string = client->parsedRequest.queryString.length() == 0 ? "" : client->parsedRequest.queryString.c_str();
    const char * server_host = server.serverHost.c_str();
    const char * server_port = server.serverPort.c_str();
    const char * path_info = client->parsedRequest.requestDataMap["path"].c_str();
    const char * content_length = client->cgiContentLength.c_str();
    const char *content_type = client->cgiContentType.c_str();
    setenv("REQUEST_METHOD", request_method, 1);
    setenv("QUERY_STRING", query_string, 1);
    setenv("SCRIPT_NAME", script_name, 1);
    setenv("SERVER_NAME", server_host, 1);
    setenv("SERVER_PORT", server_port, 1);
    setenv("REDIRECT_STATUS", "200", 1);
    setenv("PATH_INFO", path_info, 1);
    setenv("CONTENT_LENGTH", content_length, 1);
    setenv("CONTENT_TYPE", content_type, 1);
    int fdup = 0;
    if(client->isNotUpload)
        fdup = open(client->servedFileName.c_str(), O_RDONLY);
    int fd[2];
    pipe(fd);
    pid = fork();
    if (pid == 0){
        char  *args[3];
        dup2(fd[1], 1);
        if(fdup > 0)
        {
            dup2(fdup, 0);
            close(fdup);
        }
        close(fd[0]);
        close(fd[1]);
        args[0] = (char *) script_name;
        if(client->actionPath.length())
            args[1] = (char *) client->actionPath.c_str();
        else
            args[1] = (char *) client->servedFileName.c_str();
        std::cerr << "args[1] is " << args[1] << std::endl;
        args[2] = NULL;
        if (execve(script_name, args, NULL) == -1)
            exit(1);
    }
    if(fdup > 0)
        close(fdup);
    close(fd[1]);
    client->CgiReadEnd = fd[0];
    client->inReadCgiOut = 1;
    client->stillWaiting = 1;
    client->cgiPid = pid;
}
