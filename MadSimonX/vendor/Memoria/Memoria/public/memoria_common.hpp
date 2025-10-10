#pragma once

#define MEMORIA_BEGIN \
	namespace Memoria {

#define MEMORIA_END \
	}

#if defined(_WIN64) || defined(__x86_64__) || defined(__ppc64__) || defined(__aarch64__)

#define MEMORIA_64BIT 1
#undef  MEMORIA_32BIT

#define IsX64() true
#define IsX32() false

#else

#define MEMORIA_32BIT 1
#undef MEMORIA_64BIT

#define IsX64() false
#define IsX32() true

#endif

MEMORIA_BEGIN

extern bool Startup();
extern bool Cleanup();

//
// An alternative for scenarios where functionality similar to std::atexit is required.
//
// NOTE: For this to work, the programmer must manually call Memoria::Cleanup
// at the termination of the application.
//

extern void RegisterOnExitCallback(void(*cb)());

MEMORIA_END