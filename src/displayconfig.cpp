#include "displayconfig.h"

// buffer needed for high resolutions
uint8_t resbuffer[100000];

void patchresbuffer() {
	patchDWord((void*)(0x0049B3BB+1), (uint32_t)&resbuffer);
}

