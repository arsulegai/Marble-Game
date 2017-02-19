#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "mar_globals.h"

// Return node number of row, column box
unsigned int get_current(unsigned int row, unsigned int column, unsigned int *size)
{

#ifdef __DEBUG_TRACE__
  //  printf("\n Found node : %u", (row * *size) + column);
#endif

    return ((row * *size) + column);
}

// Returns node number of left box
unsigned int get_left(unsigned int row, unsigned int column, unsigned int *size, wall_loc_struct *walls, unsigned int *W)
{
    unsigned int idx;
    if(column == 0)
    {
        return get_current(row, column, size);
    }
    for(idx = 0; idx < *W; idx++)
    {
        if(row == walls[idx].side1.row && row == walls[idx].side2.row &&
            ((walls[idx].side1.column == column && walls[idx].side2.column < column) ||
            (walls[idx].side2.column == column && walls[idx].side1.column < column)))
        {
            return get_current(row, column, size);
        }
    }
    return get_current(row, column - 1, size);
}

// Returns node number of right box
unsigned int get_right(unsigned int row, unsigned int column, unsigned int *size, wall_loc_struct *walls, unsigned int *W)
{
    unsigned int idx;
    if(column == (*size - 1))
    {
        return get_current(row, column, size);
    }
    for(idx = 0; idx < *W; idx++)
    {
        if(row == walls[idx].side1.row && row == walls[idx].side2.row &&
            ((walls[idx].side1.column == column && walls[idx].side2.column > column) ||
            (walls[idx].side2.column == column && walls[idx].side1.column > column)))
        {
            return get_current(row, column, size);
        }
    }
    return get_current(row, column + 1, size);
}

// Returns node number of top box
unsigned int get_top(unsigned int row, unsigned int column, unsigned int *size, wall_loc_struct *walls, unsigned int *W)
{
    unsigned int idx;
    if(row == 0)
    {
        return get_current(row, column, size);
    }
    for(idx = 0; idx < *W; idx++)
    {
        if(column == walls[idx].side1.column && column == walls[idx].side2.column &&
            ((walls[idx].side1.row == row && walls[idx].side2.row < row) ||
            (walls[idx].side2.row == row && walls[idx].side1.row < row)))
        {
            return get_current(row, column, size);
        }
    }
    return get_current(row - 1, column, size);
}

// Returns node number of bottom box
unsigned int get_bottom(unsigned int row, unsigned int column, unsigned int *size, wall_loc_struct *walls, unsigned int *W)
{
    unsigned int idx;
    if(row == (*size - 1))
    {
        return get_current(row, column, size);
    }
    for(idx = 0; idx < *W; idx++)
    {
        if(column == walls[idx].side1.column && column == walls[idx].side2.column &&
            ((walls[idx].side1.row == row && walls[idx].side2.row > row) ||
            (walls[idx].side2.row == row && walls[idx].side1.row > row)))
        {
            return get_current(row, column, size);
        }
    }
    return get_current(row + 1, column, size);
}

// check connectivty for row / column represented node
bool_var check_if_connected(unsigned int idx_1, unsigned int idx_2, unsigned int *N, wall_loc_struct *walls, unsigned int *W)
{
    unsigned int row = idx_1 / *N;
    unsigned int column = idx_1 % *N;
    unsigned int left, right, top, bottom;
    left = get_left(row, column, N, walls, W);
    right = get_right(row, column, N, walls, W);
    top = get_top(row, column, N, walls, W);
    bottom = get_bottom(row, column, N, walls, W);
    if(left == idx_2 || right == idx_2 || top == idx_2 || bottom == idx_2)
    {
        return BOOL_TRUE;
    }
    return BOOL_FALSE;
}

// check for all nodes connectivity
void find_reachability(unsigned int *reachable[], unsigned int *N, wall_loc_struct *walls, unsigned int *W)
{
    unsigned int idx_1, idx_2, idx_3;
    bool_var result;

#ifdef __DEBUG_TRACE__
    // printf("\n Result %u", (*N * *N));
#endif

    for(idx_1 = 0; idx_1 < (*N * *N); idx_1++)
    {
        for(idx_2 = 0; idx_2 < (*N * *N); idx_2++)
        {

#ifdef __DEBUG_TRACE__
       //     printf("\n Found for node \n");
#endif
            reachable[idx_1][idx_2] = check_if_connected(idx_1, idx_2, N, walls, W);
        }
    }

    for(idx_1 = 0; idx_1 < (*N * *N); idx_1++)
    {
        for(idx_2 = 0; idx_2 < (*N * *N); idx_2++)
        {
            for(idx_3 = 0; idx_3 < (*N * *N); idx_3++)
            {
                reachable[idx_2][idx_3] |= (reachable[idx_2][idx_1] & reachable[idx_1][idx_3]);
            }
        }
    }

#ifdef __DEBUG_TRACE__
    printf("\n Found reachability matrix");
#endif

    for(idx_1 = 0; idx_1 < (*N * *N); idx_1++)
    {
        for(idx_2 = 0; idx_2 < (*N * *N); idx_2++)
        {
#ifdef __DEBUG_TRACE__
            printf("\n idx 1: %u, idx 2: %u, %u", idx_1, idx_2, reachable[idx_1][idx_2]);
#endif
        }
    }

}

// Check if hole can be reached by a marble with support of any of wall
bool_var is_any_wall_in_row_or_column(unsigned int row, unsigned int column, 
    wall_loc_struct *walls, unsigned int *W, unsigned int *N)
{
    unsigned int idx;

    if(row == 0 || column == 0 || row == (*N - 1) || column == (*N - 1))
    {
        // hole is supported by a wall
        return BOOL_TRUE;
    }

    idx = 0;
    while(idx < (*W))
   	{
        if(walls[idx].side1.row == row || walls[idx].side1.column == column ||
            walls[idx].side2.row == row || walls[idx].side2.column == column)
        {
            // There's supporting wall through which hole can be reached
            return BOOL_TRUE;
        }
        idx++;
    }

    // There's hole randomly located in middle of nowhere
    return BOOL_FALSE;
}

// Check if input state can really be tried recursively tried for finding solution
bool_var find_basic_cases(bool_var **reachable, unsigned int *N,
    marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes, unsigned int *M, 
    wall_loc_struct *walls, unsigned int *W)
{
    unsigned int idx = 0;
    while(idx < (*M))
    {
        // Marble cannot reach hole at all, even on assuming intermediate holes are closed
        if(BOOL_FALSE == reachable[get_current(marbles[idx].row, marbles[idx].column, N)]
            [get_current(holes[idx].row, holes[idx].column, N)])
        {
            return BOOL_FALSE;
        }
        idx++;
    }

    // A hole needs to be supported by a wall (can be board's wall), because marbles need support of wall
    idx = 0;
    while(idx < (*M))
    {
        if(BOOL_FALSE == is_any_wall_in_row_or_column(holes[idx].row, holes[idx].column, walls, W, N))
        {
            return BOOL_FALSE;
        }
        idx++;
    }
    // Try out recursively, best of luck
    return BOOL_TRUE;
}

// Initialize result structure
void initialize_results(result_struct *result)
{
    result->cost = 0;
    result->has_result = BOOL_FALSE;
    result->path[0] = '\0';
}

// Get new location if moving East
void get_move_east_loc(marble_hole_loc_struct *marble, 
    marble_hole_loc_struct *marbles, unsigned int *new_num, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N,
    marble_hole_loc_struct *new_location, bool_var *wrong, bool_var *placed)
{
    unsigned int idx2, min;

    new_location->row = marble->row;
    new_location->column = marble->column;
    new_location->number = marble->number;

    *wrong = BOOL_FALSE;
    *placed = BOOL_FALSE;

    if(new_location->column == (*N - 1))
    {
        return;
    }

    {
        min = (*N - 1);

        for(idx2 = 0; idx2 < *new_num; idx2++)
        {
            if(marbles[idx2].row == marble->row &&
                marbles[idx2].column > marble->column)
            {
                if(marbles[idx2].column <= min)
                {
                    min = (marbles[idx2].column - 1);
                }
            }
        }

        for(idx2 = 0; idx2 < *W; idx2++)
        {
            if((walls[idx2].side1.row - walls[idx2].side2.row) != 0)
            {
                // this wall is horizontal
                continue;
            }
            if(walls[idx2].side1.row == marble->row)
            {
                if(walls[idx2].side1.column > marble->column && 
                    walls[idx2].side2.column >= marble->column &&
                    walls[idx2].side1.column > walls[idx2].side2.column)
                {
                    if(walls[idx2].side2.column < min)
                    {
                        min = walls[idx2].side2.column;
                    }
                }
                if(walls[idx2].side2.column > marble->column && 
                    walls[idx2].side1.column >= marble->column &&
                    walls[idx2].side2.column > walls[idx2].side1.column)
                {
                    if(walls[idx2].side1.column < min)
                    {
                        min = walls[idx2].side1.column;
                    }
                }
            }
        }

        // If there's no hole in between marble and found wall (min)
        for(idx2 = 0; idx2 < *M; idx2++)
        {
            if(holes[idx2].row == marble->row &&
                holes[idx2].column > marble->column)
            {
                if(holes[idx2].column <= min)
                {
                    min = holes[idx2].column;
                    if(marble->number != holes[idx2].number)
                    {
                        *wrong = BOOL_TRUE;
                        // If not return here, then can use new_location
//                        return;
                    }
                    else
                    {
                        *placed = BOOL_TRUE;
                    }
                }
            }
        }
        if(min > new_location->column)
        {
            new_location->column = min;
        }
    }
    return;
}

// Check if can move to East, when lifting west
bool_var can_move_east(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes, 
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, unsigned int *num_of_placed)
{
    marble_hole_loc_struct temp;
    unsigned int idx1, column_idx, new_num, t, copy_m;
    bool_var wrong, placed, can_move = BOOL_FALSE;
    marble_hole_loc_struct *temp_marbles = NULL, *copy_holes = NULL;

    column_idx = (*N - 1);
    new_num = 0;
    copy_m = *M;
    temp_marbles = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    for(t = 0; t < *M; t++)
    {
        copy_holes[t] = holes[t];
    }

    while(column_idx >= 0)
    {
        for(idx1 = 0; idx1 < *M; idx1++)
        {
            wrong = BOOL_FALSE;
            placed = BOOL_FALSE;
            if(marbles[idx1].column == column_idx)
            {
                get_move_east_loc(&marbles[idx1], temp_marbles, &new_num, holes, M, walls, W, N, &temp, &wrong, &placed);
        
        #ifdef __DEBUG_TRACE__
                printf("\n Marble: %u tried to move to %u %u from %u %u", idx1+1, temp.row, temp.column, marbles[idx1].row, marbles[idx1].column);
        #endif
        
                if(BOOL_TRUE == placed)
                {
        #ifdef __DEBUG_TRACE__
                    printf("\n Placed %u", idx1+1);
        #endif
                    (*num_of_placed)++;
            		for(t = idx1; t < (copy_m - 1); t++)
            		{
            			copy_holes[t] = copy_holes[t+1];
            		}
            		copy_m--;

                }
                else
               	{
                    temp_marbles[new_num] = temp;
                    new_num++;

                }
        
                // Check if marble can be moved, and check if does not fall to wrong hole
                if(BOOL_TRUE == wrong)
                {
        #ifdef __DEBUG_TRACE__
                    printf("\n Wrong %u", idx1+1);
        #endif
                    return BOOL_FALSE;
                }
                // Check if marble is moved
                if(get_current(temp.row, temp.column, N) != get_current(marbles[idx1].row, marbles[idx1].column, N))
                {
                    can_move = BOOL_TRUE;
                }
            }
        }
        if(column_idx == 0)
        {
            break;
        }
        column_idx--;
    }

    free(temp_marbles);
    free(copy_holes);
#ifdef __DEBUG_TRACE__
    printf("\n Can move East: %d, Placed %d, Number moved: %u", can_move, placed, *num_of_placed);
#endif
    return can_move;
}

// Check if can move west, if moved calculate cost
void lift_west(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result)
{
    unsigned int idx = 0, count_of_placed_balls = 0, cur_step, column_idx, t, copy_m;
    bool_var safe_discard, placed;

    marble_hole_loc_struct *new_marbles = NULL, *new_holes = NULL, *copy_holes = NULL;
    marble_hole_loc_struct temp;
    unsigned int new_num_marbles, new_idx;

    result_struct result_north, result_south, result_east, result_west;

#ifdef __DEBUG_TRACE__
    printf("\n Number of Steps: %d, Number of Marbles: %u, Maximum: %u", number_of_steps, *M, max_num_steps);
#endif

    if(number_of_steps >= max_num_steps || *M == 0)
    {
        number_of_steps --;
        return;
    }

    number_of_steps++;

    if(BOOL_FALSE == can_move_east(marbles, holes, M, walls, W, N, &count_of_placed_balls))
    {
        number_of_steps --;
        return;
    }

    if(count_of_placed_balls > 0)
    {
        new_num_marbles = (*M - count_of_placed_balls);
    }
    else
    {
        new_num_marbles = *M;
    }

    // Add "N" to the path in the result
    strcat(result->path, "W");
    result->cost++;

    if(new_num_marbles == 0)
    {
        result->has_result = BOOL_TRUE;
        if(result->cost < max_num_steps)
        {
            max_num_steps = number_of_steps;
#ifdef __DEBUG_TRACE__
            printf("\n\n Cost: %d, Max Steps: %d\n", result->cost, max_num_steps);
#endif
        }
        number_of_steps --;
        return;
    }
    new_marbles = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    new_holes = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    if(NULL == new_marbles)
    {
        number_of_steps --;
        // Couldn't allocate memory
        return;
    }
    if(NULL == new_holes)
    {
        number_of_steps--;
        free(new_marbles);
        return;
    }

    initialize_results(&result_north);
    initialize_results(&result_south);
    initialize_results(&result_east);
    initialize_results(&result_west);

    for(column_idx = 0; column_idx < *M; column_idx++)
    {
        copy_holes[column_idx] = holes[column_idx];
    }

    new_idx = 0;
    column_idx = (*N - 1);
    copy_m = *M;
    while(column_idx >= 0)
    {
        // Create local copy of moved marble locations for possible different paths
        for(idx = 0; idx < *M; idx++)
        {
            if(marbles[idx].column == column_idx)
            {
                placed = BOOL_FALSE;
                // Using this only for purpose of getting temp
                get_move_east_loc(&marbles[idx], new_marbles, &new_idx, copy_holes, &copy_m, walls, W, N, &temp, &safe_discard, &placed);
                if(BOOL_FALSE == placed)
                {
                    new_marbles[new_idx] = temp;
                    new_holes[new_idx] = holes[idx];
                    new_idx++;
                }
                else
                {
                    for(t = idx; t < (copy_m - 1); idx++)
                    {
                        copy_holes[t] = copy_holes[t+1];
                    }
                    copy_m--;
               	}
            }
        }
        if(column_idx == 0)
        {
            break;
        }
        column_idx--;
    }
	
    // Try lifting Noth / South
    cur_step = number_of_steps;
#ifdef __DEBUG_TRACE__
    printf("\n Then lift North");
#endif
    lift_north(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_north);
    number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
    printf("\n OR then lift South");
#endif
    lift_south(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_south);

    // If can place a marble in hole, then can move one more east step as well, but move only if one more marble in row / column
    if(new_num_marbles != *M)
    {
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift East");
#endif
        lift_east(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_east);
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift West");
#endif
        lift_west(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_west);
    } 

            if(BOOL_TRUE == result_north.has_result && 
                (BOOL_FALSE == result_east.has_result ||
                result_north.cost <= result_east.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_north.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_north.cost <= result_west.cost))
            {
                // Lifting north yielded good path
                result->cost += result_north.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_north.path);
            }
            else if(BOOL_TRUE == result_east.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_east.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_east.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_east.cost <= result_west.cost))
            {
                // Lifting east yielded good path
                result->cost += result_east.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_east.path);
            }
            else if(BOOL_TRUE == result_south.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_south.cost <= result_north.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_south.cost <= result_east.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_south.cost <= result_west.cost))
            {
                // Lifting south yielded good path
                result->cost += result_south.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_south.path);
            }
            else if(BOOL_TRUE == result_west.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_west.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_west.cost <= result_south.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_west.cost <= result_east.cost))
            {
                // Lifting west yielded good path
                result->cost += result_west.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_west.path);
            }

    number_of_steps = cur_step;
    free(new_marbles);
    free(new_holes);
    free(copy_holes);
    number_of_steps--;
    return;
}

// Get new location if moving South
void get_move_west_loc(marble_hole_loc_struct *marble, 
    marble_hole_loc_struct *marbles, unsigned int *new_num, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N,
    marble_hole_loc_struct *new_location, bool_var *wrong, bool_var *placed)
{
    unsigned int idx2, min;

    new_location->row = marble->row;
    new_location->column = marble->column;
    new_location->number = marble->number;

    *wrong = BOOL_FALSE;
    *placed = BOOL_FALSE;

    if(new_location->column == 0)
    {
        return;
    }

    {
        min = 0;

        for(idx2 = 0; idx2 < *new_num; idx2++)
        {
            if(marbles[idx2].row == marble->row &&
                marbles[idx2].column < marble->column)
            {
                if(marbles[idx2].column >= min)
                {
                    min = (marbles[idx2].column + 1);
                }
            }
        }

        for(idx2 = 0; idx2 < *W; idx2++)
        {
            if((walls[idx2].side1.row - walls[idx2].side2.row) != 0)
            {
                // this wall is horizontal
                continue;
            }
            if(walls[idx2].side1.row == marble->row)
            {
                if(walls[idx2].side1.column < marble->column &&
                    walls[idx2].side2.column <= marble->column &&
                    walls[idx2].side1.column < walls[idx2].side2.column)
                {
                    if(walls[idx2].side2.column > min)
                    {
                        min = walls[idx2].side2.column;
                    }
                }
                if(walls[idx2].side2.column < marble->column && 
                    walls[idx2].side1.column <= marble->column &&
                    walls[idx2].side2.column < walls[idx2].side1.column)
                {
                    if(walls[idx2].side1.column > min)
                    {
                        min = walls[idx2].side1.column;
                    }
                }
            }
        }

        // If there's no hole in between marble and found wall (min)
        for(idx2 = 0; idx2 < *M; idx2++)
        {
            if(holes[idx2].row == marble->row &&
                holes[idx2].column < marble->column)
            {
                if(holes[idx2].column >= min)
                {
                    min = holes[idx2].column;
                    if(marble->number != holes[idx2].number)
                    {
                        *wrong = BOOL_TRUE;
                        // If not return here, then can use new_location
//                        return;
                    }
                    else
                    {
                        *placed = BOOL_TRUE;
                    }
                }
            }
        }
        if(min < new_location->column)
        {
            new_location->column = min;
        }
    }
    return;
}

// Check if can move to South, when lifting north
bool_var can_move_west(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes, 
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, unsigned int *num_of_placed)
{
    marble_hole_loc_struct temp;
    unsigned int idx1, column_idx, new_num, copy_m, t;
    bool_var wrong, placed, can_move = BOOL_FALSE;
    marble_hole_loc_struct *temp_marbles = NULL, *copy_holes = NULL;

    column_idx = 0;
    new_num = 0;
    copy_m = *M;
    temp_marbles = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    for(t = 0; t < *M; t++)
    {
        copy_holes[t] = holes[t];
    }

    while(column_idx < *N)
    {
        for(idx1 = 0; idx1 < *M; idx1++)
        {
            wrong = BOOL_FALSE;
            placed = BOOL_FALSE;
            if(marbles[idx1].column == column_idx)
            {
                get_move_west_loc(&marbles[idx1], temp_marbles, &new_num, copy_holes, &copy_m, walls, W, N, &temp, &wrong, &placed);
        
        #ifdef __DEBUG_TRACE__
                printf("\n Marble: %u tried to move to %u %u from %u %u", idx1+1, temp.row, temp.column, marbles[idx1].row, marbles[idx1].column);
        #endif
        
                if(BOOL_TRUE == placed)
                {
        #ifdef __DEBUG_TRACE__
                    printf("\n Placed %u, Number %u", idx1+1, (*num_of_placed + 1));
        #endif
                    (*num_of_placed)++;
                    for(t = idx1; t < (copy_m - 1); t++)
                    {
                        copy_holes[t] = copy_holes[t+1];
                    }
                    copy_m--;		
                }
                else
               	{
                    temp_marbles[new_num] = temp;
                    new_num++;
                }
        
                // Check if marble can be moved, and check if does not fall to wrong hole
                if(BOOL_TRUE == wrong)
                {
        #ifdef __DEBUG_TRACE__
                    printf("\n Wrong %u", idx1+1);
        #endif
                    return BOOL_FALSE;
                }
                // Check if marble is moved
                if(get_current(temp.row, temp.column, N) != get_current(marbles[idx1].row, marbles[idx1].column, N))
                {
                    can_move = BOOL_TRUE;
                }
            }
        }
        column_idx++;
    }

    free(temp_marbles);
    free(copy_holes);
#ifdef __DEBUG_TRACE__
    printf("\n Can move West: %d, Placed %d, Number moved: %u", can_move, placed, *num_of_placed);
#endif
    return can_move;
}

// Check if can move east, if moved calculate cost
void lift_east(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result)
{
    unsigned int idx = 0, count_of_placed_balls = 0, cur_step, column_idx, t, copy_m;
    bool_var safe_discard, placed;

    marble_hole_loc_struct *new_marbles = NULL, *new_holes = NULL, *copy_holes = NULL;
    marble_hole_loc_struct temp;
    unsigned int new_num_marbles, new_idx;

    result_struct result_north, result_south, result_east, result_west;

    if(number_of_steps >= max_num_steps || *M == 0)
    {
        number_of_steps --;
        return;
    }

    number_of_steps++;

    if(BOOL_FALSE == can_move_west(marbles, holes, M, walls, W, N, &count_of_placed_balls))
    {
        number_of_steps --;
        return;
    }

    if(count_of_placed_balls > 0)
    {
        new_num_marbles = (*M - count_of_placed_balls);
    }
    else
    {
        new_num_marbles = *M;
    }

    // Add "E" to the path in the result
    strcat(result->path, "E");
    result->cost++;

    if(new_num_marbles == 0)
    {
        result->has_result = BOOL_TRUE;
        if(result->cost < max_num_steps)
        {
            max_num_steps = number_of_steps;
#ifdef __DEBUG_TRACE__
            printf("\n\n Cost: %d, Max Steps: %d\n", result->cost, max_num_steps);
#endif
        }
        number_of_steps --;
        return;
    }
    new_marbles = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    new_holes = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    if(NULL == new_marbles)
    {
        number_of_steps --;
        // Couldn't allocate memory
        return;
    }
    if(NULL == new_holes)
    {
        number_of_steps--;
        free(new_marbles);
        return;
    }

    initialize_results(&result_north);
    initialize_results(&result_south);
    initialize_results(&result_east);
    initialize_results(&result_west);

    for(column_idx = 0; column_idx < *M; column_idx++)
    {
        copy_holes[column_idx] = holes[column_idx];
    }

    column_idx = 0;
    new_idx = 0;
    copy_m = *M;
    while(column_idx < *N)
    {
        // Create local copy of moved marble locations for possible different paths
        for(idx = 0; idx < *M; idx++)
        {
            if(marbles[idx].column == column_idx)
            {
                placed = BOOL_FALSE;
                // Using this only for purpose of getting temp
                get_move_west_loc(&marbles[idx], new_marbles, &new_idx, copy_holes, &copy_m, walls, W, N, &temp, &safe_discard, &placed);
                if(BOOL_FALSE == placed)
                {
                    new_marbles[new_idx] = temp;
                    new_holes[new_idx] = holes[idx];
                    new_idx++;
#ifdef __DEBUG_TRACE__
                    printf("\n Added marble %u %u as %u %u", marbles[idx].row, marbles[idx].column, new_marbles[new_idx - 1].row, new_marbles[new_idx - 1].column);
#endif
                }
                else
                {
                    for(t = idx; t < (copy_m - 1); t++)
                    {
                        copy_holes[t] = copy_holes[t+1];
                    }
                    copy_m--;
                }
            }
        }
        column_idx++;
    }
	
    // Try lifting Noth / South
    cur_step = number_of_steps;
#ifdef __DEBUG_TRACE__
    printf("\n Then lift North");
#endif
    lift_north(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_north);
    number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
    printf("\n OR then lift South");
#endif
    lift_south(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_south);

    // If can place a marble in hole, then can move one more east step as well, but move only if one more marble in row / column
    if(new_num_marbles != *M)
    {
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift East");
#endif
        lift_east(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_east);
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift West");
#endif
        lift_west(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_west);
    } 

            if(BOOL_TRUE == result_north.has_result && 
                (BOOL_FALSE == result_east.has_result ||
                result_north.cost <= result_east.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_north.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_north.cost <= result_west.cost))
            {
                // Lifting north yielded good path
                result->cost += result_north.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_north.path);
            }
            else if(BOOL_TRUE == result_east.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_east.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_east.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_east.cost <= result_west.cost))
            {
                // Lifting east yielded good path
                result->cost += result_east.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_east.path);
            }
            else if(BOOL_TRUE == result_south.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_south.cost <= result_north.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_south.cost <= result_east.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_south.cost <= result_west.cost))
            {
                // Lifting south yielded good path
                result->cost += result_south.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_south.path);
            }
            else if(BOOL_TRUE == result_west.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_west.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_west.cost <= result_south.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_west.cost <= result_east.cost))
            {
                // Lifting west yielded good path
                result->cost += result_west.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_west.path);
            }

    number_of_steps = cur_step;
    free(new_marbles);
    free(new_holes);
    free(copy_holes);
    number_of_steps--;
    return;
}

// Get new location if moving South
void get_move_south_loc(marble_hole_loc_struct *marble, 
    marble_hole_loc_struct *marbles, unsigned int *new_num, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N,
    marble_hole_loc_struct *new_location, bool_var *wrong, bool_var *placed)
{
    unsigned int idx2, max;

    new_location->row = marble->row;
    new_location->column = marble->column;
    new_location->number = marble->number;

    *wrong = BOOL_FALSE;
    *placed = BOOL_FALSE;

    if(new_location->row == (*N-1))
    {
        return;
    }

    {
        max = (*N - 1);

#ifdef __DEBUG_TRACE__
        printf("\n %u %u %u", *new_num, marbles[0].row, marbles[0].column);
#endif

        for(idx2 = 0; idx2 < *new_num; idx2++)
        {
            if(marbles[idx2].column == marble->column &&
                marbles[idx2].row > marble->row)
            {
                if(marbles[idx2].row <= max)
                {
                    max = (marbles[idx2].row - 1);
#ifdef __DEBUG_TRACE__
                    printf("\n Marble is blocking this %u %u", marble->row, marble->column);
#endif
                }
            }
        }

        for(idx2 = 0; idx2 < *W; idx2++)
        {
            if((walls[idx2].side1.column - walls[idx2].side2.column) != 0)
            {
                // this wall is vertical
                continue;
            }
            if(walls[idx2].side1.column == marble->column)
            {
                if(walls[idx2].side1.row > marble->row && 
                    walls[idx2].side2.row >= marble->row &&
                    walls[idx2].side1.row > walls[idx2].side2.row)
                {
                    if(walls[idx2].side2.row < max)
                    {
                        max = walls[idx2].side2.row;
                    }
                }
                if(walls[idx2].side2.row > marble->row && 
                    walls[idx2].side1.row >= marble->row &&
                    walls[idx2].side2.row > walls[idx2].side1.row)
                {
                    if(walls[idx2].side1.row < max)
                    {
                        max = walls[idx2].side1.row;
                    }
                }
            }
        }

        // If there's no hole in between marble and found wall (max)
        for(idx2 = 0; idx2 < *M; idx2++)
        {
            if(holes[idx2].column == marble->column &&
                holes[idx2].row > marble->row)
            {
                if(holes[idx2].row <= max)
                {
                    max = holes[idx2].row;
                    if(marble->number != holes[idx2].number)
                    {
                        *wrong = BOOL_TRUE;
                        // If not return here, then can use new_location
//                        return;
#ifdef __DEBUG_TRACE__
                        printf("\n Marble: r - %u c - %u n - %u, Hole: r - %u c - %u n - %u", marble->row, marble->column, marble->number, holes[idx2].row, holes[idx2].column, holes[idx2].number);
#endif
                    }
                    else
                    {
                        *placed = BOOL_TRUE;
                    }
                }
            }
        }
        if(max > new_location->row)
        {
            new_location->row = max;
        }
    }
    return;
}

// Check if can move to South, when lifting north
bool_var can_move_south(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes, 
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, unsigned int *num_of_placed)
{
    marble_hole_loc_struct temp;
    unsigned int idx1, row_idx, new_num, copy_m, t;
    bool_var wrong, placed, can_move = BOOL_FALSE;
    marble_hole_loc_struct *temp_marbles = NULL, *copy_holes = NULL;

    row_idx = (*N - 1);
    new_num = 0;
    copy_m = *M;
    temp_marbles = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    for(t = 0; t < *M; t++)
    {
        copy_holes[t] = holes[t];
    }

    while(row_idx >= 0)
    {
        for(idx1 = 0; idx1 < *M; idx1++)
        {
            wrong = BOOL_FALSE;
            placed = BOOL_FALSE;
            if(marbles[idx1].row == row_idx)
            {
                get_move_south_loc(&marbles[idx1], temp_marbles, &new_num, copy_holes, &copy_m, walls, W, N, &temp, &wrong, &placed);

#ifdef __DEBUG_TRACE__
                printf("\n Marble: %u tried to move to %u %u from %u %u", idx1+1, temp.row, temp.column, marbles[idx1].row, marbles[idx1].column);
#endif

                if(BOOL_TRUE == placed)
                {
#ifdef __DEBUG_TRACE__
                    printf("\n Placed %u", idx1+1);
#endif
                    (*num_of_placed)++;
     				for(t = idx1; t < (copy_m - 1); t++)
     				{
     					copy_holes[t] = copy_holes[t+1];
     				}
     				copy_m--;

                }
                else
               	{
                    temp_marbles[new_num] = temp;
                    new_num++;

                }

                // Check if marble can be moved, and check if does not fall to wrong hole
                if(BOOL_TRUE == wrong)
                {
#ifdef __DEBUG_TRACE__
                    printf("\n Wrong %u", idx1+1);
#endif
                    return BOOL_FALSE;
                }
                // Check if marble is moved
                if(get_current(temp.row, temp.column, N) != get_current(marbles[idx1].row, marbles[idx1].column, N))
                {
                    can_move = BOOL_TRUE;
                }
            }
        }
        if(row_idx == 0)
        {
            break;
        }
        row_idx--;
    }
    free(temp_marbles);
    free(copy_holes);

#ifdef __DEBUG_TRACE__
    printf("\n Can move South: %d, Placed %d, Number moved: %u", can_move, placed, *num_of_placed);
#endif
    return can_move;
}

// Check if can move north, if moved calculate cost
void lift_north(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result)
{
    unsigned int idx = 0, count_of_placed_balls = 0, cur_step, row_idx, copy_m, t;
    bool_var safe_discard, placed;

    marble_hole_loc_struct *new_marbles = NULL, *new_holes = NULL, *copy_holes = NULL;
    marble_hole_loc_struct temp;
    unsigned int new_num_marbles, new_idx;

    result_struct result_north, result_south, result_east, result_west;

    if(number_of_steps >= max_num_steps || *M == 0)
    {
        number_of_steps --;
        return;
    }

    number_of_steps++;

    if(BOOL_FALSE == can_move_south(marbles, holes, M, walls, W, N, &count_of_placed_balls))
    {
        number_of_steps --;
        return;
    }

    if(count_of_placed_balls > 0)
    {
        new_num_marbles = (*M - count_of_placed_balls);
    }
    else
    {
        new_num_marbles = *M;
    }

    // Add "N" to the path in the result
    strcat(result->path, "N");
    result->cost++;

    if(new_num_marbles == 0)
    {
        result->has_result = BOOL_TRUE;
        if(result->cost < max_num_steps)
        {
            max_num_steps = number_of_steps;
        }
#ifdef __DEBUG_TRACE__
        printf("\n\n Cost: %d, Max Steps: %d\n", result->cost, max_num_steps);
#endif
        number_of_steps --;
        return;
    }
    new_marbles = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    new_holes = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    if(NULL == new_marbles)
    {
        number_of_steps --;
        // Couldn't allocate memory
        return;
    }
    if(NULL == new_holes)
    {
        number_of_steps--;
        free(new_marbles);
        return;
    }

    initialize_results(&result_north);
    initialize_results(&result_south);
    initialize_results(&result_east);
    initialize_results(&result_west);

    for(row_idx = 0; row_idx < *M; row_idx++)
    {
        copy_holes[row_idx] = holes[row_idx];
    }

    row_idx = (*N - 1);
    new_idx = 0;
    copy_m = *M;
    while(row_idx >= 0)
    {
        // Create local copy of moved marble locations for possible different paths
        for(idx = 0; idx < *M; idx++)
        {
            if(marbles[idx].row == row_idx)
            {
                placed = BOOL_FALSE;
                // Using this only for purpose of getting temp
                get_move_south_loc(&marbles[idx], new_marbles, &new_idx, copy_holes, &copy_m, walls, W, N, &temp, &safe_discard, &placed);
                if(BOOL_FALSE == placed)
                {
                    new_marbles[new_idx] = temp;
                    new_holes[new_idx] = holes[idx];
                    new_idx++;
                }
                else
                {
                    for(t = row_idx; t < (copy_m - 1); t++)
                    {
                        copy_holes[t] = copy_holes[t+1];
                    }
                    copy_m--;
                }
            }
        }
        if(row_idx == 0)
        {
            break;
        }
        row_idx--;
    }

#ifdef __DEBUG_TRACE__
    printf("\n New number of marbles : %d", new_num_marbles);
#endif
	
    // Try lifting East / West
    cur_step = number_of_steps;
#ifdef __DEBUG_TRACE__
    printf("\n Then lift East");
#endif
    lift_east(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_east);
    number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
    printf("\n OR then lift West, %d is the count", number_of_steps);
#endif
    lift_west(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_west);

    // If can place a marble in hole, then can move one more north step as well, but move only if one more marble in row / column
    if(new_num_marbles != *M)
    {
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift North");
#endif
        lift_north(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_north);
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift South");
#endif
        lift_south(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_south);
    } 

            if(BOOL_TRUE == result_north.has_result && 
                (BOOL_FALSE == result_east.has_result ||
                result_north.cost <= result_east.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_north.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_north.cost <= result_west.cost))
            {
                // Lifting north yielded good path
                result->cost += result_north.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_north.path);
            }
            else if(BOOL_TRUE == result_east.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_east.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_east.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_east.cost <= result_west.cost))
            {
                // Lifting east yielded good path
                result->cost += result_east.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_east.path);
            }
            else if(BOOL_TRUE == result_south.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_south.cost <= result_north.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_south.cost <= result_east.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_south.cost <= result_west.cost))
            {
                // Lifting south yielded good path
                result->cost += result_south.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_south.path);
            }
            else if(BOOL_TRUE == result_west.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_west.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_west.cost <= result_south.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_west.cost <= result_east.cost))
            {
                // Lifting west yielded good path
                result->cost += result_west.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_west.path);
            }

    number_of_steps = cur_step;
    free(new_marbles);
    free(new_holes);
    free(copy_holes);
    number_of_steps--;
    return;
}

// Get new location if moving South
void get_move_north_loc(marble_hole_loc_struct *marble, 
    marble_hole_loc_struct *marbles, unsigned int *new_num, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N,
    marble_hole_loc_struct *new_location, bool_var *wrong, bool_var *placed)
{
    unsigned int idx2, max;

    new_location->row = marble->row;
    new_location->column = marble->column;
    new_location->number = marble->number;

    *wrong = BOOL_FALSE;
    *placed = BOOL_FALSE;

    if(new_location->row == 0)
    {
        return;
    }

    {
        max = 0;

        for(idx2 = 0; idx2 < *new_num; idx2++)
        {
            if(marbles[idx2].column == marble->column &&
                marbles[idx2].row < marble->row)
            {
                if(marbles[idx2].row >= max)
                {
                    max = (marbles[idx2].row + 1);
                }
            }
        }

        for(idx2 = 0; idx2 < *W; idx2++)
        {
            if((walls[idx2].side1.column - walls[idx2].side2.column) != 0)
            {
                // this wall is vertical
                continue;
            }
            if(walls[idx2].side1.column == marble->column)
            {
                if(walls[idx2].side1.row < marble->row && 
                    walls[idx2].side2.row <= marble->row &&
                    walls[idx2].side1.row < walls[idx2].side2.row)
                {
                    if(walls[idx2].side2.row > max)
                    {
                        max = walls[idx2].side2.row;
                    }
                }
                if(walls[idx2].side2.row < marble->row && 
                    walls[idx2].side1.row <= marble->row &&
                    walls[idx2].side2.row < walls[idx2].side1.row)
                {
                    if(walls[idx2].side1.row > max)
                    {
                        max = walls[idx2].side1.row;
                    }
                }
            }
        }

        // If there's no hole in between marble and found wall (max)
        for(idx2 = 0; idx2 < *M; idx2++)
        {
            if(holes[idx2].column == marble->column &&
                holes[idx2].row < marble->row)
            {
                if(holes[idx2].row >= max)
                {
                    max = holes[idx2].row;
                    if(marble->number != holes[idx2].number)
                    {
                        *wrong = BOOL_TRUE;
                        // If not return here, then can use new_location
 //                       return;
                    }
                    else
                    {
                        *placed = BOOL_TRUE;
                    }
                }
            }
        }
        if(max < new_location->row)
        {
            new_location->row = max;
        }
    }
    return;
}

// Check if can move to South, when lifting north
bool_var can_move_north(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes, 
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, unsigned int *num_of_placed)
{
    marble_hole_loc_struct temp;
    unsigned int idx1, row_idx, new_num, copy_m, t;
    bool_var wrong, placed, can_move = BOOL_FALSE;
    marble_hole_loc_struct *temp_marbles = NULL, *copy_holes = NULL;

    row_idx = 0;
    new_num = 0;
    copy_m = *M;
    temp_marbles = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    for(t = 0; t < *M; t++)
    {
        copy_holes[t] = holes[t];
    }
    while(row_idx < *N)
    {
        for(idx1 = 0; idx1 < *M; idx1++)
        {
            wrong = BOOL_FALSE;
            placed = BOOL_FALSE;
            if(marbles[idx1].row == row_idx)
            {
                get_move_north_loc(&marbles[idx1], temp_marbles, &new_num, copy_holes, &copy_m, walls, W, N, &temp, &wrong, &placed);
        
        #ifdef __DEBUG_TRACE__
                printf("\n Marble: %u tried to move to %u %u from %u %u", idx1+1, temp.row, temp.column, marbles[idx1].row, marbles[idx1].column);
        #endif
        
                if(BOOL_TRUE == placed)
                {
        #ifdef __DEBUG_TRACE__
                    printf("\n Placed %u", idx1+1);
        #endif
                    (*num_of_placed)++;
            		for(t = idx1; t < (copy_m - 1); t++)
            		{
            			copy_holes[t] = copy_holes[t+1];
            		}
            		copy_m--;

                }
                else
                {
                    temp_marbles[new_num] = temp;
                    new_num++;
                }
        
                // Check if marble can be moved, and check if does not fall to wrong hole
                if(BOOL_TRUE == wrong)
                {
        #ifdef __DEBUG_TRACE__
                    printf("\n Wrong %u", idx1+1);
        #endif
                    return BOOL_FALSE;
                }
                // Check if marble is moved
                if(get_current(temp.row, temp.column, N) != get_current(marbles[idx1].row, marbles[idx1].column, N))
                {
                    can_move = BOOL_TRUE;
                }
            }
        }
        row_idx++;
    }
    free(temp_marbles);
    free(copy_holes);

#ifdef __DEBUG_TRACE__
    printf("\n Can move North: %d, Placed %d, Number moved: %u", can_move, placed, *num_of_placed);
#endif
    return can_move;
}

// Check if can move north, if moved calculate cost
void lift_south(marble_hole_loc_struct *marbles, marble_hole_loc_struct *holes,
    unsigned int *M, wall_loc_struct *walls, unsigned int *W, unsigned int *N, result_struct *result)
{
    unsigned int idx = 0, count_of_placed_balls = 0, cur_step, row_idx, t, copy_m;
    bool_var safe_discard, placed;

    marble_hole_loc_struct *new_marbles = NULL, *new_holes = NULL, *copy_holes = NULL;
    marble_hole_loc_struct temp;
    unsigned int new_num_marbles, new_idx;

    result_struct result_north, result_south, result_east, result_west;

    if(number_of_steps >= max_num_steps || *M == 0)
    {
        number_of_steps --;
        return;
    }

    number_of_steps++;

    if(BOOL_FALSE == can_move_north(marbles, holes, M, walls, W, N, &count_of_placed_balls))
    {
        number_of_steps --;
        return;
    }

    if(count_of_placed_balls > 0)
    {
        new_num_marbles = (*M - count_of_placed_balls);
    }
    else
    {
        new_num_marbles = *M;
    }

    // Add "N" to the path in the result
    strcat(result->path, "S");
    result->cost++;

 
    if(new_num_marbles == 0)
    {
        result->has_result = BOOL_TRUE;
        if(result->cost < max_num_steps)
        {
            max_num_steps = number_of_steps;
#ifdef __DEBUG_TRACE__
            printf("\n\n Cost: %d, Max Steps: %d\n", result->cost, max_num_steps);
#endif
        }
        number_of_steps --;
        return;
    }

    new_marbles = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    new_holes = (marble_hole_loc_struct *)malloc(new_num_marbles * sizeof(marble_hole_loc_struct));
    copy_holes = (marble_hole_loc_struct *)malloc(*M * sizeof(marble_hole_loc_struct));
    if(NULL == new_marbles)
    {
        number_of_steps --;
        // Couldn't allocate memory
        return;
    }
    if(NULL == new_holes)
    {
        number_of_steps--;
        free(new_marbles);
        return;
    }

    initialize_results(&result_north);
    initialize_results(&result_south);
    initialize_results(&result_east);
    initialize_results(&result_west);

    for(row_idx = 0; row_idx < *M; row_idx++)
    {
        copy_holes[row_idx] = holes[row_idx];
    }

    row_idx = 0;
    new_idx = 0;
    copy_m = *M;
    while(row_idx < *N)
    {
        // Create local copy of moved marble locations for possible different paths
        for(idx = 0; idx < *M; idx++)
        {
            if(row_idx == marbles[idx].row)
            {
                // Using this only for purpose of getting temp
                get_move_north_loc(&marbles[idx], new_marbles, &new_idx, copy_holes, &copy_m, walls, W, N, &temp, &safe_discard, &placed);
                if(BOOL_FALSE == placed)
                {
                    new_marbles[new_idx] = temp;
                    new_marbles[new_idx].number = marbles[idx].number;
                    new_holes[new_idx] = holes[idx];
                    new_idx++;
                }
                else
                {
                    for(t = idx; t < (copy_m-1); t++)
                    {
                        copy_holes[t] = copy_holes[t+1];
                    }
                    copy_m--;
                }
            }
        }
        row_idx++;
    }
	
    // Try lifting East / West
    cur_step = number_of_steps;
#ifdef __DEBUG_TRACE__
    printf("\n Then lift East");
#endif
    lift_east(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_east);
    number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
    printf("\n OR then lift West");
#endif
    lift_west(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_west);

    // If can place a marble in hole, then can move one more south step as well, but move only if one more marble in row / column
    if(new_num_marbles != *M)
    {
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift North");
#endif
        lift_north(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_north);
        number_of_steps = cur_step;
#ifdef __DEBUG_TRACE__
        printf("\n OR then lift South");
#endif
        lift_south(new_marbles, new_holes, &new_num_marbles, walls, W, N, &result_south);
    } 

            if(BOOL_TRUE == result_north.has_result && 
                (BOOL_FALSE == result_east.has_result ||
                result_north.cost <= result_east.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_north.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_north.cost <= result_west.cost))
            {
                // Lifting north yielded good path
                result->cost += result_north.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_north.path);
            }
            else if(BOOL_TRUE == result_east.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_east.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_east.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_east.cost <= result_west.cost))
            {
                // Lifting east yielded good path
                result->cost += result_east.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_east.path);
            }
            else if(BOOL_TRUE == result_south.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_south.cost <= result_north.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_south.cost <= result_east.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_south.cost <= result_west.cost))
            {
                // Lifting south yielded good path
                result->cost += result_south.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_south.path);
            }
            else if(BOOL_TRUE == result_west.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_west.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_west.cost <= result_south.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_west.cost <= result_east.cost))
            {
                // Lifting west yielded good path
                result->cost += result_west.cost;
                result->has_result = BOOL_TRUE;
                strcat(result->path, result_west.path);
            }

    number_of_steps = cur_step;
    free(new_marbles);
    free(new_holes);
    free(copy_holes);
    number_of_steps--;
    return;
}

// Start from here
int main(int argc, char *argv[])
{
    FILE * fp = NULL, * op = NULL;
    unsigned int N, W, M;
    bool_var **reachable = NULL;
    unsigned int idx, case_number = 1;
    marble_hole_loc_struct *marbles = NULL;
    marble_hole_loc_struct *holes = NULL;
    wall_loc_struct *walls = NULL;
    result_struct result_north, result_east, result_south, result_west;

    if(argc != 2)
    {
        printf("\n Enter valid input file");
        return BOOL_TRUE;
    }

    fp = fopen(argv[1], "r");
    if(NULL == fp)
    {
        printf("\n Fatal! Unable to open the input file");
        return BOOL_TRUE;
    }
    op = fopen(OUTPUT_FILE, "w");
    if(NULL == op)
    {
        printf("\n Fatal! Unable to open writable file, please check for %s file", OUTPUT_FILE);
    }	

    fscanf(fp, "%u%u%u",&N,&M,&W);

#ifdef __DEBUG_TRACE__
    printf("\n User command done");
#endif

    while(N != 0 || W != 0 || M != 0)
    {
        // Visualize each box on board as a node
        reachable = (bool_var **)malloc(N * N * sizeof(bool_var *));
        for(idx = 0; idx < (N * N); idx++)
        {
#ifdef __DEBUG_TRACE__
            printf("\n idx : %u", idx);
#endif
            reachable[idx] = NULL;
            reachable[idx] = (bool_var *)malloc(N * N * sizeof(bool_var));
#ifdef __DEBUG_TRACE__
            if(reachable[idx] == NULL)
            {
                printf("\n Memory issue", idx);
            }
#endif
        }

        marbles = (marble_hole_loc_struct *)malloc(M *sizeof(marble_hole_loc_struct));
        for(idx = 0; idx < M; idx++)
        {
            fscanf(fp, "%u%u", &marbles[idx].row, &marbles[idx].column);
            marbles[idx].number = idx;
#ifdef __DEBUG_TRACE__
            printf("\n Marble %u is at %u %u", marbles[idx].number, marbles[idx].row, marbles[idx].column);
#endif
        }
        holes = (marble_hole_loc_struct *)malloc(M * sizeof(marble_hole_loc_struct));
        for(idx = 0; idx < M; idx++)
        {
            fscanf(fp, "%u%u", &holes[idx].row, &holes[idx].column);
            holes[idx].number = idx;
        }
        walls = (wall_loc_struct *)malloc(W * sizeof(wall_loc_struct));
        for(idx = 0; idx < W; idx++)
        {
            fscanf(fp, "%u%u%u%u", &walls[idx].side1.row, &walls[idx].side1.column, 
                &walls[idx].side2.row, &walls[idx].side2.column);
        }

#ifdef __DEBUG_TRACE__
        printf("\n Read first contents %u %u %u", N, M, W);
#endif

        if(reachable == NULL || marbles == NULL || walls == NULL || holes == NULL)
        {
            printf("\n Insufficient memory");
        }

        find_reachability(reachable, &N, walls, &W);

#ifdef __DEBUG_TRACE__
        printf("\n Filtered basic IO");
#endif

        // Intension is to remove all abnormal wall, hole inputs
        if(BOOL_TRUE == find_basic_cases(reachable, &N, marbles, holes, &M, walls, &W))
        {

#ifdef __DEBUG_TRACE__
            printf("\n Decided to find the path");
#endif
            /* There's possibility that a marble can move to hole
             * Still there's chance that marble can be moving infinitely in loop
             */
            initialize_results(&result_north);
            initialize_results(&result_east);
            initialize_results(&result_south);
            initialize_results(&result_west);
            max_num_steps = (N * N);
            number_of_steps = 0;
#ifdef __DEBUG_TRACE__
            printf("\n Try lifting North");
#endif
            lift_north(marbles, holes, &M, walls, &W, &N, &result_north);
            number_of_steps = 0;
#ifdef __DEBUG_TRACE__
            printf("\n Try lifting East");
#endif
            lift_east(marbles, holes, &M, walls, &W, &N, &result_east);
            number_of_steps = 0;
#ifdef __DEBUG_TRACE__
            printf("\n Try lifting South");
#endif
            lift_south(marbles, holes, &M, walls, &W, &N, &result_south);
            number_of_steps = 0;
#ifdef __DEBUG_TRACE__
            printf("\n Try lifting West");
#endif
            lift_west(marbles, holes, &M, walls, &W, &N, &result_west);

#ifdef __DEBUG_TRACE__
            printf("\n Done trying all possibilities N: %d, E: %d, S: %d, W: %d", result_north.has_result, result_east.has_result, result_south.has_result, result_west.has_result);
#endif

            if(BOOL_TRUE == result_north.has_result && 
                (BOOL_FALSE == result_east.has_result ||
                result_north.cost <= result_east.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_north.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_north.cost <= result_west.cost))
            {
                // Lifting north yielded good path
                fprintf(op, "case %u: %u moves %s\n", case_number, result_north.cost, result_north.path);
            }
            else if(BOOL_TRUE == result_east.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_east.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_east.cost <= result_south.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_east.cost <= result_west.cost))
            {
                // Lifting east yielded good path
                fprintf(op, "case %u: %u moves %s\n", case_number, result_east.cost, result_east.path);
            }
            else if(BOOL_TRUE == result_south.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_south.cost <= result_north.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_south.cost <= result_east.cost) && 
                (BOOL_FALSE == result_west.has_result ||
                result_south.cost <= result_west.cost))
            {
                // Lifting south yielded good path
                fprintf(op, "case %u: %u moves %s\n", case_number, result_south.cost, result_south.path);
            }
            else if(BOOL_TRUE == result_west.has_result && 
                (BOOL_FALSE == result_north.has_result ||
                result_west.cost <= result_north.cost) && 
                (BOOL_FALSE == result_south.has_result ||
                result_west.cost <= result_south.cost) && 
                (BOOL_FALSE == result_east.has_result ||
                result_west.cost <= result_east.cost))
            {
                // Lifting west yielded good path
                fprintf(op, "case %u: %u moves %s\n", case_number, result_west.cost, result_west.path);
            }
            else
            {
#ifdef __DEBUG_TRACE__
                printf("\n No Result");
#endif
                // No result possible moving anyway
                fprintf(op, "case %u: impossible\n", case_number);
#ifdef __DEBUG_TRACE__
                printf("\n Writing done to file");
#endif
            }
        }
        else
        {
            fprintf(op, "case %u: impossible\n", case_number);
        }


        for(idx = 0; idx < (N * N); idx++)
        {
            free(reachable[idx]);
            reachable[idx] = NULL;
        }

        free(reachable);
        reachable = NULL;
        free(marbles);
        marbles = NULL;
        free(holes);
        holes = NULL;
        free(walls);
        walls = NULL;

#ifdef __DEBUG_TRACE__
        printf("\n Need to read, next input");
#endif

        // Check if there's one more board to solve
        fscanf(fp, "%u%u%u", &N, &M, &W);
        case_number++;
    }

    fclose(fp);
    fclose(op);

    return BOOL_FALSE;
}
