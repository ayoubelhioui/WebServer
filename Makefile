NAME=webserv
CC=c++
CPPFLAGS= -Wall -Wextra -Werror -std=c++98
# FS= -fsanitize=address -g3
RM=rm -rf

HEADERS = Interfaces/ConfigFileParser.hpp \
		  Interfaces/MultiHttpServer.hpp \

SOURCES = main.cpp \
		Implementations/ConfigFileParser.cpp \
		Implementations/LocationBlockParse.cpp \
		Implementations/GETMethod.cpp \
		Implementations/POSTMethod.cpp \
		Implementations/ServerConfiguration.cpp \
		Implementations/MultiHttpServer.cpp \
		Implementations/Client.cpp \
		Implementations/ChunkedPostRequest.cpp \
		Implementations/HttpServer.cpp \
		Implementations/RequestParser.cpp \
		Implementations/DeleteMethod.cpp \
		errorsHandling/errorsHandling.cpp \
		Utils/stringManip.cpp \
		getFormat.cpp \

DIR_LISTING_FILE = directoryListing.html
OBJECTS=$(SOURCES:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CC) $(CPPFLAGS) $(FS) $(OBJECTS) -o $(NAME)

$(OBJECTS) : $(HEADERS)

leaks:
	./leaks.sh

clean:
	$(RM) $(OBJECTS)
	$(RM) $(DIR_LISTING_FILE)

fclean: clean
	$(RM) $(NAME)

re : fclean all