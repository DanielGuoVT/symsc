// Copyright notice here

#include <stdint.h>
#include <assert.h>

int main(int argc, char **argv) {
	uint64_t value;

	if (value + 0xFFFFFFFFFFFFFFFCL < 3 &&
			value + 0xFFFFFFFFFFFFFFF8L >= 3) {
		assert(0 && "ERROR!");
	}
	
	return 0;
}
