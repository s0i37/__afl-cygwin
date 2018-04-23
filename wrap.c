#include "Windows.h"
#include <stdio.h>

#pragma comment(lib, "ws2_32.lib")

HANDLE pipe;

int make_crach()
{	
	return *(int *)0x41414141;
}

void sync_entry()
{
	pipe = CreateNamedPipe(
        "\\\\.\\pipe\\afl_sync",                            // pipe name
        PIPE_ACCESS_DUPLEX,                                 // read/write access
        PIPE_TYPE_MESSAGE|PIPE_READMODE_MESSAGE|PIPE_WAIT,  // no wait incoming pipe connections
        PIPE_UNLIMITED_INSTANCES,                           // max. instances
        512,                                                // output buffer size
        512,                                                // input buffer size
        0,                                                  // client time-out
        0);
}

void sync_exit()
{
	char result[1];
	DWORD num_read;

    if(pipe == INVALID_HANDLE_VALUE)
        return;

    if(! ConnectNamedPipe(pipe, NULL) ) 		/* WAIT */
		return;

	ReadFile(pipe, &result, 1, &num_read, NULL);
	printf("%c\n", result[0]);

	if( result[0] == 'e' )
		return;
	else if( result[0] == 'c' )
		make_crach();
}

void main(int argc, char **argv)
{
	FILE *f;
	int port;
	char *input_file;
	char input[1024];
	char outbuf[16];
	WSADATA wsaData;
	int s;
	struct sockaddr_in target;

	if(argc < 3)
		return;
	
	port = atoi( argv[1] );
	input_file = argv[2];

	f = fopen( argv[2], "rb" );
	if(!f)
		return;
	fread(input, 1024, 1, f);
	fclose(f);
	

	WSAStartup( MAKEWORD(2,2), &wsaData );
	s = socket(AF_INET, SOCK_STREAM, 0);
	target.sin_family = AF_INET;
	target.sin_addr.s_addr = inet_addr("127.0.0.1");
	target.sin_port = htons(port);
	connect( s, (struct sockaddr *)&target, sizeof(target) );

	send(s, "qtate0", 6, 0);
	recv(s, outbuf, sizeof(outbuf), 0);
	send(s, "wtate1", 6, 0);
	recv(s, outbuf, sizeof(outbuf), 0);
	send(s, "etate2", 6, 0);
	recv(s, outbuf, sizeof(outbuf), 0);

	sync_entry();

	send(s, input, sizeof(input), 0);
	closesocket(s);
	
	sync_exit();

	WSACleanup();
	DisconnectNamedPipe(pipe);
	CloseHandle(pipe);
}