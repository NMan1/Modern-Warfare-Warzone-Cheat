#pragma once
#include <string>
#include <iostream>
#include <windows.h> 
#include <vector>
#include "Vector.h"

enum Color { Red = 12, Green = 10 };

#define INRANGE(x,a,b)   (x >= a && x <= b)
#define GET_BYTE( x )    (GET_BITS(x[0]) << 4 | GET_BITS(x[1]))
#define GET_BITS( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xa) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define PTRMAXVAL ((PVOID)0x000F000000000000)

namespace Utils
{
	static void Log(std::string str, Color clr)
	{
		HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
		SetConsoleTextAttribute(hConsole, clr);
		std::cout << "[Overflow] " << str << std::endl;
	}

	static void Log(std::string str)
	{
		std::cout << "[Overflow] " << str << std::endl;
	}

	static void Log(Vector vec)
	{
		std::cout << "[Overflow] X: " << vec.x << " Y: " << vec.y << " Z: " << vec.z << std::endl;
	}

	template <typename T>
	static void Log(const std::string str, T Addy, bool Hex = true)
	{
		if (Hex)
			std::cout << "[Overflow] " << str << std::hex << Addy << std::endl;
		else
			std::cout << "[Overflow] " << str << Addy << std::endl;
	}

	template<class T>
	T Read(const DWORD64 dwPtr)
	{
		if (!IsBadReadPtr((void*)dwPtr, sizeof(T)))
			return *(T*)dwPtr;
		return 0;
	}
	static __forceinline BOOLEAN IsValid(PVOID ptr)
	{
		return (ptr >= (PVOID)0x10000) && (ptr < PTRMAXVAL) && ptr != nullptr && !IsBadReadPtr(ptr, sizeof(ptr));
	}
	template <class vType>
	static BOOLEAN Write(uintptr_t address, vType value)
	{
		if (IsValid((vType*)(address)))
		{
			*(vType*)(address) = value;
			return TRUE;
		}
		return FALSE;
	}

	template <typename vType>
	static vType ReadPtr(std::initializer_list<uintptr_t> _Offsets, bool ReadFirstOffset)
	{
		uintptr_t LastPtr = NULL;
		int OffsetsSize = NULL;
		std::vector<uintptr_t> Offsets = { NULL };
		Offsets = _Offsets;
		OffsetsSize = Offsets.size();
		LastPtr = Read<uintptr_t>((ReadFirstOffset ? Read<uintptr_t>(Offsets[0]) : Offsets[0]) + Offsets[1]);
		for (size_t i = 2; i < OffsetsSize - 1; i++)
			if (!(LastPtr = Read<uintptr_t>(LastPtr + Offsets[i])))
				return vType();
		return Read<vType>(LastPtr + Offsets[OffsetsSize - 1]);
	}

	template <typename vType>
	static BOOLEAN WritePtr(std::initializer_list<uintptr_t> _Offsets, vType _value, bool ReadFirstOffset)
	{
		uintptr_t LastPtr = NULL;
		int OffsetsSize = NULL;
		std::vector<uintptr_t> Offsets = { NULL };
		Offsets = _Offsets;
		OffsetsSize = Offsets.size();
		LastPtr = Read<uintptr_t>((ReadFirstOffset ? Read<uintptr_t>(Offsets[0]) : Offsets[0]) + Offsets[1]);
		for (size_t i = 2; i < OffsetsSize - 1; i++)
			if (!(LastPtr = Read<uintptr_t>(LastPtr + Offsets[i])))
				return FALSE;
		return Write<vType>(LastPtr + Offsets[OffsetsSize - 1], _value);
	}

	static uint8_t* FindSignature(uintptr_t Module, const char* szSignature)
	{
		static auto pattern_to_byte = [](const char* pattern) {
			auto bytes = std::vector < int >{};
			auto start = const_cast<char*>(pattern);
			auto end = const_cast<char*>(pattern) + strlen(pattern);

			for (auto current = start; current < end; ++current) {
				if (*current == '?') {
					++current;
					if (*current == '?')
						++current;
					bytes.push_back(-1);
				}
				else {
					bytes.push_back(strtoul(current, &current, 16));
				}
			}
			return bytes;
		};

		auto dosHeader = (PIMAGE_DOS_HEADER)Module;
		auto ntHeaders = (PIMAGE_NT_HEADERS)((uint8_t*)Module + dosHeader->e_lfanew);

		auto sizeOfImage = ntHeaders->OptionalHeader.SizeOfImage;
		auto patternBytes = pattern_to_byte(szSignature);
		auto scanBytes = reinterpret_cast<uint8_t*>(Module);

		auto s = patternBytes.size();
		auto d = patternBytes.data();

		for (auto i = 0ul; i < sizeOfImage - s; ++i) {
			bool found = true;
			for (auto j = 0ul; j < s; ++j) {
				if (scanBytes[i + j] != d[j] && d[j] != -1) {
					found = false;
					break;
				}
			}
			if (found) {
				return &scanBytes[i];
			}
		}
		return nullptr;
	}
}