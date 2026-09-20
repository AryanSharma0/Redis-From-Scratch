#ifndef COMMAND_HANDLER
#define COMMAND_HANDLER
#include <string>
class CommandHandler
{
public:
    CommandHandler();
    std::string processComand(std::string &commandline);
};

#endif COMMAND_HANDLER