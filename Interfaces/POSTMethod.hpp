#ifndef POST_METHOD_HPP_
#define POST_METHOD_HPP_

# include "ServerConfiguration.hpp"
# include "Client.hpp"
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
//# include "HttpServer.hpp"
# define UPLOAD_SUCCESS_FILE_PROBLEM "Couldn't Open UploadSuccess File"
# define BODY_SIZE_EXCEPTION "body size exception"
# define TRANSFER_ENCODING_EXCEPTION "Transfer Encoding Exception"
# define BAD_REQUEST_EXCEPTION "Bad Request Exception"
# define TMP_FOLDER_PATH "/tmp/."
# define DEFAULT_UPLOAD_FOLDER "./DefaultUploads"
# define TMP_FOLDER_COULDNT_OPEN "Couldn't Open /Tmp Folder"
# define UPLOAD_FOLDER_COULDNT_OPEN "Couldn't Open Uploads Folder"



class ClientInfo;
class PostMethod{
    private :
        void _searchingForUploadFolder( ClientInfo* );
        void _preparingPostRequest( ClientInfo* );
        void _writeInTempFile( ClientInfo* );
        void _receiveFromClient( ClientInfo* );
        void _searchForCurrentLocation(ClientInfo *);
        bool _isLocationSupportsUpload( ClientInfo * );
        void startPostRequest( ClientInfo *, bool );
        ServerConfiguration _serverConfiguration;
    public :
        int cgiContentLength;
        PostMethod(ServerConfiguration &);
        void receiveTheBody( ClientInfo* );
        void successfulPostRequest( ClientInfo* );
        void handleFirstRead( ClientInfo* );
};

#endif