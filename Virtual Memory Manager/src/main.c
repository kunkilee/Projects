//Name : Kun Ki Lee, Maximillian Risdall, Julia Brooks
//Project 3
//main.c program to translate the virtual addresses from the virtual.txt
//into the physical frame address. Project 3 - CSci 4061
#define _POSIX_C_SOURCE 200809L
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#include <dirent.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <grp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "vmemory.h"

#define OUT_TLB "../bin/tlb_out.txt"
#define INPUT_FILE "../bin/virtual.txt"
#define BYTEMASK_OFFSET 0xFFF
#define BYTEMASK_FIRST20 0xFFFFF000
#define BYTEMASK_FRONTMID 0xFFFFF000

int main(int argc, char* argv[])
{
	int *offset_arr;
	int *frontmid_arr;
	int count = 0;


	if (argc > 2) {
		printf("Too many arguments, enter up to one argument\n");
		exit(-1);
	}

	int policy = (argc == 2) ? 1:0;
	initialize_vmanager(policy);

	FILE *infile;
	char *line = NULL;
	size_t len = 0;
	size_t nread;
	infile = fopen(INPUT_FILE, "r");

	if (infile == NULL) {
		perror("File does not exist");
		exit(1);
	}

	while ((getline(&line, &len, infile)) != -1) {
		count++; //to get the number of virtual address
	}

	fclose(infile);
	infile = fopen(INPUT_FILE, "r");
	int i = 0;
	unsigned int *virtual_arr = malloc(sizeof(unsigned int *)*count); //creates an array according to the number of virtual address

	while ((getline(&line, &len, infile)) != -1){
		virtual_arr[i] = strtoul(line, NULL, 16); //sotres virtual addresses into virtual_arr
		i++;
	}

	free(line);
	fclose(infile);

	offset_arr = malloc(sizeof(int *)*count);
	for(int k = 0; k < count; k++){
		offset_arr[k] = ((unsigned int)virtual_arr[k] & BYTEMASK_OFFSET) >> 0;
	}
	FILE *tlb_out = fopen(OUT_TLB, "w");
	if(argc == 1) {
		fprintf(tlb_out, "FIFO\n");
		//prints physical addresses and populates tlb
		//if already in tlb print from tlb, else fetch physical frame
		fclose(tlb_out);
		for(int z = 0; z < count; z++){

			int page = (virtual_arr[z] & BYTEMASK_FRONTMID) >> 12;
			int from_tlb = get_tlb_entry(page);
			printf("0x%x : ", virtual_arr[z]);

			if(from_tlb != -1){
				print_physical_address(from_tlb,offset_arr[z]);
			}
			else{
				populate_tlb(page,fetch_physical_frame(virtual_arr[z]));
				print_tlb();
				print_physical_address((fetch_physical_frame(virtual_arr[z])),offset_arr[z]);
			}
			printf("\n");
		}

		//get and print hit ratio
		float hits = get_hit_ratio();
		printf("Hit Ratio: %f\n", hits);
	}

	else if(strcmp(argv[1], "-lru") == 0) {
		fprintf(tlb_out, "LRU\n");
		fclose(tlb_out);
		int** lru_tlb = (int**) malloc(10*sizeof(int*));
		for(int i = 0; i < 10; i++) {
			lru_tlb[i] = malloc(sizeof(int)*2);
		}

		for(int i = 0; i < 10; i++) {
			for(int j = 0; j < 2; j++) {
				lru_tlb[i][j] = 0;
			}
		}

		//prints physical addresses and populates tlb
		//if already in tlb print from tlb, else fetch physical frame
		for(int k = 0; k < count; k++){

			int page = (virtual_arr[k] & BYTEMASK_FRONTMID) >> 12;
			int from_tlb = get_lru_tlb_entry(lru_tlb, page);
			printf("0x%x : ", virtual_arr[k]);

			if(from_tlb != -1) {
				print_physical_address(from_tlb, offset_arr[k]);
				lru_tlb = reorder_lru_tlb(page, fetch_physical_frame(virtual_arr[k]), lru_tlb);
			}

			else {
				lru_tlb = populate_lru_tlb(page, fetch_physical_frame(virtual_arr[k]), lru_tlb);
				print_lru_tlb(lru_tlb);
				print_physical_address((fetch_physical_frame(virtual_arr[k])),offset_arr[k]);
			}

			printf("\n");
		}

		//get and print hit ratio
		float hits = get_hit_ratio();
		printf("Hit Ratio: %f\n", hits);
	}

	else {
		//do nothing
	}

	free_resources();
	return 0;
}
