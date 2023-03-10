#ifndef __DELETEMETHOD_H__
# define __DELETEMETHOD_H__
# include <iostream>
# include "Client.hpp"
# include <sys/stat.h>
# include <dirent.h>
# include <unistd.h>
# define FOLDER 2
# define FILE 1
# define UNKNOWN 0

class DeleteMethod {
	public:
		DeleteMethod ( std::string &, ClientInfo *, ServerConfiguration & );
		DeleteMethod ( void );
		~DeleteMethod ( void );
		DeleteMethod ( const DeleteMethod & );
		DeleteMethod	&operator= ( const DeleteMethod & );
		void	deleteTargetedResource ( void );
		void	sendResponse ( void );
		ClientInfo				*client;
		ServerConfiguration	    serverConfig;
	private:
		std::string	_pathToResource;
		short int	_resourceType;
		void		_locateResource ( void );
		void		_deleteResource ( void );
		void		_defineResourceType ( void );
		void		_removeRecursively ( const char * );
};

#endif
