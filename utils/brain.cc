#include <stdint.h>
#include <stdio.h>

#include <util.h>

#include <Brain.hh>

int main(int argc, char *argv[])
{
	Brain b;

	if (argc != 2) {
		fprintf(stderr, "usage: brain <luascript>\n");
		return 1;
	}

	b.runFile(argv[1]);

	return 0;
}
