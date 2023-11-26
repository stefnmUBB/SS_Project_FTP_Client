#pragma once

#include "TCP.h"
#include "TelNetClient.h"
#include <functional>

class FTPClient
{
public:
	static constexpr int MAX_LINE_BUFF_SIZE = 2048;
private:
	bool connected = false;
	TelNetClient* telnet_client;
	TCP data_port;
	std::function<void(const char*)> print_line;
	void line_received_callback(const char*);
	int send_command_wrapper(const char*);	
	char line_buffer[MAX_LINE_BUFF_SIZE];

public:
	FTPClient(const char* ip, int port = 21, std::function<void(const char*)> print_line = [](const char*) {});

	void login(const char* user, const char* pass);
	void logout();
	void list(const char* path);
	void pasv();



	~FTPClient();	
};