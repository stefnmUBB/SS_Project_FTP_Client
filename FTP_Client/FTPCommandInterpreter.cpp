#include "FTPCommandInterpreter.h"

#include <functional>

#define LAMBDA(ci, ftp, fname) ((std::function<void(const Parameter*)>)std::bind(fname, ci, ftp, std::placeholders::_1))


void cmd_login(CommandInterpreter* ci, FTPClient* ftp, const Parameter* pms)
{
	const char* user = pms[0].get_value_str();
	const char* pass = pms[1].get_value_str();
	ftp->login(user, pass);
}

void cmd_logout(CommandInterpreter* ci, FTPClient* ftp, const Parameter* pms)
{
	ftp->logout();
}

void cmd_help(CommandInterpreter* ci, FTPClient* ftp, const Parameter* pms)
{
	ci->print_commands(std::cout);
}

void cmd_list1(CommandInterpreter* ci, FTPClient* ftp, const Parameter* pms)
{
	ftp->pasv();
	const char* path = pms[0].get_value_str();
	ftp->list(path);
}

void cmd_list0(CommandInterpreter* ci, FTPClient* ftp, const Parameter* pms)
{	
	ftp->pasv();
	ftp->list(nullptr);
}

void cmd_pasv(CommandInterpreter* ci, FTPClient* ftp, const Parameter* pms)
{
	ftp->pasv();
}

FTPCommandInterpreter::FTPCommandInterpreter(FTPClient* ftp) : ftp{ ftp }
{
   register_command(LAMBDA(this, ftp, cmd_login), "login", Param(0, "user", ParameterType::STRING), Param(1, "pass", ParameterType::STRING));
   register_command(LAMBDA(this, ftp, cmd_help), "help");
   register_command(LAMBDA(this, ftp, cmd_logout), "logout");
   register_command(LAMBDA(this, ftp, cmd_list1), "list", Param(0, "path", ParameterType::STRING));
   register_command(LAMBDA(this, ftp, cmd_list0), "list");
   register_command(LAMBDA(this, ftp, cmd_pasv), "pasv");

}



