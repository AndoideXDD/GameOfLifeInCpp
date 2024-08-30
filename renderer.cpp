#include "utils.cpp"


internal void 
clear_screen(u32 color, Render_State render_state) {
	u32* pixel = (u32*)render_state.memory;
	for (int y = 0; y < render_state.height; y++) {
		for (int x = 0; x < render_state.width; x++) {
			*pixel++ = color;
		}
	}
}



internal void 
draw_react_in_pixels(int x0, int y0, int x1, int y1, u32 color, Render_State render_state) {
	x0 = clamp(0, x0, render_state.width);
	x1 = clamp(0, x1, render_state.width);
	y0 = clamp(0, y0, render_state.height);
	y1 = clamp(0, y1, render_state.height);


	for (int y = y0; y < y1; y++) {
		u32* pixel = (u32*)render_state.memory + x0 + y * render_state.width;
		for (int x = x0; x < x1; x++) {
			*pixel++ = color;
		}
	}
}

global_variable float render_scale = 0.01f;

internal void 
draw_react(float x, float y, float half_size_x, float half_size_y, u32 color, Render_State render_state) {
	
	x *= render_state.height * render_scale;
	y *= render_state.height * render_scale;
	half_size_x *= render_state.height * render_scale;
	half_size_y *= render_state.height * render_scale;
	
	x += render_state.width / 2.f;
	y += render_state.height / 2.f;


	int x0 = x - half_size_x;
	int x1 = x + half_size_x;
	int y0 = y - half_size_y;
	int y1 = y + half_size_y;

	draw_react_in_pixels(x0, y0, x1, y1, color, render_state);
}

const char* letters[][7] = {
	" 000",
	"0   0",
	"0   0",
	"00000",
	"0   0",
	"0   0",
	"0   0",

	"0000",
	"0   0",
	"0   0",
	"0000",
	"0   0",
	"0   0",
	"0000",

	"00000",
	"0",
	"0",
	"0",
	"0",
	"0",
	"00000",

	"0000",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0000",

	"00000",
	"0",
	"0",
	"00000",
	"0",
	"0",
	"00000",

	"00000",
	"0",
	"0",
	"00000",
	"0",
	"0",
	"0",

	"00000",
	"0",
	"0",
	"0 000",
	"0   0",
	"0   0",
	"00000",

	"0   0",
	"0   0",
	"0   0",
	"00000",
	"0   0",
	"0   0",
	"0   0",

	"00000",
	"  0",
	"  0",
	"  0",
	"  0",
	"  0",
	"00000",

	"    0",
	"    0",
	"    0",
	"    0",
	"0   0",
	"0   0",
	" 000",

	"0   0",
	"0   0",
	"0 00",
	"000",
	"000",
	"0 00",
	"0   0",

	"0",
	"0",
	"0",
	"0",
	"0",
	"0",
	"00000",

	"00 00",
	"0 0 0",
	"0 0 0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",


	"00  0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	"0  00",

	"00000",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"00000",

	" 000",
	"0   0",
	"0   0",
	"0000",
	"0",
	"0",
	"0",

	" 000 ",
	"0   0",
	"0   0",
	"0   0",
	"0 0 0",
	"0  0 ",
	" 00 0",

	"0000",
	"0   0",
	"0   0",
	"0000",
	"0   0",
	"0   0",
	"0   0",

	" 0000",
	"0",
	"0 ",
	" 000",
	"    0",
	"    0",
	"0000",

	"00000",
	"  0",
	"  0",
	"  0",
	"  0",
	"  0",
	"  0",

	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	" 000",

	"0   0",
	"0   0",
	"0   0",
	"0   0",
	"0   0",
	" 0 0",
	"  0",

	"0   0 ",
	"0   0",
	"0   0",
	"0 0 0",
	"0 0 0",
	"0 0 0",
	" 0 0 ",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	" 0 0",
	"0   0",
	"0   0",

	"0   0",
	"0   0",
	" 0 0",
	"  0",
	"  0",
	"  0",
	"  0",

	"00000",
	"    0",
	"   0",
	"  0",
	" 0",
	"0",
	"00000",

};

internal void 
draw_text(const char *text, float x, float y, float size, u32 color, Render_State render_state) {
	float half_size = size * .5f;
	float original_y = y;
	float rect_size = size * 0.9;
	while (*text) {
		if (*text != 32)
		{
			const char** a_letter = letters[*text - 'A']; // Starting with A
			float original_x = x;

			for (int i = 0; i < 7; i++) {
				const char* row = a_letter[i];
				while (*row) {
					if (*row == '0') {
						draw_react(x, y, half_size, half_size, color, render_state);
					}
					x += rect_size;
					row++;
				}
				y -= rect_size;
				x = original_x;
			}
		}
		
		text++;
		x += size * 6.f;
		y = original_y;
	}
	
}

internal void
draw_number(int number, float x, float y, float size, u32 color, Render_State render_state) {
	float half_size = size * .5f;
	bool render_number = false;
	while (number != 0 || !render_number){
		render_number = true;
		int digit = number % 10;
		number /= 10;
		switch (digit)
		{
		case 0: {
			draw_react(x - size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x + size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x, y + size * 2.f, half_size, half_size, color, render_state);
			draw_react(x, y - size * 2.f, half_size, half_size, color, render_state);
			x -= size * 4.f;
		} break;
		case 1: {
			draw_react(x + size, y, half_size, 2.5f * size, color, render_state);
			x -= size * 2.f;
		} break;
		case 2: {
			draw_react(x, y + size * 2.f, 1.5f * size, half_size, color, render_state);
			draw_react(x, y - size * 2.f, 1.5f * size, half_size, color, render_state);
			draw_react(x, y, 1.5f * size, half_size, color, render_state);
			draw_react(x + size, y + size, half_size, half_size, color, render_state);
			draw_react(x - size, y - size, half_size, half_size, color, render_state);
			x -= size * 4.f;
		} break;
		case 3: {
			draw_react(x - half_size, y + size * 2.f, size, half_size, color, render_state);
			draw_react(x - half_size, y - size * 2.f, size, half_size, color, render_state);
			draw_react(x - half_size, y, size, half_size, color, render_state);
			draw_react(x + size, y, half_size, 2.5f * size, color, render_state);
			x -= size * 4.f;
		} break;
		case 4: {
			draw_react(x + size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x - size, y + size, half_size, 1.5f * size, color, render_state);
			draw_react(x, y, half_size, half_size, color, render_state);
			x -= size * 4.f;
		} break;
		case 5: {
			draw_react(x, y + size * 2.f, 1.5f * size, half_size, color, render_state);
			draw_react(x, y, 1.5f * size, half_size, color, render_state);
			draw_react(x, y - size * 2.f, 1.5f * size, half_size, color, render_state);
			draw_react(x - size, y + size, half_size, half_size, color, render_state);
			draw_react(x + size, y - size, half_size, half_size, color, render_state);
			x -= size * 4.f;
		} break;
		case 6: {
			draw_react(x + half_size, y + size * 2.f, size, half_size, color, render_state);
			draw_react(x + half_size, y, size, half_size, color, render_state);
			draw_react(x + half_size, y - size * 2.f, size, half_size, color, render_state);
			draw_react(x - size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x + size, y - size, half_size, half_size, color, render_state);
			x -= size * 4.f;
		} break;

		case 7: {
			draw_react(x + size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x - half_size, y + size * 2.f, size, half_size, color, render_state);
			x -= size * 4.f;
		} break;
		case 8: {
			draw_react(x - size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x + size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x, y + size * 2.f, half_size, half_size, color, render_state);
			draw_react(x, y - size * 2.f, half_size, half_size, color, render_state);
			draw_react(x, y, half_size, half_size, color, render_state);
			x -= size * 4.f;
		} break;
		case 9: {
			draw_react(x - half_size, y + size * 2.f, size, half_size, color, render_state);
			draw_react(x - half_size, y, size, half_size, color, render_state);
			draw_react(x - half_size, y - size * 2.f, size, half_size, color, render_state);
			draw_react(x + size, y, half_size, 2.5f * size, color, render_state);
			draw_react(x - size, y + size, half_size, half_size, color, render_state);
			x -= size * 4.f;
		} break;
		default: {

		} break;
		}
	}
	
}
