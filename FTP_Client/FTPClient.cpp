#include "FTPClient.h"
#include <iostream>
#include "utils.h"

FTPClient::FTPClient(const char* ip, int port, std::function<void(const char*)> print_line)
{
	this->print_line = print_line;
	std::function<void(const char*)> line_rec_cb = std::bind(&FTPClient::line_received_callback, this, std::placeholders::_1);
	telnet_client = new TelNetClient(ip, port, line_rec_cb);
	connected = true;
	filesystem = new VirtualFS("vfs_root");
}

void FTPClient::line_received_callback(const char* line)
{	
	strncpy_s(line_buffer, line, sizeof(line_buffer));

	std::cout << Utils::Color::Yellow();
	print_line(line);
	std::cout << Utils::Color::White();
}

int FTPClient::send_command_wrapper(const char* cmd)
{
	std::cout << Utils::Color::Blue() << cmd << Utils::Color::White() << "\n";
	return telnet_client->send_command(cmd);
}

void FTPClient::login(const char* user, const char* pass)
{	
	if (!connected)
	{
		telnet_client->reconnect();
		connected = true;
	}
	if (send_command_wrapper(bufferf("USER %s", user)) != 331)
	{
		throw std::exception("login failed");				
	}	
	if (send_command_wrapper(bufferf("PASS %s", pass)) != 230)
	{
		throw std::exception("login failed");
	}	
}

void FTPClient::logout()
{	
	if (send_command_wrapper("QUIT") != 221)
	{
		throw std::exception("logout failed");
	}
	connected = false;
}

namespace
{
	// returns the address of the first occurence of c in buff, otherwise exception
	const char* my_strnchr(const char* buff, int len, char c)
	{
		for (int i = 0; i < len && *buff && *buff != c; i++, buff++);		
		if (*buff == c) return buff;
		throw std::exception(bufferf("Failed to find character: '%c'", c));
	}

	//	"150 Opening ASCII mode data connection for ... (163 bytes)
	int get_data_size(const char* input)
	{
		const char* nr_start = my_strnchr(input, FTPClient::MAX_LINE_BUFF_SIZE - 1, '(') + 1;
		long long nr = 0;
		const char* nr_max_end = input + FTPClient::MAX_LINE_BUFF_SIZE - 1;
		
		const char* it = nr_start;
		int digits_cnt = 0;
		while ('0' <= *it && *it <= '9' && it != nr_max_end && digits_cnt < 10)
		{
			nr = nr * 10 + *it - '0';
			it++;
			digits_cnt++;
		}

		if (digits_cnt == 0)
			throw std::exception("No number found");
		if (nr<INT_MIN || nr>INT_MAX)
			throw std::exception("Number out of range");
		return nr;
	}
}


void FTPClient::list(const char* path)
{
	int resp = path == nullptr ? send_command_wrapper("LIST") : send_command_wrapper(bufferf("LIST %s", path));		
	if (resp != 150)
		throw std::exception("Failed");

	int data_size = get_data_size(line_buffer);

	printf("Data size = %i\n", data_size);

	char* buffer = new char[data_size + 1];
	data_port.recv(buffer, data_size);
	buffer[data_size] = 0;

	printf(buffer);

	if (telnet_client->recv_response() != 226)
		throw std::exception("Failed transfer");

}

void FTPClient::stor(const char* path)
{
	if(send_command_wrapper(bufferf("STOR %s", path))!=150)
		throw std::exception("Failed");

	std::vector<char> buffer = filesystem->read(path);
	data_port.send(buffer.data(), buffer.size());
	data_port.close();

	if (telnet_client->recv_response() != 226)
		throw std::exception("Failed transfer");
}

void FTPClient::retr(const char* path)
{
	if (send_command_wrapper(bufferf("RETR %s", path)) != 150)
		throw std::exception("Failed");

	int data_size = get_data_size(line_buffer);
	printf("Data size = %i\n", data_size);
	
	std::vector<char> buffer(data_size);	
	data_port.recv(buffer.data(), buffer.size());	
	filesystem->write(path, buffer);

	if (telnet_client->recv_response() != 226)
		throw std::exception("Failed transfer");
}


void FTPClient::pasv()
{
	if (send_command_wrapper("PASV") != 227)
		throw std::exception("Entering passive mode failed");	
	const char* line_pfx = "227 Entering Passive Mode (";
	if (strncmp(line_pfx, line_buffer, strlen(line_pfx)) != 0)
		throw std::exception("Invalid passive response message");
	char* buff = line_buffer + strlen(line_pfx);	

	// buff = "192,168,56,1,244,12)" --> int[] {192, 168, 56, 1, 244, 12};

	int maxStrLen = 4 * 6;

	int a[6] = {}, i = 0;

	int k = 0;
	for (; buff[k] != ')' && k < maxStrLen; k++)
	{
		if ('0' <= buff[k] && buff[k] <= '9')
		{
			a[i] = a[i] * 10 + (buff[k] - '0');
			continue;
		}
		if (buff[k] == ',')
		{
			if (i >= 6)
				throw std::exception("Failed to parse PASV address: too many numbers");
			i++;
			continue;
		}
		throw std::exception(bufferf("Failed to parse PASV address: invalid character '\\x%02X'", buff[i]));
	}

	if (buff[k] != ')')
		throw std::exception("Failed to parse PASV address: input too long");
	else
	{
		if (i >= 6)
			throw std::exception("Failed to parse PASV address: too many numbers");
		i++;		
	}

	if (i < 6)
		throw std::exception("Failed to parse PASV address: insufficient numbers");

	bufferf ip("%i.%i.%i.%i", a[0], a[1], a[2], a[3]);
	int port = a[4] * 256 + a[5];

	data_port.connect(ip, port);
	printf("Opened data port on %s:%i.\n", (const char*)ip, port);
}

FTPClient::~FTPClient()
{
	delete telnet_client;
	delete filesystem;
}