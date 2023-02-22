#ifndef POST_REQUEST_HPP_
#define POST_REQUEST_HPP_

# include "Client.hpp"
# include "ServerConfiguration.hpp"
# include "HttpServer.hpp"
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
        PostMethodExceptions(const std::string &errorMessage);
};


class PostMethod{
    private :
        ServerConfiguration _serverConfiguration;
        std::list<ClientInfo *>::iterator _clientInfoIt;
        std::list<ClientInfo *> _clientsList;
        void writeInTempFile();
        void receiveFromClient();
        void moveFileToUploads();
    public :
        PostMethod(ServerConfiguration &, std::list<ClientInfo *>::iterator &, std::list<ClientInfo *> &);
        PostMethod();
        ~PostMethod();
        void preparingPostRequest();
        void isValidPostRequest();
        void serveClient();
        void  successfulPostRequest();
};

#endif