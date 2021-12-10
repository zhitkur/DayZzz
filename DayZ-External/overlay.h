#pragma once
#include <Windows.h>
#include <string>

#include <d3d9.h>
#include <d3dx9.h>
#include <dwmapi.h>

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dwmapi.lib")

#include "drawings.h"
#include "globals.h"

extern IDirect3D9Ex* p_Object;
extern IDirect3DDevice9Ex* p_Device;
extern D3DPRESENT_PARAMETERS p_Params;

namespace ov
{
	void create_window(bool console = false);
	void setup_directx(HWND hWnd);
	void render();
	void update_overlay();
	WPARAM loop();

	void clean_directx();
}

void draw_esp();
void init();
void update_list();