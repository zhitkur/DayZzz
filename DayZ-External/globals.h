#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <cstdint>

struct _globals
{
	uintptr_t process_id;
	HANDLE handle;

	HWND OverlayWnd;
	HWND TargetWnd;
	int Width, Height;
	bool console;
	const char lTargetWindow[256] = "DayZ";
	const char lWindowName[256] = "Element";

	uint64_t Base;
	uint64_t World;
};

extern LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef struct _player_t
{
	std::uint64_t EntityPtr;
	std::uint64_t TableEntry;
	int NetworkID;
} player_t;

typedef struct _item_t
{
	std::uint64_t ItemPtr;
	std::uint64_t ItemTable;
} item_t;

extern _globals globals;