#ifndef __DELETEMETHOD_H__
# define __DELETEMETHOD_H__

class DeleteMethod {
	public:
		DeleteMethod ( void );
		~DeleteMethod ( void );
		DeleteMethod ( const DeleteMethod & );
		DeleteMethod	&operator= ( const DeleteMethod & );
		void	deleteTargetedResource ( void );
	private:

};


#endif