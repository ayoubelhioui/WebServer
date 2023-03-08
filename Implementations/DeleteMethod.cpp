# include "../Interfaces/DeleteMethod.hpp"

/* ----------------------------------------------------- */
/* ------------------ CANONICAL FORM ------------------- */
/* ----------------------------------------------------- */

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
