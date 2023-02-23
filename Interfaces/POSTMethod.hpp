#ifndef POST_METHOD_HPP_
#define POST_METHOD_HPP_

# include "ServerConfiguration.hpp"
# include "Client.hpp"
//# include "HttpServer.hpp"
# define UPLOAD_SUCCESS_FILE_PROBLEM "Couldn't Open UploadSuccess File"
# define BODY_SIZE_EXCEPTION "body size exception"
# define TRANSFER_ENCODING_EXCEPTION "Transfer Encoding Exception"
# define BAD_REQUEST_EXCEPTION "Bad Request Exception"
# define TMP_FOLDER_PATH "/tmp/."
# define UPLOADS_FOLDER_PATH "uploads/"
# define TMP_FOLDER_COULDNT_OPEN "Couldn't Open /Tmp Folder"
# define UPLOAD_FOLDER_COULDNT_OPEN "Couldn't Open Uploads Folder"



class PostMethodExceptions : std::runtime_error{
    public:
        PostMethodExceptions( const std::string & );
};
class ClientInfo;
class PostMethod{
    private :
        void writeInTempFile( ClientInfo* );
        void receiveFromClient( ClientInfo* );
        void moveFileToUploads( ClientInfo* );
        ServerConfiguration _serverConfiguration;
public :
        PostMethod(ServerConfiguration &);
        void preparingPostRequest( ClientInfo* );
        void isValidPostRequest( ClientInfo* );
        void serveClient( ClientInfo* );
        void  successfulPostRequest( ClientInfo* );
};

#endif