#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdint.h>
#include <string.h>

typedef struct { //struct for page
	int frameNum; //which frame a page is in
	int presentBit; //is the page in physical memory(stored in a frame)
	int useBit; //is the page in use
	int victimPage; //this is only for the page fault and clock replace i didnt want to make another structure for it
} pageEntry;

int numFrame = 16; //from here to 38(at time of writing) is to not have to pass more variables than necessary
int numPage = 64;

int pageSize = 256;
int offset = 8;

pageEntry *pageTable = NULL; //page and frame init
int *frame = NULL;

int clockNum = 0;
int replacements = 0;

pageEntry clockReplace(int pageNum) {
	while (1) {
		int clockPage = frame[clockNum]; //starts at frame 0 and moves around

		if (pageTable[clockPage].useBit == 0) { //check is page is in use

			pageEntry victim; //i dont know why i chose to use victim as the variable name my v key is broken
			victim.frameNum = clockNum; //what frame will be open
			victim.victimPage = clockPage; //what page will be evicted

			//clear page to be replaced
			pageTable[clockPage].frameNum = -1; //clockPage page is not in any frame and can be moved
			pageTable[clockPage].presentBit = 0; //not present in physical memory yet
			pageTable[clockPage].useBit = 0; //not yet in use

			frame[clockNum] = pageNum; //page now stored in frame[clocknum]

			clockNum = (clockNum + 1) % numFrame; //clock will go in a circle resetting to 0 eventually
			replacements++;

			return victim;
		}
		pageTable[clockPage].useBit = 0; //page has to be used again before the clock comes around again to be kept in use
		clockNum = (clockNum + 1) % numFrame;
	}
}
int findFrame() {
	for (int i = 0; i < numFrame; i++)
		if (frame[i] == -1)
			return i;
	return -1; //no avaliable frames
}
int pageFault(int pageNum) {
	int freeFrame = findFrame(); //find open frame
	if (freeFrame != -1) { // frames open
		frame[freeFrame] = pageNum; //page is stored in freeFrame
		pageTable[pageNum].frameNum = freeFrame; //page in freeFrame
		pageTable[pageNum].presentBit = 1; //page stored in a frame
		pageTable[pageNum].useBit = 1;
		printf("PAGE FAULT: loaded page %d into frame %d", pageNum, freeFrame);
		return freeFrame;
	}
	//need replacement
	pageEntry victim = clockReplace(pageNum); //all frames have a page stored in it
	pageTable[pageNum].frameNum = victim.frameNum; //page now in evicted frame
	pageTable[pageNum].presentBit = 1; //page in physical memory
	pageTable[pageNum].useBit = 1; //page in use

	printf("PAGE FAULT EVICTION NEEDED: evicted page %d from frame %d",
			victim.victimPage, victim.frameNum);
	return victim.frameNum;
}

void translateAddress(uint32_t logicalAddress) {
	uint32_t pageNum = logicalAddress >> offset; //shifts the logical address right by offset bits
	uint32_t offsetMask = logicalAddress & (pageSize - 1); //gets the offset mask using bitwise AND
	printf("Translating Virtual Address=0x%04X (page=%u, offset=%u): ",
			logicalAddress, pageNum, offsetMask);

	if (pageTable[pageNum].presentBit) { //if page exists in memory
		int physFrame = pageTable[pageNum].frameNum; //which frame the page is in
		pageTable[pageNum].useBit = 1; //page is in use
		uint32_t physAddress = (physFrame << offset) | offsetMask; //get physical address by shifting bits and using bitwise OR
		printf("PAGE PRESENT: page=%d, frame=%d, physical address=0x%04X\n",
				pageNum, physFrame, physAddress);
		return;
	}
	//if page does not exist in memory
	int physFrame = pageFault(pageNum); //get frame for page to be stored in
	uint32_t physAddress = (physFrame << offset) | offsetMask; //get physical address by shifting bits and using bitwise OR
	printf(", physical address=0x%04X\n", physAddress);
}

void addressGenerator(int genNum) {
	srand(time(NULL)); //this line will let the generator generate different random addresses each time this code is run

	for (int i = 0; i < genNum; i++) {
		//uint32_t is an integer that is 32 bits long and is used here for addresses and necessary bit math
		uint32_t maxAddress = pageSize * numPage; //4096 is maximum address for what ive set at time of writing
		uint32_t addressNum = rand() % maxAddress; //gets a valid address in the range [0, 4095] by dividing the generated number by maxAddress
		//and the remainder is addressNum which is shown as a hexadecimal value
		printf("Generated Virtual Address %d: 0x%04X\n", i + 1, addressNum);
		translateAddress(addressNum);//no reason to send back to main just to send to translateAddress
	}
	return;
}
int main(int argc, char **argv) {
	printf("How many virtual addresses should be generated? ");
	int genNum;
	scanf("%d", &genNum);
	if (genNum <= 0) { //must generate a positive amount
		printf(
				"Entered an invalid amount, 20 virtual addresses will be generated\n");
		genNum = 20;
	}

	pageTable = malloc(sizeof(pageEntry) * numPage);
	frame = malloc(sizeof(int) * numFrame);

	for (int i = 0; i < numPage; i++) {
		pageTable[i].frameNum = -1; //page is not in any frame
		pageTable[i].presentBit = 0; //not yet present in memory
		pageTable[i].useBit = 0; //page not in use
	}
	for (int i = 0; i < numFrame; i++) {
		frame[i] = -1; //frame has no page inside it
	}
	clockNum = 0;
	addressGenerator(genNum); //start generating addresses
	printf("Total evictions: %d\n", replacements);
	return 0;
}
