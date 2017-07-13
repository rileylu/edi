#include "fixedbuffer.h"
#include <iostream>

int main()
{
	std::string str = "haha";
	buffer::FixedBuffer<buffer::SMALL> buf;
	buf.append(std::move(str));
	return 0;
}