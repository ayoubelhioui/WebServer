# include "../Interfaces/DeleteMethod.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

DeleteMethod::DeleteMethod ( std::string &path )
	: _pathToResource(path)
{}

DeleteMethod::DeleteMethod ( void )
{
	
}

DeleteMethod::~DeleteMethod ( void )
{
	
}

DeleteMethod	&DeleteMethod::operator= ( const DeleteMethod &obj )
{
	if (this == &obj)
		return (*this);
	return (*this);
}

DeleteMethod::DeleteMethod ( const DeleteMethod &obj )
{
	*this = obj;
}


/* ----------------------------------------------------- */
/* ------------------ MEMBER FUNCTIONS ----------------- */
/* ----------------------------------------------------- */

void	DeleteMethod::deleteTargetedResource ( void )
{
	this->_locateResource();
	this->_deleteResource();
}

void	DeleteMethod::sendResponse( void )
{}

void	DeleteMethod::_locateResource ( void )
{
	this->_defineResourceType();
	//*  proceed for location stuff 
	if (this->_resourceType ==  FILE)
		remove(this->_pathToResource.c_str());
	else if (this->_resourceType == FOLDER)
		this->removeRecursively();
}

void	DeleteMethod::_defineResourceType ( void )
{
	struct stat	fileStat;

	if (stat(this->_pathToResource.c_str(), &fileStat))
		throw std::runtime_error("Stat syscall failed");
	if (S_ISREG(fileStat.st_mode))
		this->_resourceType = FILE;
	else if (S_ISDIR(fileStat.st_mode))
		this->_resourceType = FOLDER;
	else
		this->_resourceType = UNKNOWN;
}