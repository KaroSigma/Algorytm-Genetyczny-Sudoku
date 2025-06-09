#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define SIZE 9
#define SUBGRID_SIZE 3
#define POPULATION_SIZE 1000
#define MAX_GENERATIONS 100000
#define MUTATION_RATE 0.5
#define CROSSOVER_RATE 0.7
#define ELITE_SIZE 50

void initialize_population(int population[][SIZE][SIZE], int board[][SIZE]) 
{
    for (int i = 0; i < POPULATION_SIZE; i++) 
    {
        for (int row = 0; row < SIZE; row++) 
        {
            int used[SIZE + 1] = {0};
            int available[SIZE], count = 0;
            
            // Mark fixed numbers as used
            for (int col = 0; col < SIZE; col++) 
            {
                if (board[row][col] != 0) 
                {
                    used[board[row][col]] = 1;
                }
            }
            
            // Collect available numbers
            for (int number = 1; number <= SIZE; number++) 
            {
                if (!used[number]) {
                    available[count++] = number;
                }
            }
            
            // Shuffle available numbers
            for (int j = 0; j < count; j++) 
            {
                int r = rand() % count;
                int temp = available[j];
                available[j] = available[r];
                available[r] = temp;
            }
            
            // Fill empty cells
            count = 0;
            for (int col = 0; col < SIZE; col++) 
            {
                if (board[row][col] == 0) {
                    population[i][row][col] = available[count++];
                } else {
                    population[i][row][col] = board[row][col];
                }
            }
        }
    }
}

int fitness(int individual[][SIZE]) {
    int errors = 0;

    //Check rows
    for (int i = 0; i < SIZE; i++) 
    {
        int count[SIZE + 1] = {0};
        for (int j = 0; j < SIZE; j++) 
        {
            count[individual[i][j]]++;
        }
        for (int number = 1; number <= SIZE; number++) 
        {
            if (count[number] > 1) {
                errors += count[number] - 1;
            }
        }
    }

    //Check columns
    for (int j = 0; j < SIZE; j++) 
    {
        int count[SIZE + 1] = {0};
        for (int i = 0; i < SIZE; i++) 
        {
            count[individual[i][j]]++;
        }
        for (int number = 1; number <= SIZE; number++) 
        {
            if (count[number] > 1) 
            {
                errors += count[number] - 1;
            }
        }
    }

    //Check 3x3
    for (int start_i = 0; start_i < SIZE; start_i += SUBGRID_SIZE) 
    {
        for (int start_j = 0; start_j < SIZE; start_j += SUBGRID_SIZE) 
        {
            int count[SIZE + 1] = {0};
            for (int i = start_i; i < start_i + SUBGRID_SIZE; i++) 
            {
                for (int j = start_j; j < start_j + SUBGRID_SIZE; j++) 
                {
                    count[individual[i][j]]++;
                }
            }
            for (int number = 1; number <= SIZE; number++) 
            {
                if (count[number] > 1) {
                    errors += count[number] - 1;
                }
            }
        }
    }

    return -errors;  // Negative value
}

int tournament(int fitness_scores[]) 
{
    int idx1 = rand() % POPULATION_SIZE;
    int idx2 = rand() % POPULATION_SIZE;
    if (fitness_scores[idx1] > fitness_scores[idx2]) 
    {
        return idx1;
    }
    return idx2;
}

void crossover(int parent1[][SIZE], int parent2[][SIZE], int child[][SIZE]) 
{
    for (int row = 0; row < SIZE; row++) 
    {
        if ((double)rand() / RAND_MAX < CROSSOVER_RATE) 
        {
            // Take row from parent2
            for (int col = 0; col < SIZE; col++) 
            {
                child[row][col] = parent2[row][col];
            }
        } 
        else 
        {
            // Take row from parent1
            for (int col = 0; col < SIZE; col++) 
            {
                child[row][col] = parent1[row][col];
            }
        }
    }
}

void mutate(int individual[][SIZE], int board[][SIZE]) 
{
    for (int row = 0; row < SIZE; row++) {
        if ((double)rand() / RAND_MAX < MUTATION_RATE) 
        {
            int mutable_cols[SIZE], count = 0;
            
            // Collect mutable positions
            for (int col = 0; col < SIZE; col++) 
            {
                if (board[row][col] == 0) 
                {
                    mutable_cols[count++] = col;
                }
            }
            
            // Perform 1-3 swaps in the row
            if (count >= 2) 
            {
                int swaps = 1 + rand() % 3;
                for (int s = 0; s < swaps; s++) 
                {
                    int col1 = rand() % count;
                    int col2;
                    do 
                    {
                        col2 = rand() % count;
                    }while (col2 == col1);
                    
                    // Swap values
                    int temp = individual[row][mutable_cols[col1]];
                    individual[row][mutable_cols[col1]] = individual[row][mutable_cols[col2]];
                    individual[row][mutable_cols[col2]] = temp;
                }
            }
        }
    }
}

void print_board(int board[][SIZE]) 
{
    for (int i = 0; i < SIZE; i++) 
    {
        for (int j = 0; j < SIZE; j++) 
        {
            printf("%d ",board[i][j]);
        }
        printf("\n");
    }
}

//Sort indices based on values (descending)
void sort_indexes(int in[], int values[], int n) 
{
    for (int i = 0; i < n; i++) 
    {
        for (int j = i + 1; j < n; j++)
        {
            if (values[in[i]] < values[in[j]])
            {
                int temp = in[i];
                in[i] = in[j];
                in[j] = temp;
            }
        }
    }
}

int main() {
    srand(time(0));
    
    //Board for change in the future
    int board[SIZE][SIZE] = {
        {5, 3, 0, 0, 7, 0, 0, 0, 0},
        {6, 0, 0, 1, 9, 5, 0, 0, 0},
        {0, 9, 8, 0, 0, 0, 0, 6, 0},
        {8, 0, 0, 0, 6, 0, 0, 0, 3},
        {4, 0, 0, 8, 0, 3, 0, 0, 1},
        {7, 0, 0, 0, 2, 0, 0, 0, 6},
        {0, 6, 0, 0, 0, 0, 2, 8, 0},
        {0, 0, 0, 4, 1, 9, 0, 0, 5},
        {0, 0, 0, 0, 8, 0, 0, 7, 9}
    };

    int population[POPULATION_SIZE][SIZE][SIZE];
    initialize_population(population, board);

    int fitness_scores[POPULATION_SIZE];
    int best_fitness = -1000000;
    int best_individual[SIZE][SIZE];
    int generation = 0;

    while (generation < MAX_GENERATIONS) {
        //Evaluate population
        for (int i = 0; i < POPULATION_SIZE; i++) 
        {
            fitness_scores[i] = fitness(population[i]);
            
            //Check for solution
            if (fitness_scores[i] == 0) 
            {
                printf("Solution found in generation %d:\n", generation);
                print_board(population[i]);
                
                return 0;
            }
            
            //Update best individual
            if (fitness_scores[i] > best_fitness) 
            {
                best_fitness = fitness_scores[i];
                memcpy(best_individual, population[i], sizeof(int) * SIZE * SIZE);
            }
        }

        //Select elite (top individuals)
        int elite_indices[ELITE_SIZE];
        for (int i = 0; i < ELITE_SIZE; i++) {
            elite_indices[i] = i;
        }
        sort_indexes(elite_indices, fitness_scores, ELITE_SIZE);
        
        //Maintain elite in population
        for (int i = ELITE_SIZE; i < POPULATION_SIZE; i++) {
            if (fitness_scores[i] > fitness_scores[elite_indices[ELITE_SIZE - 1]]) 
            {
                elite_indices[ELITE_SIZE - 1] = i;
                sort_indexes(elite_indices, fitness_scores, ELITE_SIZE);
            }
        }

        int new_population[POPULATION_SIZE][SIZE][SIZE];
        
        //Preserve elite
        for (int e = 0; e < ELITE_SIZE; e++) {
            memcpy(new_population[e], population[elite_indices[e]], sizeof(int) * SIZE * SIZE);
        }

        //Create new individuals using crossover and mutation
        for (int i = ELITE_SIZE; i < POPULATION_SIZE; i++) {
            int parent1_idx = tournament(fitness_scores);
            int parent2_idx = tournament(fitness_scores);
            crossover(population[parent1_idx], population[parent2_idx], new_population[i]);
            mutate(new_population[i], board);
        }

        //Update population
        for (int i = 0; i < POPULATION_SIZE; i++) 
        {
            memcpy(population[i], new_population[i], sizeof(int) * SIZE * SIZE);
        }

        //Progress report
        if (generation % 100 == 0) 
        {
            printf("Generation %d, best fitness: %d\n", generation, best_fitness);
        }
        generation++;
    }

    printf("No solution found after %d generations. Best solution (fitness: %d):\n", MAX_GENERATIONS, best_fitness);
    print_board(best_individual);
    
    return 0;
}