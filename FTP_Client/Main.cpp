#include <iostream>
#include <exception>

#include "FTPClient.h"
#include "FTPCommandInterpreter.h"

#include <string>
#include "tcp_exception.h"
#include "utils.h"

using namespace std;


void run_client()
{
    FTPClient ftp_client("192.168.56.1", 21, printf);

    FTPCommandInterpreter ci(&ftp_client);        

    while (1)
    {
        std::cout << ">> ";
        std::string cmd;
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

int main()
{    
    try
    {        
        run_client();
    }
    catch (exception& e)
    {
        cout << e.what() << "\n";
    }   
}
