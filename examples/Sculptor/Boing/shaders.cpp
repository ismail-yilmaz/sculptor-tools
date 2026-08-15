#include <Draw/Draw.h>

using namespace Upp;

Image CreateBoingTexture()
{
	ImageBuffer ib(512, 256);
	for(int y = 0; y < 256; y++) {
		for(int x = 0; x < 512; x++) {
			int cx = (x / 32) % 2;
			int cy = (y / 32) % 2;
			ib[y][x] = (cx == cy) ? Color(255, 50, 50) : White();
		}
	}
	return ib;
}

Image CreateGridTexture()
{
	constexpr int tsz = 128;
	constexpr int thickness = 4;
	
	ImageBuffer ib(tsz, tsz);
	for(int y = 0; y < tsz; y++) {
		for(int x = 0; x < tsz; x++) {
			if(x < thickness || y < thickness)
				ib[y][x] = Color(180, 0, 180);
			else
				ib[y][x] = Gray();
		}
	}
	return ib;
}
