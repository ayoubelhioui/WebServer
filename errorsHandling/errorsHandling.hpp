#ifndef __ERRORSHANDLING_H__
# define __ERRORSHANDLING_H__

# include "../Interfaces/Client.hpp"
class ClientInfo;
bool    	isTransferEncodingNotChunked(std::map<std::string, std::string> &requestData);
bool    	isNotValidPostRequest(std::map<std::string, std::string> &requestData);
bool    	isBodySizeBigger(ServerConfiguration &serverConfig, unsigned int bodySize);
void		error_414(ClientInfo *, std::string &);
void		error_501(ClientInfo *, std::string &);
void		error_400(ClientInfo *, std::string &);
void		error_500(ClientInfo *, std::string &);
void		error_413(ClientInfo *, std::string &);
void		error_404(ClientInfo *, std::string &);
void    	error_405(ClientInfo *, std::string &);
void    	error_403(ClientInfo *, std::string &);
#endif