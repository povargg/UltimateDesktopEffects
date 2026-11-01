#pragma once
#include <cstdint>
using DWORD = uint32_t;
using HWND = void*;
using HINSTANCE = void*;
struct POINT { int32_t x, y; };
inline uint32_t GetTickCount() { return 0x12345678u; }
