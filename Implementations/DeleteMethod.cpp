# include "../Interfaces/DeleteMethod.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

DeleteMethod::DeleteMethod ( std::string &path )
	: _pathToResource(path)
{
	std::cout << this->_pathToResource << std::endl;
}

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
{
	std::cout << this->_pathToResource << " DELETED SUCCESSFULLY" << std::endl;
}

void	DeleteMethod::_removeRecursively ( const char *path )
{
	// const char	*newPath;

	
	// if (rmdir(path) == 0)
	// 	std::cout << std::string(path) << "REMOVED SUCCESSFULLY !!" << std::endl;
	// else
	// {
	// 	if (path[0] == '/')
	// 		newPath = strstr(path + 1, "/");
	// 	else
	// 		newPath = strstr(path, "/");
	// 	this->_removeRecursively (newPath);
	// }
	DIR	*dir_ptr = opendir(path);
	if (dir_ptr == NULL) 
	{
		throw std::runtime_error("Cannot access " + this->_pathToResource);
		return;
	}

	struct dirent* dir_entry_ptr;
	while ((dir_entry_ptr = readdir(dir_ptr)) != NULL) 
	{
		if (strcmp(dir_entry_ptr->d_name, ".") == 0 || strcmp(dir_entry_ptr->d_name, "..") == 0) {
			continue;
		}

		std::string file_path = std::string(path) + "/" + dir_entry_ptr->d_name;
		if (dir_entry_ptr->d_type == DT_DIR) 
		{
			_removeRecursively(file_path.c_str());
		} 
		else 
		{
			if (access(file_path.c_str(), W_OK) != 0) 
			{
				std::cout << "Error: permission denied to delete file " << file_path << std::endl;
				continue;
			}
			if (remove(file_path.c_str()) == -1) 
			{
				
			}
		}
	}

	closedir(dir_ptr);

	// delete folder
	if (rmdir(folder_path) != 0) {
		std::cout << "Error deleting folder " << folder_path << std::endl;
	}
}

void	DeleteMethod::_deleteResource ( void )
{
	/*-- path/to/the/targeted/resources --*/
	/*
		FILE --> delete simply

				    ___ EMPTY FOLDER --> rmdir(pathToFolder);
				   /
                  /
		FOLDER ---
				  \
				   \___ NON-EMPTY FOLDER --> remove recursively(pathToFolder);
	*/

	this->_defineResourceType();
	if (this->_resourceType ==  FILE)
		remove(this->_pathToResource.c_str());
	else if (this->_resourceType == FOLDER)
	{
		if (opendir(this->_pathToResource.c_str()) == 0)
			throw std::runtime_error("Cannot access " + this->_pathToResource);
		this->_removeRecursively( this->_pathToResource.c_str() );
	}
}

void	DeleteMethod::_locateResource ( void )
{
	//*  proceed for location stuff 
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

/*
	DELETE /file.html HTTP/1.1
*/

/*
#include <iostream>
#include <cstring>
#include <dirent.h>

void remove_folder(const char* folder_path) {
  DIR* dir_ptr = opendir(folder_path);
  if (dir_ptr == NULL) {
    std::cout << "Error opening folder." << std::endl;
    return;
  }

  struct dirent* dir_entry_ptr;
  while ((dir_entry_ptr = readdir(dir_ptr)) != NULL) {
    if (strcmp(dir_entry_ptr->d_name, ".") == 0 || strcmp(dir_entry_ptr->d_name, "..") == 0) {
      // skip current and parent directory entries
      continue;
    }

    std::string file_path = std::string(folder_path) + "/" + dir_entry_ptr->d_name;
    if (dir_entry_ptr->d_type == DT_DIR) {
      // recursively remove subdirectories
      remove_folder(file_path.c_str());
    } else {
      // delete file
      if (remove(file_path.c_str()) != 0) {
        std::cout << "Error deleting file " << file_path << std::endl;
      }
    }
  }

  closedir(dir_ptr);

  // delete folder
  if (rmdir(folder_path) != 0) {
    std::cout << "Error deleting folder " << folder_path << std::endl;
  }
}

int main() {
  const char* folder_path = "/path/to/folder";
  remove_folder(folder_path);

  return 0;
}

*/