#include "../Interfaces/configFileParse.hpp"

void errorPrinting(const char *errorMessage){
    std::cout << errorMessage << std::endl;
    exit (EXIT_FAILURE);
}

bool isValidNumber(std::string &data){
    for (size_t i = 0; i < data.length(); i++)
        if (!isnumber(data[i]))
            return (false);
    return (true);
}
