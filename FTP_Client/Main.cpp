#include <iostream>
#include <exception>

#include "FTPClient.h"
#include "FTPCommandInterpreter.h"

#include <string>
#include "tcp_exception.h"
#include "utils.h"

using namespace std;


void run_client(const char* ip, int port)
{    
    FTPClient ftp_client(ip, port, printf);

    FTPCommandInterpreter ci(&ftp_client);        

    std::string cmd;
    while (1)
    {
        std::cout << ">> ";        
        std::getline(cin, cmd);

        try
        {
            ci.execute(cmd.c_str());
        }
        catch (exception& e)
        {
            cout << Utils::Color::Red() << e.what() << Utils::Color::White() << "\n";
        }
    }

    return;
}

int main(int argc, const char** argv)
{    
    try
    {        
        //run_client(argv[1], argc == 3 ? atoi(argv[2]) : 21);
        run_client("127.0.0.1", 21);
    }
    catch (exception& e)
    {
        cout << e.what() << "\n";
    }   
}
