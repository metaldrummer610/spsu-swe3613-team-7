#include "Utils.h"
#include <iostream>
#include <stdio.h>
#include <string.h>

void dump(const char* str)
{
	std::cout << str;
}

void dumpBuffer(const char* buffer, int bufferSize)
{
	char buf[512];
	char ascbuf[19];

	for(int i = 0; i < bufferSize; i += 16)
	{
		char* p = buf;
		int count = 16;
		int rem = bufferSize - i;
		
		if(rem < 16)
			count = rem;

		memset(buf, 0, sizeof(buf));
		memset(ascbuf, ' ', sizeof(ascbuf));
		ascbuf[0] = '[';
		ascbuf[17] = ']';
		ascbuf[18] = '\0';

		sprintf(p, "%08x: ", i);
		p += 10;

		for(int j = 0; j < count; j++)
		{
			int c = (buffer[i + j] & 0xff);
			sprintf(p, "%02x ", c);
			p += 3;

			if(isprint(c))
				ascbuf[j + 1] = c;
			else
				ascbuf[j + 1] = '.';
		}

		for(int j = 0; j < (16 - rem); j++)
		{
			strcpy(p, "   ");
			p += 3;
		}

		strcpy(p, ascbuf);
		p += strlen(ascbuf);
		*p++ = '\r';
		*p++ = '\n';
		*p++ = '\0';

		dump(buf);
	}
}
