//File: brace-topia.c
//Author: Marina Kania
//////////////
#define _DEFAULT_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <getopt.h>
#include <unistd.h>
#include "display.h"

int micro_delay = 900000;
//Negative count means unset or infinite mode
static int count = -1;
static int dimension = 15;
static int strength = 50;
static int vacancy = 20;
static int percent_endline = 60;
static char grid[39][39];

typedef struct {
    int r, c;
} Pos;


void print_usage() {
    fprintf(stderr, "usage:\n");
    fprintf(stderr, "brace-topia [-h] [-t N] [-c N] [-d dim] [-s %%str] [-v %%vac] " 
    "[-e %%end]\n");
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

//Initializes the grid
void initialize_grid(int dim, int vacant, int endline){
    int total = dim * dim;
    int vacant_amount = (vacant/100) * total;
    int endline_amount = ((endline/100) * total) - vacant_amount;
    int newline_amount = total - (endline_amount + vacant_amount);
    char flat[total];
    int index = 0;
    for(int i = 0; i < vacant_amount; i++){
        flat[index] = '.';
        index++;
    }
    for(int j = 0; j < endline_amount; j++){
        flat[index] = 'e';
        index++;
    }
    for(int k =0; k < newline_amount; k++){
        flat[index] = 'n';
        index++;
    }
    //The Fisher-Yates Shuffle
    for(int i = total -1; i > 0; i--){
        int j = rand() % (i+1);
        char tmp = flat[i];
        flat[i] = flat[j];
        flat[j] = tmp;
    }

    //Copy into the 2-D array created above
    index = 0;
    for(int r = 0; r < dim; r++){
        for(int c = 0; c < dim; c++){
            grid[r][c] = flat [index];
            index++;
        }
    }
}

//Find the current happiness
//Found by finding the ratio of all the neighbors who code like the agent
//Compared to the total of non-vacant neighbors
//If no neighbors, the agents happinness is 1.0
float find_happiness(int row, int col){
    int same_neighbors;
    int total_neighbors;
    float happiness;
    //Directions!
    int dr[8] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dc[8] = {-1, 0, 1, -1, 1, -1, 0, 1};
    //Find the non-vacant neighbors
    for (int i = 0; i < 8; i++) {
        int nr = row + dr[i];
        int nc = col + dc[i];

        // Boundary check
        if (nr >= 0 && nr < dimension && nc >= 0 && nc < dimension) {
            // Valid neighbor
            if(grid[nr][nc] != '.'){
                total_neighbors++;
                if(grid[nr][nc] == grid[row][col]){
                    same_neighbors++;
                }
            }
        }
    }

    //Calculate happiness
    if(total_neighbors == 0){
        happiness = 1.0;
    }else{
        happiness = same_neighbors/total_neighbors;
    }
    return happiness;
}

//The agent must move to a vacant spot which is available at the beginning
//Each agent can move up to one time per cycle as only agents unhappy will move
//If there is no vacant spot for the agent, it will not move
//If a vacant site was used by an earlier move, agent will not move again
//this cycle.
//If a location becomes available within the cycle, other agents can't use it
//I'm going to move by the first vacant location
int move(){
    int total = dimension * dimension;
    int moves = 0;
    Pos vacant[(int)(vacancy/100.0) * total];
    int vacant_count = 0;

    //Record all vacant cells at START of cycle
    for (int r = 0; r < dimension; r++) {
        for (int c = 0; c < dimension; c++) {
            if (grid[r][c] == '.') {
                vacant[vacant_count++] = (Pos){r, c};
            }
        }
    }

    //Track which vacancies are already used
    int used[dimension * dimension];
    for(int i = 0; i < vacant_count; i++){
        used[i] = 0;
    }

    //Track which agents have already moved
    int moved[dimension][dimension];
    for (int r = 0; r < dimension; r++) {
        for (int c = 0; c < dimension; c++) {
            moved[r][c] = 0;
        }
    }

    //Iterate through grid
    for (int r = 0; r < dimension; r++) {
        for (int c = 0; c < dimension; c++) {
            // Skip empty cells
            if (grid[r][c] == '.') continue;
            // Skip agents that already moved this cycle
            if (moved[r][c]) continue;
            // Only move unhappy agents
            if (find_happiness(r,c) < strength){
                // Try to find a valid vacant location
                for (int i = 0; i < vacant_count; i++) {
                    if (used[i]) continue;  // already taken
                    int vr = vacant[i].r;
                    int vc = vacant[i].c;
                    // Move agent
                    grid[vr][vc] = grid[r][c];
                    grid[r][c] = '.';
                    moves++;
                    // Mark vacancy as used
                    used[i] = 1;
                    // Mark new position as already moved
                    moved[r][c] = 1;
                    moved[vr][vc] = 1;
                    break;  // only one move per agent
                }
            }
        }
    } 
    return moves;
}


//Display the grid and info
//This is only for print mode since infinite mode uses display.c
void display(int cycle){
    int moves = 0;
    //Check if this is the first cycle
    //If not, the grid changes!
    if(cycle != 0){
        moves = move();
    }
    int total_agents = 0;
    float sum_happiness = 0.0;
    for(int i = 0; i < dimension; i++){
        for(int j = 0; j < dimension; j++){
            printf("%c", grid[i][j]);
            if(grid[i][j] != '.'){
                sum_happiness += find_happiness(i,j);
                total_agents++;
            }
        }
        printf("\n");
    }
    //Get the average happiness
    float avg_happiness = sum_happiness/total_agents;
    printf("cycle: %d \n", cycle);
    printf("moves this cycle: %d \n", moves);
    printf("teams' \"happiness\": %f\n", avg_happiness);
    printf("dim: %d, ", dimension);
    printf("%%strength of preference: %d%%, ", strength);
    printf("%%vacancy: %d%%, ", vacancy);
    printf("%%end: %d%% \n", percent_endline);
    printf("Use Control-C to quit \n");
}


int main( int argc, char * argv[] ){
    int opt;
    while ( (opt = getopt( argc, argv, "ht:c:d:s:v:e:") ) != -1 ) {
        switch(opt){
            //Prints the help message
            case 'h':
                print_usage();
                return EXIT_SUCCESS;
            //Sets the cycle delay
            case 't':{
                int tmp_t = (int)strtol(optarg, NULL, 10);
                if(tmp_t > 0){
                    micro_delay = tmp_t;
                }else{
                    fprintf(stderr, "Delay (%d) must be a positive integer\n", tmp_t);
                    print_usage();
                    return EXIT_FAILURE;
                }
                break;
            }
            //Max count value
            case 'c':{
                int tmp_c = (int)strtol(optarg, NULL, 10);
                if(tmp_c > 0){
                    count = tmp_c;
                }else{
                    fprintf(stderr, "Count (%d) must be a positive integer\n", tmp_c);
                    print_usage();
                    return EXIT_FAILURE;
                }
                break;
            }
            //Sets the dimensions
            case 'd':{
                int tmp_d = (int)strtol(optarg, NULL, 10);
                if(tmp_d >= 5 && tmp_d <= 39){
                    dimension = tmp_d;
                }else{
                    fprintf(stderr, "Dimension (%d) must be an integer [5..39]\n",
                    tmp_d);
                    print_usage();
                    return EXIT_FAILURE;
                }
                break;
            }
            //Sets the strength of preference
            case 's':{
                int tmp_s = (int)strtol(optarg, NULL, 10);
                if(tmp_s > 0){
                    strength = tmp_s;
                }else{
                    fprintf(stderr, "Strength (%d) must be a positive integer\n", 
                    tmp_s);
                    print_usage();
                    return EXIT_FAILURE;
                }
                break;
            }
            //Sets the percent vacancies
            case 'v':{
                int tmp_v = (int)strtol(optarg, NULL, 10);
                if(tmp_v > 0 && tmp_v < 100){
                    vacancy = tmp_v;
                }else{
                    fprintf(stderr, "Vacancy (%d) must be a value in [1...99]\n",
                    tmp_v);
                    print_usage();
                    return EXIT_FAILURE;
                }
                break;
            }
            //Sets the percent of endline braces
            case 'e':{
                int tmp_e = (int)strtol(optarg, NULL, 10);
                if(tmp_e > 0){
                    percent_endline = tmp_e;
                }else{
                    fprintf(stderr, "Endline Proportions (%d) must be a positive" 
                    " integer\n", tmp_e);
                    print_usage();
                    return EXIT_FAILURE;
                }
                break;
            }
            default:
            fprintf(stderr, "brace-topia: invalid option -- '%c'", opt);
            print_usage();
            return EXIT_FAILURE;

            //Randomize it
            srandom(41);

            //Initialize grid
            initialize_grid(dimension, vacancy, percent_endline);

            //Check for infinite mode
            if(count != -1){
                for(int i = 1; i <=count; i++){
                //Print the grid and info
                display(i);
                }
            }else{
                //Infinite mode uses curor control within the display.c file
                //Clears before each new cycle
                count = 0;
                for(;;){
                    int moves = 0;
                    int sum_happiness = 0;
                    int total_agents = 0;
                    if(count != 0){
                        moves = move();
                    }
                    clear(); 
                    for(int r = 0; r <= dimension; r++){
                        set_cur_pos(r+1,1);
                        for(int c = 0; r <= dimension; c++){
                            put(grid[r][c]);
                            if(grid[r][c] != '.'){
                                sum_happiness += find_happiness(r,c);
                                total_agents++;
                                }
                        }
                    }
                    count++;
                    //First status row
                    int sr = dimension +1;
                    set_cur_pos(sr, 1);
                    float avg_happiness = sum_happiness/total_agents;
                    printf("cycle: %d \n", count);
                    printf("moves this cycle: %d \n", moves);
                    printf("teams' \"happiness\": %f\n", avg_happiness);
                    printf("dim: %d, ", dimension);
                    printf("%%strength od preference: %d%%, ", strength);
                    printf("%%vacancy: %d%%, ", vacancy);
                    printf("%%end: %d%% \n", percent_endline);
                    printf("Use Control-C to quit \n");
                    //Sleep before restarting the cycle
                    usleep(micro_delay);
                }
            }

        }
    }
}



