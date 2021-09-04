#pragma once
#include <Windows.h>

#define MAX_WIDTH 150
#define MAX_HEIGHT 100

class Screen
{
private:
	HANDLE handle;
	unsigned short width = MAX_WIDTH, height = MAX_HEIGHT;
	TCHAR cache[MAX_WIDTH * MAX_HEIGHT];
	TCHAR drawBuffer[MAX_WIDTH * MAX_HEIGHT];
	DWORD dw;
public:
	Screen(HANDLE handle) : handle(handle) {}

	void updateResolution();
	void setPixel(SHORT x, SHORT y, TCHAR val); //sets the pixel in the draw buffer.. to render the screen you need to make a call to 'render'
	void drawString(SHORT x, SHORT y, LPCWSTR string);
	void clear(TCHAR val);
	void forceClear();
	void render(); // actually draws the draw buffer to the screen and updates the cache

	unsigned short getWidth() { return this->width; }
	unsigned short getHeight() { return this->height; }
};

