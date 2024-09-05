#include "renderer.cpp"
#include <cstdlib>
#include <ctime>
#include <cmath>

#define is_down(b) input->buttons[b].is_down
#define pressed(b) (input->buttons[b].is_down && input->buttons[b].changed)
#define realeased(b) (!input->buttons[b].is_down && input->buttons[b].changed)

 
internal void 
preparing_simulation(Input* input, u16*& grid, bool& endedPreparetion, bool& running, int& rows, int& columns, Render_State& render_state, bool& rowsSeted){
	clear_screen(0, render_state);
	int amountTosSum = 100;
	if (pressed(BUTTON_ENTER)) {
		if (!rowsSeted && rows != 0) rowsSeted = true;
		else if (rowsSeted && columns != 0) { 
			int dataLengh = columns * rows;
			endedPreparetion = true; 
			if (grid != nullptr) delete grid;  
			grid = new u16[dataLengh];
			// set alive or dead
			srand(time(0));
			for (int i = 0; i < dataLengh; i++) grid[i] = rand() % 2;
		}
	}

	if (rowsSeted) {
		draw_text("COLUMNS THAT YOU WANT", -50, 30, 1, 0xffffff, render_state);
		draw_text("PRESS ENTER TO START SIMULATING", -50, 20, 0.5, 0xfc0606, render_state);
		draw_text("WE RECOMEND HALF ROWS THAN COLUMS", -50, 15, 0.5, 0xfc0606, render_state);
		draw_number(columns, 0, 0, 1, 0xffffff, render_state);
	}
	else {
		draw_text("ROWS THAT YOU WANT", -50, 30, 1, 0xffffff, render_state);
		draw_text("PRESS ENTER TO SET NEXT", -50, 20, 0.5, 0xfc0606, render_state);
		draw_text("WE RECOMEND HALF ROWS THAN COLUMS", -50, 15, 0.5, 0xfc0606, render_state);

		draw_number(rows, 0, 0, 1, 0xffffff, render_state);
	}
	
	

	if (is_down(BUTTON_W)) {
		draw_react(42, -3, 5, 5, 0xccccff, render_state);
		draw_text("W", 40, 0, 1, 0xfc0606, render_state);
		draw_react(42, -14, 5, 5, 0xccccff, render_state);
		draw_text("S", 40, -11, 1, 0x000000, render_state);
		if (input->buttons[BUTTON_W].changed) if (!rowsSeted) {
			rows += amountTosSum;
		} else {
			columns += amountTosSum;
		}

	}
	else if(is_down(BUTTON_S) ) {
		draw_react(42, -3, 5, 5, 0xccccff, render_state);
		draw_text("W", 40, 0, 1, 0x000000, render_state);
		draw_react(42, -14, 5, 5, 0xccccff, render_state);
		draw_text("S", 40, -11, 1, 0xfc0606, render_state);
		if (input->buttons[BUTTON_S].changed) if (!rowsSeted && rows > amountTosSum - 1) {
			rows -= amountTosSum;
		}
		else if(columns > amountTosSum - 1) {
			columns -= amountTosSum;
		}
	}
	else
	{
		draw_react(42, -3, 5, 5, 0xccccff, render_state);
		draw_text("W", 40, 0, 1, 0x000000, render_state);
		draw_react(42, -14, 5, 5, 0xccccff, render_state);
		draw_text("S", 40, -11, 1, 0x000000, render_state);
	}

	if (is_down(BUTTON_ESC)) running = false;

}

internal void
procesNeighbours(const int rows, const int columns, u16*& grid,const int row,const int colum, Render_State& render_state, const float size) {
	int neighbours = 0;
	int newRow, newColum;
	for (int y = -1; y < 2; y++) {
		for (int x = -1; x < 2; x++) {
			newColum = x + colum;
			newRow = y + row;
			if (!(newRow < 0 && newRow >= rows && newColum < 0 && newColum >= columns))
			{
				if (grid[newColum + (newRow)*columns] == 1) neighbours++;
			}
		}
	}
	neighbours--;
	if (neighbours < 2) grid[colum + row * columns] = 0; // die
	else if (neighbours > 3) grid[colum + row * columns] = 0; // die
	else if (neighbours == 3) { 
		grid[colum + row * columns] = 1; // new live 
		draw_react(float(colum) / columns * 160 - 80, float(row) / rows * 80 - 40, size, size, 0xffffff, render_state);
	} else {
		draw_react(float(colum)/columns*160 - 80, float(row)/rows * 80 - 40, size, size, 0xffffff, render_state);
	}
}



internal void 
procesFrame(int rows, int columns, u16*& grid, Render_State& render_state) {
	float size = 45.f / columns;
	for (int row = 0; row < rows; row++) {
		for (int colum = 0; colum < columns; colum++) {
			procesNeighbours(rows, columns, grid, row, colum, render_state, size);
		}
	}
}

internal void 
showGrid(u16*& grid, int rows, int columns, Render_State& render_state) {
	float size = 45.f / columns;
	for (int row = 0; row < rows; row++) {
		for (int colum = 0; colum < columns; colum++) {
			if (grid[colum + row * columns] == 1) {
				draw_react(float(colum)/columns*160 - 80, float(row)/rows * 80 - 40, size, size, 0xffffff, render_state);
			}
		}
	}
}

internal void
simulate_game(Input* input, float dt, int rows, int columns, u16*& grid, bool& running, Render_State& render_state) {
	// the ability to freeze time pressing w
	clear_screen(0, render_state);
	if (!is_down(BUTTON_F)) procesFrame(rows, columns, grid, render_state);
	else showGrid(grid, rows, columns, render_state);
	if (is_down(BUTTON_ESC)) running = false;
	//Sleep(100);
}