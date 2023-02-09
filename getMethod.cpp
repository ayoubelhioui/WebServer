#include "parsing/parsing.hpp"

void	requestFoundInRoot(std::string path, std::list<locationBlock> Locations){
	for(std::list<locationBlock>::iterator beg = Locations.begin(); beg != Locations.end(); beg++){
		locationBlock loc = (*beg);
		std::string res = loc.Location;
		int len = path.length() - 1; 
		int	index_last = len;
		if(path[len] == '/') len--;
		bool is_file_last = 0, point = 0, is_dir = 0, is_file = 0;
		for(; len >= 0; len--){
			if(path[len] == '.')
				is_file = point = 1;
			if(path[len] == '/' && point){
				is_file_last = 1;
				index_last = len;
				break;
			}
			else if (path[len] == '/' && !point) break;
		}
		if(res[res.length() - 1] != '/') res += '/';
		if(path[path.length() - 1] == '/') path += '/';
		std::string full_path = path.substr(0, index_last + 1);
		std::cout << "full is " << std::endl;
	}
}
void	handle_get_method(std::map<std::string, std::string> &request, Parsing &server){
	std::string path = request["path"];
	requestFoundInRoot(request["path"], server.Locations);
}
