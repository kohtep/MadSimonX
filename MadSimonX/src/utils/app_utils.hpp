#pragma once

// Utility
namespace U::App
{
	extern bool HasCommandLineParam(const char *param);
	extern bool HasDebugParam();

	extern bool AppendWindowTag(const char *tag);
}