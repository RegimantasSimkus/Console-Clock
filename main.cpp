#include <iostream>
#include <sstream>

#define WIN32_LEAN_AND_MEAN
#include <Windows.h> // GetLocalTime

constexpr float PI = (float)3.14159265358979323846;
constexpr float RAD = (PI/180.f);

constexpr int clamp(int val, const int min, const int max)
{
	if (val < min)
		return min;
	if (val > max)
		return max;
	return val;
}

template <int W, int H>
void GenerateClock(char (&matrix)[H][W])
{
	memset(matrix, ' ', sizeof(matrix));

	// might as well use floats for better accuracy despite it being integers in the end
	constexpr float cx = W / 2.f;
	constexpr float cy = H / 2.f;

	// we need the brim!
	for (int i = 0; i < 360; i++)
	{
		int x = (int)(cx + (cos(RAD * i) * cx));
		int y = (int)(cy + (sin(RAD * i) * cy));

		//x += cx;
		//y += cy;
		
		matrix[y][x] = 'X';
	}

	// hour ticks!
	for (int i = 0; i < 360; i += (360 / 12))
	{
		// size of the tick
		for (int j = 0; j < 4; j++)
		{
			int x = (int)(cx + (cos(RAD * i) * (cx - j - 1)));
			int y = (int)(cy + (sin(RAD * i) * (cy - j - 1)));

			matrix[y][x] = 'x';
		}
	}
	return;
}

template <int W, int H>
void PrintMatrix(char (&matrix)[H][W])
{
	// build in ostream for smoother appearance
	std::ostringstream str;
	for (int h = 0; h < H; h++)
	{
		for (int w = 0; w < W; w++)
		{
			// print double to compensate for aspect ratio otherwise its thin and wack
			str << matrix[h][w];
			str << matrix[h][w];
		}
		str << "\r\n";
	}

	std::cout << str.str();
}

template <int W, int H>
void GenerateHand(const char* str, float angle, int length, char(&matrix)[H][W])
{
	constexpr float cx = W / 2.f;
	constexpr float cy = H / 2.f;
	
	// offset the angle so that 0 degrees is the top
	angle -= 90.f;

	int strLen = (int)strlen(str);
	int nthStr = 0;
	char c = *str;

	for (int i = 0; i < length; i++)
	{
		// clamp just incase of floating point unpredictability
		int x = clamp((int)(cx + (cos(RAD * angle) * i)), 0, W - 1);
		int y = clamp((int)(cy + (sin(RAD * angle) * i)), 0, H - 1);
		
		char* slot = &matrix[y][x];
		if (*slot == 'X') // dont overwrite brim
			continue;
		
		*slot = c;
		if (nthStr >= strLen)
			nthStr = 0;
		c = str[nthStr++];
	}
}

void HideCaret()
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_CURSOR_INFO info;
	GetConsoleCursorInfo(console, &info);

	info.bVisible = false;
	info.dwSize = 1;
	SetConsoleCursorInfo(console, &info);
}

int main()
{
	constexpr int H = 40;
	constexpr int W = 40;

	HideCaret();

	WORD lastGeneration = -1;
	while (true)
	{
		SYSTEMTIME systime;
		GetLocalTime(&systime);
		Sleep(100);

		if (systime.wSecond == lastGeneration)
			continue;
		lastGeneration = systime.wSecond;
		system("cls");

		char matrix[H][W];
		GenerateClock(matrix);
		GenerateHand("S", ((systime.wSecond) / 60.f) * 360.f, (int)(H * 0.4f), matrix);
		GenerateHand("M", ((systime.wMinute) / 60.f) * 360.f, (int)(H * 0.45f), matrix);
		GenerateHand("H", (((systime.wHour) / 12.f) + ((systime.wMinute / 60.f) / 12.f)) * 360.f, (int)(H * 0.33f), matrix);

		matrix[H / 2][W / 2] = '#';
		PrintMatrix(matrix);
	}
}