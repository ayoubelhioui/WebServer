NAME=webserv
CC=c++
CPPFLAGS= -Wall -Wextra -Werror -std=c++98
FS=-fsanitize=address -g
RM=rm -rf

HEADERS = Interfaces/ConfigFileParser.hpp \
		  Interfaces/MultiHttpServer.hpp \

SOURCES = main.cpp \
		Implementations/ConfigFileParser.cpp \
		Implementations/LocationBlockParse.cpp \
		Implementations/GETMethod.cpp \
		Implementations/ServerConfiguration.cpp \
		Implementations/MultiHttpServer.cpp \
		Implementations/Client.cpp \
		Implementations/HttpServer.cpp \
		Implementations/RequestParser.cpp \
		parsing/parsingUtils.cpp \
		errorsHandling/errorsHandling.cpp \
		getFormat.cpp \

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