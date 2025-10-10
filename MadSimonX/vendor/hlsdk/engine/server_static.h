#pragma once

#include "basetypes.h"
#include "netadr.h"

struct server_log_t
{
	qboolean active;
	qboolean net_log_;
	netadr_t net_address_;
	void *file;
};

struct server_stats_t
{
	int num_samples;
	int at_capacity;
	int at_empty;
	float capacity_percent;
	float empty_percent;
	int minusers;
	int maxusers;
	float cumulative_occupancy;
	float occupancy;
	int num_sessions;
	float cumulative_sessiontime;
	float average_session_len;
	float cumulative_latency;
	float average_latency;
};

struct server_static_t
{
	qboolean dll_initialized;
	struct client_s *clients;
	int maxclients;
	int maxclientslimit;
	int spawncount;
	int serverflags;
	server_log_t log;
	double next_cleartime;
	double next_sampletime;
	server_stats_t stats;
	qboolean isSecure;
};
