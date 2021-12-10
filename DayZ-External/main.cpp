#include "overlay.h"

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	switch (uMsg)
	{
	case WM_DESTROY:
	{
		ov::clean_directx();
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}


int main()
{
	init();
	ov::create_window();

	ov::loop();
}