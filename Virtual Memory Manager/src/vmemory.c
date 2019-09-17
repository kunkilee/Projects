//Implement the API, modeling the translation of virtual page address to a
//physical frame address. We assume a 32 bit virtual memory and physical memory.
//Access to the page table is only via the CR3 register.

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "vmemory.h"
#include <unistd.h>
#include <fcntl.h>
#define OUT_TLB "../bin/tlb_out.txt"

#define BYTEMASK_FRONT_10 0xFFC00000
#define BYTEMASK_MID_10	0x3FF000
#define BYTEMASK_OFFSET 0xFFF
#define BYTEMASK_FRONTMID 0xFFFFF000

int tlb[8][2];
float tlb_miss;
float tlb_hit;
int entries;

bool FIFO_policy = true;
int **cr3;

// The implementation of get_vpage_cr3 is provided in
// an object file, so no need to re-implement it
void initialize_vmanager(int policy)
{
	// Set LRU policy when passsed as a parameter
	if (policy)
		FIFO_policy = false;
	cr3 = get_vpage_cr3();

}

int fetch_physical_frame(unsigned int v_addr)
{
	int front = 0;
	int mid = 0;
	int offs = 0;

	int bytes[3];
	bytes[0] = (v_addr & BYTEMASK_FRONT_10) >> 22; //contains first 10 MSB
	bytes[1] = (v_addr & BYTEMASK_MID_10) >> 12;//contains second 10 MSB
	bytes[2] = (v_addr & BYTEMASK_OFFSET) >> 0;//contains offset

	front = bytes[0];
	mid = bytes[1];
	offs = bytes[2];

	if(cr3[(int)front] == 0){ //checks first page
		return -1; //returns -1 if not found
	}

	if(cr3[(int)front][(int)mid] == 0){ //checks second page
		return -1; //returns -1 if not found
	}

	else{
		return (int)cr3[(int)front][(int)mid]; //return physical frame
	}
}


void print_physical_address(int frame, int offset)
{
	//TODO
	int results = 0;
	if(frame == -1)
	{
		printf("%d \n", -1);
	}
	else{
		results = (frame<<12) + offset;
		printf("0x%x \n", results);
	}
}

/* Takes 20MSB of virtual address and return associated physical frame base address
 if in tlb (tlb hit), if it is a miss returns -1*/
int get_tlb_entry(int n)
{
  //looks for entry already in TLB (hit)
  for(int i=0;i<8;i++){
    if(tlb[i][0]==n){
      tlb_hit++;
      return tlb[i][1];
    }
  }
  //Did not find entry, TLB miss
  tlb_miss++;
	return -1;
}

void populate_tlb(int v_addr, int p_addr)
{
	//if there is room in tlb put in next slot and increment entry count
	if(entries < 8){
		tlb[entries][0]=v_addr;
		tlb[entries][1]=p_addr;
		entries++;
		return;
  }
	//implement FIFO, shifts elements and adds new entry
	else if(entries >= 8){
		for(int i=0; i<7; i++){
			tlb[i][0]=tlb[i+1][0];
			tlb[i][1]=tlb[i+1][1];
		}
		tlb[entries-1][0]=v_addr;
		tlb[entries-1][1]=p_addr;

	}
	return;
}

/*calculates hit rate from misses and hits*/
float get_hit_ratio()
{
	float hit_rate = tlb_hit/(tlb_hit+tlb_miss);
	return hit_rate;
}

/*Write to the file in OUT_TLB, prints each entry in tlb*/
void print_tlb()
{
	FILE *tlb_out = fopen(OUT_TLB,"a");
	for(int i=0; i<8; i++) {
		if(tlb[i][0]==0){
			fprintf(tlb_out,"%d %d\n",-1,-1);
		}
		else{
		fprintf(tlb_out,"0x%x 0x%x\n",tlb[i][0],tlb[i][1]);
	}
	}
	fprintf(tlb_out, "\n");
  fclose(tlb_out);
	return;
}

int **populate_lru_tlb(int v_addr, int p_addr, int** lru_tlb) {
	for(int i = 9; i > 0; i--) {
		lru_tlb[i][0] = lru_tlb[i-1][0];
		lru_tlb[i][1] = lru_tlb[i-1][1];
	}

	lru_tlb[0][0] = v_addr;
	lru_tlb[0][1] = p_addr;

	return lru_tlb;
}

int **reorder_lru_tlb(int v_addr, int p_addr, int** lru_tlb) {
	int index = 9;
	for(int i = 0; i < 10; i++) {
		if(lru_tlb[i][0] == v_addr && lru_tlb[i][1] == p_addr) {
			index = i;
		}
	}

	for(int i = index; i > 0; i--) {
		lru_tlb[i][0] = lru_tlb[i-1][0];
		lru_tlb[i][1] = lru_tlb[i-1][1];
	}

	lru_tlb[0][0] = v_addr;
	lru_tlb[0][1] = p_addr;

	return lru_tlb;
}

void print_lru_tlb(int **lru_tlb) {
	FILE *tlb_out = fopen(OUT_TLB,"a");
	for(int i = 0; i < 10; i++) {
		if(lru_tlb[i][0] == 0) {
			fprintf(tlb_out,"%d %d\n",-1,-1);
		}

		else {
			fprintf(tlb_out,"0x%x 0x%x\n", lru_tlb[i][0], lru_tlb[i][1]);
		}
	}

	fprintf(tlb_out,"\n");
 	fclose(tlb_out);
	return;
}

int get_lru_tlb_entry(int ** lru_tlb, int n) {
	for(int i=0; i < 10; i++){
 		if(lru_tlb[i][0] == n) {
    		tlb_hit++;
	    	return lru_tlb[i][1];
		}
	}

  	tlb_miss++;
	return -1;
}

