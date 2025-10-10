//
// memoria_utils_assert.hpp
// 
// Custom implementation of assert to eliminate dependency on the CRT.
// 
// The `Assert` macro: its behavior is similar to the original — just a boolean condition
// in the arguments, disabled in release configuration.
// 
// Example:
//   Assert(B != FALSE);
// 
// The `AssertMsg` macro: same as `Assert`, but allows injecting a custom message
// into the assert window with C-style formatting support.
// 
// Example:
//   Assert(B != FALSE, "Some formatted message (%d)", 1);
// 
// The `AssertIf` macro: displays an assert window if the condition fails, but also
// behaves like a function — returns the result of the condition. This allows it to be used in
// `if()` blocks. Returning the condition result is preserved in the release configuration.
// 
// Example:
//   if (AssertIf(B != FALSE)) { /* Something happens. */ }
// 
// The `AssertMsgIf` macro: same as `AssertIf`, but allows injecting a custom message
// into the assert window with C-style formatting support.
// 
// Example:
//   if (AssertMsgIf(B != FALSE, "Some formatted message (%d)", 1)) { /* Something happens. */ }
//

#pragma once

#ifdef _DEBUG

extern bool AssertImpl(bool exprResult, const char *exprStr, const char *file, int line, const char *fmt, ...);

#define Assert(expr) (void)(AssertImpl((expr), #expr, __FILE__, __LINE__, nullptr))
#define AssertMsg(expr, fmt, ...) (void)(AssertImpl((expr), #expr, __FILE__, __LINE__, fmt, __VA_ARGS__))

#define AssertIf(expr) AssertImpl((expr), #expr, __FILE__, __LINE__, nullptr)
#define AssertMsgIf(expr, fmt, ...) AssertImpl((expr), #expr, __FILE__, __LINE__, fmt, __VA_ARGS__)

#else

#define Assert(expr) ((void)0)
#define AssertMsg(expr, fmt, ...) ((void)0)

#define AssertIf(expr) (expr)
#define AssertMsgIf(expr, fmt, ...) (expr)

#endif