#pragma once
#include <Windows.h>

#define MAX_WIDTH 150
#define MAX_HEIGHT 100

class Screen
{
private:
	HANDLE handle;
	unsigned short width = MAX_WIDTH, height = MAX_HEIGHT;
	
	TCHAR* cache;
	TCHAR* drawBuffer;
	float* depthBuffer;
	
	DWORD dw;

public:
	Screen(HANDLE handle) : handle(handle), dw(0) {
		this->depthBuffer = new float[MAX_WIDTH * MAX_HEIGHT];
		this->drawBuffer = new TCHAR[MAX_WIDTH * MAX_HEIGHT];
		this->cache = new TCHAR[MAX_WIDTH * MAX_HEIGHT];
	}
	~Screen() {
		delete[] depthBuffer;
		delete[] drawBuffer;
	}

	void updateResolution();
	void setPixel(SHORT x, SHORT y, TCHAR val); //sets the pixel in the draw buffer.. to render the screen you need to make a call to 'render'
	void rasterize(const SHORT points[6], const float depth[3]);
	void drawString(SHORT x, SHORT y, LPCWSTR string);
	void clear(TCHAR val);
	void forceClear();
	void render(); // actually draws the draw buffer to the screen and updates the cache

	unsigned short getWidth() { return this->width; }
	unsigned short getHeight() { return this->height; }
};

