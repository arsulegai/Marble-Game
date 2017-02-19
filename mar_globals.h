// constants

#define OUTPUT_FILE "output.txt"
// Maximum path possible should be less than 800
#define MAX_PATH 1000

// Create enum for boolean representation

typedef enum {
    BOOL_FALSE = 0,
    BOOL_TRUE = 1,
} bool_var;

// Structure to store marbles / holes / walls / result

typedef struct {
    unsigned int row;
    unsigned int column;
    unsigned int number;
} marble_hole_loc_struct;

typedef struct {
    marble_hole_loc_struct side1;
    marble_hole_loc_struct side2;
} wall_loc_struct;

typedef struct {
    unsigned int cost;
    bool_var has_result;
    char path[MAX_PATH];
} result_struct;

// Make globals visible to all

extern int number_of_steps;
extern int max_num_steps;

// make functions visible to all

extern void lift_west(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result);

extern void lift_east(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result);

extern void lift_north(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result);

extern void lift_south(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result);

