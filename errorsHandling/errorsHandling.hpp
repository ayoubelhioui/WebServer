#ifndef __ERRORSHANDLING_H__
# define __ERRORSHANDLING_H__

# include "../Interfaces/Client.hpp"

bool    isTransferEncodingNotChunked(std::map<std::string, std::string> &requestData);
bool    isNotValidPostRequest(std::map<std::string, std::string> &requestData);
bool    isBodySizeBigger(ServerConfiguration &serverConfig, unsigned int bodySize);
const char *get_real_format(const char *mime_type);
const char *get_mime_format(const char *type);
void		error_414(std::list<ClientInfo *>::iterator &client);
void		error_501(std::list<ClientInfo *>::iterator &client);
void		error_400(std::list<ClientInfo *>::iterator &client);
void		error_413(std::list<ClientInfo *>::iterator &client);
void		error_404(std::list<ClientInfo *>::iterator &client);
#endif