#pragma once

typedef int qboolean;
typedef unsigned int string_t;
typedef int EOFFSET;
typedef int BOOL;

using xcommand_t = void(*)();

struct cmd_function_t
{
	cmd_function_t* next;
	char* name;
	xcommand_t callback;
	int flags;
};

enum cactive_t
{
	ca_dedicated = 0,
	ca_disconnected,
	ca_connecting,
	ca_connected,
	ca_uninitialized,
	ca_active
};