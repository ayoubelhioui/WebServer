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



class ClientInfo;
class PostMethod{
    private :
        void _searchingForUploadFolder( ClientInfo* );
        void _preparingPostRequest( ClientInfo* );
        void _isValidPostRequest( ClientInfo* );
        void _writeInTempFile( ClientInfo* );
        void _receiveFromClient( ClientInfo* );
        void _isLocationExist(ClientInfo *);
        ServerConfiguration _serverConfiguration;
        std::list<LocationBlockParse>::iterator _currentLocation;
    public :
        std::ifstream sourceFile;
        std::ofstream destinationFile;
        int totalTempFileSize;
        int toWrite;
        bool    isErrorOccured;
        PostMethod(ServerConfiguration &);
        void writeToUploadedFile( void );
        void receiveTheBody( ClientInfo* );
        void successfulPostRequest( ClientInfo* );
        void preparingMovingTempFile( ClientInfo* );
        void handleFirstRead( ClientInfo* );
};

#endif