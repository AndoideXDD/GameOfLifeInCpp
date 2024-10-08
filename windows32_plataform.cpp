#include <stdio.h>

#include "game.cpp"


global_variable Render_State render_state;
global_variable bool running = true;


LRESULT CALLBACK  window_callback(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam) {
	LRESULT result = 0;
	switch (uMsg){
		case WM_CLOSE:
		case WM_DESTROY: {
			running = false;
		} break;

		case WM_SIZE: {
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			int size = render_state.width * render_state.height * sizeof(u32);
			if (render_state.memory) VirtualFree(render_state.memory, 0, MEM_RELEASE);
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;
		} break;
		default: {
			result = DefWindowProc(hwnd, uMsg, wParam, lParam);
		}
	}
	
	return result;
}

internal void 
procesButtons (const MSG& mesage, Input& input) {
	switch (mesage.message)
	{
	case WM_KEYUP:
	case WM_KEYDOWN: {
		u32 vk_code = (u32)mesage.wParam;
		bool is_down = ((mesage.lParam & (1 << 31)) == 0);
		switch (vk_code) {
			#define process_button(b, vk)\
				case vk: { \
				input.buttons[b].changed = is_down != input.buttons[b].is_down;\
				input.buttons[b].is_down = is_down;\
				} break;
			process_button(BUTTON_F, 'F');
			process_button(BUTTON_W, 'W');
			process_button(BUTTON_S, 'S');
			process_button(BUTTON_D, 'D');
			process_button(BUTTON_A, 'A');
			process_button(BUTTON_R, 'R');
			process_button(BUTTON_ENTER, VK_RETURN);
			process_button(BUTTON_ESC, VK_ESCAPE);

		}
	} break;

	default: {
		TranslateMessage(&mesage);
		DispatchMessage(&mesage);
	}
	}
}

internal void
preparingSimulation(Input& input, HWND& window, HDC& hdc, LARGE_INTEGER& frame_begin_time, 
	float& delta_time,const float performance_frequency, int& columns, int& rows, u16*& grid, bool& repeat, bool& camera) {
	// preparing simulation  
	bool rowsSeted = false;
	running = true;
	repeat = false;
	bool endedPreparetion = false;
	LARGE_INTEGER frame_end_time;
	while (!endedPreparetion && running)
	{
		// Input 
		MSG mesage;

		for (int i = 0; i < BUTTON_COUNT; i++)
			input.buttons[i].changed = false;

		while (PeekMessage(&mesage, window, 0, 0, PM_REMOVE))
			procesButtons(mesage, input);
		// Simulate
		preparing_simulation(&input, grid, endedPreparetion, running, rows, columns, render_state, rowsSeted, camera);

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
	}
}

internal void
simulation(Input& input, HWND& window, HDC& hdc, LARGE_INTEGER& frame_begin_time,
	float& delta_time, const float performance_frequency, int& columns, int& rows, u16*& grid, bool& repeat) {
	LARGE_INTEGER frame_end_time;
	while (running)
	{
		// Input 
		MSG mesage;

		for (int i = 0; i < BUTTON_COUNT; i++)
			input.buttons[i].changed = false;

		while (PeekMessage(&mesage, window, 0, 0, PM_REMOVE))
			procesButtons(mesage, input);
		// Simulate

		simulate_game(&input, delta_time, rows, columns, grid, running, render_state, repeat);

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);
		QueryPerformanceCounter(&frame_end_time);
		delta_time = (float)(frame_end_time.QuadPart - frame_begin_time.QuadPart) / performance_frequency;
		frame_begin_time = frame_end_time;
	}
}

internal void 
setWindow(HWND& window, WNDCLASS& window_class, HINSTANCE& hInstance) {
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = L"Game Of Life";
	window_class.lpfnWndProc = window_callback;
	// Register Class 
	RegisterClass(&window_class);
	ShowCursor(FALSE);
	window = CreateWindow(window_class.lpszClassName, L"The name of the window", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInstance, 0);
	{
		// Fullscreen 
		SetWindowLong(window, GWL_STYLE, GetWindowLong(window, GWL_STYLE) & -WS_OVERLAPPEDWINDOW);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(MonitorFromWindow(window, MONITOR_DEFAULTTOPRIMARY), &mi);
		SetWindowPos(window, HWND_TOP, mi.rcMonitor.left, mi.rcMonitor.top, mi.rcMonitor.right - mi.rcMonitor.left, mi.rcMonitor.bottom - mi.rcMonitor.top, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

internal float 
setPerformanceFrequenci() {
	LARGE_INTEGER perf;
	QueryPerformanceFrequency(&perf);
	return (float)perf.QuadPart;
}

int WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nShowCmd) {
	// create windows class 
	WNDCLASS window_class = {};
	// Create Window
	HWND window;
	setWindow(window, window_class, hInstance);
	HDC hdc = GetDC(window);
	Input input = {};
	float delta_time = 0.016666f;
	LARGE_INTEGER frame_begin_time;
	QueryPerformanceCounter(&frame_begin_time);
	float performance_frequency = setPerformanceFrequenci();
	// prepare simulation
	int columns = 0, rows = 0;
	u16* grid = nullptr;
	bool repeat = true, camera = false;
	while (repeat) {
		preparingSimulation(input, window, hdc, frame_begin_time, delta_time, performance_frequency, columns, rows, grid, repeat, camera);
		simulation(input, window, hdc, frame_begin_time, delta_time, performance_frequency, columns, rows, grid, repeat);
	}
	
}