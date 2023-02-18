NAME=webserv
CC=c++
CPPFLAGS= -Wall -Wextra -Werror -std=c++98
FS=-fsanitize=address
RM=rm -rf

HEADERS = Interfaces/ConfigFileParser.hpp \
		  Interfaces/MultiHttpServer.hpp \

SOURCES = _main.cpp \
		Implementations/ConfigFileParser.cpp \
		Implementations/LocationBlockParse.cpp \
		Implementations/ServerConfiguration.cpp \
		Implementations/MultiHttpServer.cpp \
		Implementations/HttpServer.cpp \
		Implementations/Client.cpp \
		Implementations/RequestParser.cpp \
		parsing/parsingUtils.cpp \

OBJECTS=$(SOURCES:.cpp=.o)

all : $(NAME)

$(NAME) : $(OBJECTS)
	$(CC) $(CPPFLAGS) $(FS) $(OBJECTS) -o $(NAME)

$(OBJECTS) : $(HEADERS)

clean:
	$(RM) $(OBJECTS)

fclean: clean
	$(RM) $(NAME)

re : fclean all