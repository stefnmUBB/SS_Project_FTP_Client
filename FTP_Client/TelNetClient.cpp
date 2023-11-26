#include "TelNetClient.h"
#include <exception>

TelNetClient::TelNetClient(const char* ip, int port, std::function<void(char*)> line_received_callback) : line_received_callback { line_received_callback },
	ip{ip}, port{port}
{
	try
	{
		tcp.connect(ip, port);		
		tcp.set_timeout(3);

		printf("Client: %s:%i\n", tcp.get_ip(), tcp.get_port());

		recv_response(); // Server greeting		
	}
	catch (std::exception& e)
	{
		throw e;
	}
}

void TelNetClient::reconnect()
{	
	printf("Reconnecting...\n");
	tcp.close();
	tcp.connect(ip, port);
	recv_response(); // Server greeting		
}

int TelNetClient::send_command(const char* command)
{
	tcp.send(command, strlen(command));
	tcp.send("\r\n", 2);
	return recv_response();
}

namespace
{
	void read_line(TCP& tcp, char* buffer)
	{
		char* c = buffer;
		for (; (*c = tcp.recv_i8()) != 0x0A; c++);
		*(++c) = '\0';
	}
}

namespace
{
	int response_code_to_int(const char* code)
	{
		return (code[0] - '0') * 100 + (code[1] - '0') * 10 + (code[2] - '0');
	}
}

int TelNetClient::recv_response()
{	
	char first_line[2048] = { 0 };
	char buffer[2048] = { 0 };
	
	read_line(tcp, first_line);
	line_received_callback(first_line);

	if (first_line[3] == ' ') return response_code_to_int(first_line);

	while (buffer[0] != first_line[0] || buffer[1] != first_line[1] || buffer[2] != first_line[2] || buffer[3] != ' ')
	{
		read_line(tcp, buffer);
		line_received_callback(buffer);
	}

	return response_code_to_int(first_line);
}