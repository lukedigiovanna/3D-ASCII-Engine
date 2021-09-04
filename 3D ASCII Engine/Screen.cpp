#include "Screen.h"

void Screen::updateResolution()
{
	CONSOLE_SCREEN_BUFFER_INFO inf;
	GetConsoleScreenBufferInfo(handle, &inf);
	
	SHORT w = (inf.srWindow.Right - inf.srWindow.Left) / 2;
	SHORT h = inf.srWindow.Bottom - inf.srWindow.Top;

	if (w != this->width || h != this->height)
		this->forceClear();
	this->width = w;
	this->height = h;
}

void Screen::setPixel(SHORT x, SHORT y, TCHAR val)
{
	if (x >= 0 && x < this->width && y >= 0 && y < this->height)
		this->drawBuffer[y * width + x] = val;
}

void Screen::drawString(SHORT x, SHORT y, LPCWSTR string)
{
	WriteConsoleOutputCharacter(this->handle, string, wcslen(string), { x * 2, y }, &dw);
}

void Screen::clear(TCHAR val)
{
	for (SHORT x = 0; x < this->getWidth(); x++) {
		for (SHORT y = 0; y < this->getHeight(); y++) {
			this->setPixel(x, y, val);
		}
	}
}

void Screen::forceClear()
{
	TCHAR val = ' ';
	for (SHORT x = 0; x < MAX_WIDTH * 2; x++) {
		for (SHORT y = 0; y < MAX_HEIGHT; y++) {
			WriteConsoleOutputCharacter(this->handle, &val, 1, { x, y }, &this->dw);
			if (x % 2 == 0) cache[(x / 2) + y * this->getWidth()] = ' ';
		}
	}
}

void Screen::render()
{
	for (SHORT x = 0; x < this->getWidth(); x++) {
		for (SHORT y = 0; y < this->getHeight(); y++) {
			int idx = x + y * this->getWidth();
			TCHAR val = this->drawBuffer[idx];
			if (val != cache[idx]) {
				WriteConsoleOutputCharacter(this->handle, &val, 1, { x * 2, y }, &this->dw);
				cache[idx] = val;
			}
		}
	}
}
