#pragma once

#include "memoria_common.hpp"

MEMORIA_BEGIN

extern void MsgAlert(const char *fmt, ...);
extern void MsgError(const char *fmt, ...);
extern void MsgInfo(const char *fmt, ...);

MEMORIA_END