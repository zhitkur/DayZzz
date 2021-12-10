#pragma once
#include <Windows.h>
#include "globals.h"
#include "offsets.h"

uintptr_t FindProcessID(const std::string& ProcessName)
{
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(pe32);

	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
	if (hSnap == INVALID_HANDLE_VALUE)
		return 0;

	Process32First(hSnap, &pe32);
	if (!ProcessName.compare(pe32.szExeFile))
	{
		CloseHandle(hSnap);
		return pe32.th32ProcessID;
	}

	while (Process32Next(hSnap, &pe32))
	{
		if (!ProcessName.compare(pe32.szExeFile))
		{
			CloseHandle(hSnap);
			return pe32.th32ProcessID;
		}
	}

	CloseHandle(hSnap);
	return 0;
}
uintptr_t GetModule(const std::string& ModuleName)
{
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, globals.process_id);
	if (hSnap == INVALID_HANDLE_VALUE) return 0;

	MODULEENTRY32 me32{ 0 };
	me32.dwSize = sizeof(me32);

	if (Module32First(hSnap, &me32))
	{
		do
		{
			if (strcmp(ModuleName.c_str(), me32.szModule) == 0)
			{
				CloseHandle(hSnap);
				return (uintptr_t)me32.modBaseAddr;
			}
		} while (Module32Next(hSnap, &me32));
	}
	CloseHandle(hSnap);
	return 0;
}

static __forceinline bool ZwCopyMemory(uint64_t address, PVOID buffer, uint64_t size, BOOL write = false)
{
	if (globals.handle == NULL)
		return false;

	SIZE_T bytes = 0;

	if (!write)
	{
		if (!ReadProcessMemory(globals.handle, (LPVOID)address, buffer, size, (SIZE_T*)&bytes))
			return false;

		if (bytes != size)
			return false;
	}
	else
	{
		if (!WriteProcessMemory(globals.handle, (LPVOID)address, buffer, size, (SIZE_T*)&bytes))
			return false;

		if (bytes != size)
			return false;
	}

	return true;
};

template <typename T>
static T ReadData(uint64_t address)
{
	if (!address)
		return T();

	T buffer;
	return ZwCopyMemory(address, &buffer, sizeof(T), false) ? buffer : T();
};

template <typename T>
static void WriteData(uint64_t address, T data)
{
	if (!address)
		return;

	ZwCopyMemory(address, &data, sizeof(T), true);
};

static std::string ReadString(uint64_t address, size_t size)
{
	if (!address || size > 1024)
		return "";

	char string[1024] = "";
	return ZwCopyMemory(address, string, size, false) ? std::string(string) : "";
};

std::string ReadArmaString(uint64_t address)
{
	int length = ReadData<int>(address + OFF_LENGTH);

	std::string text = ReadString(address + OFF_TEXT, length);

	return text.c_str();
}
