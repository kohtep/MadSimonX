#include "memoria_core_errors.hpp"

MEMORIA_BEGIN

int memoria_last_error = 0;
OnErrorEvent_t memoria_error_callback = nullptr;

void SetError(int value)
{
	memoria_last_error = value;

	if (memoria_error_callback)
		memoria_error_callback(value);
}

void ResetError()
{
	SetError(ME_NO_ERROR);
}

int GetError()
{
	return memoria_last_error;
}

void SetErrorCallback(OnErrorEvent_t value)
{
	memoria_error_callback = value;
}

OnErrorEvent_t GetErrorCallback()
{
	return memoria_error_callback;
}

MEMORIA_END