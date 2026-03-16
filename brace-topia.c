//File: brace-topia.c
//Author: Marina Kania
//////////////
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>

int mirco_delay = 900000;
//Negative count means unset or infinite mode
int count = -1;
int dimension = 15;
int strength = 50;
int vacancy = 20;
int percent_endline = 60;



void print_usage() {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] 
    [-e %%end]\n");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "Option", "Default", "Example", 
    "Description");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "'-h'", "NA", "-h", 
    "print this usage message.");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "'-t N'", "900000", "-t 5000", 
    "microseconds cycle delay.");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "'-c N'", "NA", "-c4", 
    "count cycle maximum value.");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "'-d dim'", "15", "-d 7", 
    "width and height dimension.");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "'-s %%str'", "50", "-s 30", 
    "strength of preference.");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "'-v %%vac'", "20", "-v30", 
    "percent vacancies.");
    fprintf(stderr, "%-12s%-10s%-10s%s\n", "'-e %%endl'", "60", "-e75", 
    "percent Endline braces. Others want Newline.");
}



int main( int argc, char * argv[] ){
    while ( (opt = getopt( argc, argv, "ht:c:d:s:v:e:") ) != -1 ) {
        int opt;
        switch(opt){
            //Prints the help message
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            //Sets the cycle delay
            case 't':
                int tmp_t = (int)strtol(optarg, NULL, 10);
                if(tmp_t > 0){
                    micro_delay = tmp_t;
                }
                break;
            //Max count value
            case 'c':
                int tmp_c = (int)strtol(optarg, NULL, 10);
                if(tmp_c >= 0){
                    count = tmp_c;
                }
                break;
            //Sets the dimensions
            case 'd':
                int tmp_d = (int)strtol(optarg, NULL, 10);
                if(tmp_d >= 5 && tmp_d <= 39){
                    dimension = tmp_d;
                }
                break;
            //Sets the strength of preference
            case 's':
                int tmp_s = (int)strtol(optarg, NULL, 10);
                if(tmp_s > 0){
                    strength = tmp_s;
                }
                break;
            //Sets the percent vacancies
            case 'v':
                int tmp_v = (int)strtol(optarg, NULL, 10);
                if(tmp_v > 0){
                    vacancy = tmp_v;
                }
                break;
            //Sets the percent of endline braces
            case 'e':
                int tmp_e = (int)strtol(optarg, NULL, 10);
                if(tmp_e > 0){
                    percent_endline = tmp_e;
                }
                break;
            default:
            print_usage();
            return EXIT_FAILURE;
        }
    }
}



