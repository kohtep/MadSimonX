# Memoria

[![License](https://img.shields.io/badge/License-BSD%202--Clause-orange.svg)](https://opensource.org/licenses/BSD-2-Clause)

Memory Management Library for x86.

## What is it?

Memoria is a personal memory manipulation library that the author uses in their own projects.

This project has undergone multiple redesigns, with its earliest versions originally written in Delphi. It is now being published publicly for convenient integration into other projects that require memory operations.

> [!WARNING]
> This project is not considered complete. Many features may not work and may contain bugs. The project still requires further testing and has potential for future expansion. Overall, it is intended for use by the author only, but if you wish to use it in your own projects, please proceed with caution.

## What does this project offer?

<ul>
<li>VMT and regular function hooking</li>
<li>Signature scanning</li>
<li>Memory check/read/write</li>
<li>State-managed patching</li>
<li>Memory/DLL/EXE fragment management</li>
<li>RTTI scanner</li>
<li>Windows-specific utilities</li>
</ul>

## Examples

> Demonstration of signaturing and locating a code fragment pointer in the current EXE
```cpp
#include "memoria_amalgamation.hpp"

static std::unique_ptr<Memoria::CMemoryModule> some_module;
static uint64_t *some_data;

int main()
{
	/* Retrieving information about the .EXE file */
	some_module = Memoria::CMemoryModule::CreateFromAddress(nullptr);

	/* Searching for SomeData inside the .EXE */
	some_module->Sig("48 8B 05 ? ? ? ? 48 89 34", [](Memoria::CSigHandle &sig, void *lpParam) -> void
		{
			some_data = sig.Add(3).Rip().As<uint64_t *>();
		}, nullptr);

	return 0;
}
```

> Demonstration of `F1` function hooking
```cpp
#include "memoria_amalgamation.hpp"

using Fn_t = void(*)();
static Fn_t Fn;

static void F1()
{
	MessageBoxA(HWND_DESKTOP, "F1", "", MB_SYSTEMMODAL);
}

static void F2()
{
	MessageBoxA(HWND_DESKTOP, "F2", "", MB_SYSTEMMODAL);
	Fn();
}

int main()
{
	/* Hook the function `F1` by inserting a jump to `F2` at its beginning,
	   and write a trampoline (to call the original) into `Fn`. */
	Memoria::Hook(F1, F2, &Fn);

	F1();

	return 0;
}
```

> Demonstration of controlled patching of a double constant
```cpp
#include "memoria_amalgamation.hpp"

#include <iostream>

static double some_double_value = 4.0;

int main()
{
	Memoria::CPatch *patch;
	patch = Memoria::PatchDouble(&some_double_value, 8.0, false);

	std::cout << "Original value: " << some_double_value << "\n";
	patch->Apply();
	std::cout << "Patched value : " << some_double_value << "\n";
	patch->Restore();
	std::cout << "Restored value: " << some_double_value << "\n";

	/* `Memoria::FreeAllPatches()` is also acceptable */
	Memoria::FreePatch(patch);

	return 0;
}
```

> Demonstration of quick deployment of logging to the system console
```cpp
#include "memoria_ext_logger.hpp"
#include "memoria_utils_assert.hpp"

int main()
{
	if (!AssertMsgIf(Memoria::InitializeConsoleLogging(), "Failed to initialize console logging."))
	{
		return EXIT_FAILURE;
	}

	LOG("Application started (PID: %d)", GetCurrentProcessId());

	LOG_DBG("Debug logging is enabled.");
	LOG_DBG("Performing internal diagnostics: value = %d", 42);
	LOG_DBG("Shutting down subsystems...");

	Memoria::FinalizeConsoleLogging();

	return EXIT_SUCCESS;
}
```

## License

This product is distributed under the 2-Clause BSD License.

## Third-party components

This project includes the Hacker Disassembler Engine (HDE) by Vyacheslav Patkov, licensed under the BSD 2-Clause License.  
See `vendor/hde/LICENSE` for details.
