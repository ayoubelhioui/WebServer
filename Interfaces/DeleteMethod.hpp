#ifndef __DELETEMETHOD_H__
# define __DELETEMETHOD_H__
# include <iostream>
# include <sys/stat.h>
# define FOLDER 2
# define FILE 1
# define UNKNOWN 0

class DeleteMethod {
	public:
		DeleteMethod ( std::string & );
		DeleteMethod ( void );
		~DeleteMethod ( void );
		DeleteMethod ( const DeleteMethod & );
		DeleteMethod	&operator= ( const DeleteMethod & );
		void	deleteTargetedResource ( void );
		void	sendResponse ( void );
	private:
		std::string	_pathToResource;
		short int	_resourceType;
		void		_locateResource ( void );
		void		_deleteResource ( void );
		void		_defineResourceType ( void );
		void		_removeRecursively ( void );
};

#endif