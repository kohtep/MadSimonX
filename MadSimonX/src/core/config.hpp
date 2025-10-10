#pragma once

#ifdef MEMORIA_LOG_PREFIX
#undef MEMORIA_LOG_PREFIX
#endif

#define MEMORIA_LOG_PREFIX FORE_LIGHTBLACK "[%#d / %#t] " FORE_RESET

#ifndef _DEBUG
#define MEMORIA_LOG_WRAP(str) STRING(str)
#endif