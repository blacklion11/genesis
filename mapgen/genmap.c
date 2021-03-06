
#include <string.h>
#include <time.h>
#include "genmap.h"



int maxHeight, minHeight;


void halp(const char *prog)
{
    printf("Usage: %s [-hwf]\n  --help\tDisplay this help screen\n  -h <number>\tSpecify the height (must be a 2^n + 1)\n  -w <number>\tSpecify the width (must be a 2^n + 1)\n  -r <number>\tSpecify the roughness of the terrain (positive number, may be a fractional decimal)\n  -f <filename>\tThe file to save the map to\n", prog);
}

void print_map(int **map, int width, int height)
{
     /* fill map with 0's */
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            printf("%d ", map[y][x]);
        }
        printf("\n");
    }
}


int** malloc_heightmap(int width, int height)
{
    /* malloc map*/
    int **map;
    map = (int **) malloc(sizeof(int *) * height);

    for(int i = 0; i < height; i++)
    {
        map[i] = (int *) malloc(sizeof(int) * width);
    }

    /* fill map with -1's */
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            map[y][x] = -1;
        }
    }


    return map;
}

/*
 * Randomizes the map with 0's and 1's
 */
void randomize(int **map, int width, int height)
{
    srand(time(NULL));
    for(int y = 0; y < height; y++)
    {
        for(int x = 0; x < width; x++)
        {
            map[y][x] = rand() % 2;
        }
    }
}


int** generate_heightmap(int **map, int width, int height, float roughness)
{
    // set the corner seeds
    srand(time(NULL));
    int seed1, seed2, seed3, seed4;
    seed1 = rand() % 5000;
    seed2 = rand() % 5000;
    seed3 = rand() % 5000;
    seed4 = rand() % 5000;

    map[0][0] = seed1;
    map[0][width - 1] = seed2;
    map[height - 1][0] = seed3;
    map[height - 1][width -1] = seed4;
    
    printf("Seeds:\n%d\t%d\n%d\t%d\n", seed1, seed2, seed3, seed4);

    /* fill center tile */
    map[(height - 1) / 2][(width - 1) / 2] = average(4, seed1, seed2, seed3, seed4);

    int length = width / 2;
    while(length > 0)
    {
    
        /* Do Diamond Step */
        for(int y = 0; y < height / 2 / length; y++)
        {
            for(int x = 0; x < width / 2 / length; x++)
            {
                //printf("length: %d\n", length);
                diamond(map, width, height, length + (x * 2 * length), length + (y * 2 * length), length, roughness);
            }
        }

        /* Do Square Step */
        for(int y = 0; y < height / 2 / length; y++)
        {
            for(int x = 0; x < width / 2 / length; x++)
            {
                if(length > 1)
                {
                    square(map, length + (x * 2 * length), length + (y * 2 * length), length, roughness);
                }
            }
        }


        /* Adjust the length left */
        if(length == 1) length = 0;
        length -= length / 2;
    }

    return map;
}

void diamond (int** map, int width, int height, int x, int y, int length, float roughness)
{
    int salt;

    // top edge
    if(y - length == 0)
    {
        salt = rand() % length;
        if(should_add())
        {
            map[y - length][x] = average(3, map[y][x], map[y - length][x - length], map[y - length][x + length]) + (salt * roughness);       
        }
        else
        {
            map[y - length][x] = average(3, map[y][x], map[y - length][x - length], map[y - length][x + length]) - (salt * roughness);
        }
    }
    // bottom edge
    if(y + length == height - 1)
    {
        salt = rand() % length;
        if(should_add())
        {
            map[y + length][x] = average(3, map[y][x], map[y + length][x - length], map[y + length][x + length]) + (salt * roughness);
        }
        else
        {
            map[y + length][x] = average(3, map[y][x], map[y + length][x - length], map[y + length][x + length]) - (salt * roughness);
        }
    }
    //internal y index
    else
    {
        salt = rand() % length;
        if(should_add())
        {
            map[y + length][x] = average(4, map[y][x], map[y + length][x - length], map[y + length][x + length], map[y + 2 * length][x]) + (salt * roughness);
        }
        else
        {
            map[y + length][x] = average(4, map[y][x], map[y + length][x - length], map[y + length][x + length], map[y + 2 * length][x]) - (salt * roughness);
        }
    }


    // left edge
    if(x - length == 0)
    {
        salt = rand() % length;
        if(should_add())
        {
            map[y][x - length] = average(3, map[y][x], map[y - length][x - length], map[y + length][x - length]) + (salt * roughness);
        }
        else
        {
            map[y][x - length] = average(3, map[y][x], map[y - length][x - length], map[y + length][x - length]) - (salt * roughness);
        }
    }
    // right edge
    if(x + length == width - 1)
    {
        salt = rand() % length;
        if(should_add())
        {
            map[y][x + length] = average(3, map[y][x], map[y - length][x + length], map[y + length][x + length]) + (salt * roughness);
        }
        else
        {
            map[y][x + length] = average(3, map[y][x], map[y - length][x + length], map[y + length][x + length]) - (salt * roughness);
        }
    }
    //internal x index
    else
    {
        salt = rand() % length;
        if(should_add())
        {
            map[y][x + length] = average(4, map[y][x], map[y - length][x - length], map[y + length][x + length], map[y][x + 2 * length]) + (salt * roughness);
        }
        else
        {
            map[y][x + length] = average(4, map[y][x], map[y - length][x - length], map[y + length][x + length], map[y][x + 2 * length]) - (salt * roughness);
        }
    }

}


void square_without_salt(int **map, int x, int y, int length)
{

    map[y - length / 2][x - length / 2] = average(4, map[y - length][x - length], map[y - length][x], map[y][x - length], map[y][x]);

    map[y - length / 2][x + length / 2] = average(4, map[y - length][x], map[y - length][x + length], map[y][x + length], map[y][x]);

    map[y + length / 2][x - length / 2] = average(4, map[y][x - length], map[y + length][x - length], map[y + length][x], map[y][x]);

    map[y + length / 2][x + length / 2] = average(4, map[y][x + length], map[y + length][x], map[y + length][x + length], map[y][x]);


}

void square(int **map, int x, int y, int length, float roughness)
{
    int salt;

    salt = rand() % length;

    if(should_add())
    {
        map[y - length / 2][x - length / 2] = average(4, map[y - length][x - length], map[y - length][x], map[y][x - length], map[y][x]) + (salt * roughness);
    }
    else
    {
        map[y - length / 2][x - length / 2] = average(4, map[y - length][x - length], map[y - length][x], map[y][x - length], map[y][x]) - (salt * roughness);
    }
    
    
    salt = rand() % length;

    if(should_add())
    {
        map[y - length / 2][x + length / 2] = average(4, map[y - length][x], map[y - length][x + length], map[y][x + length], map[y][x]) + (salt * roughness);
    }
    else
    {
        map[y - length / 2][x + length / 2] = average(4, map[y - length][x], map[y - length][x + length], map[y][x + length], map[y][x]) - (salt * roughness);
    }
   

    salt = rand() % length;

    if(should_add())
    {
        map[y + length / 2][x - length / 2] = average(4, map[y][x - length], map[y + length][x - length], map[y + length][x], map[y][x]) + (salt * roughness);
    }
    else
    {
        map[y + length / 2][x - length / 2] = average(4, map[y][x - length], map[y + length][x - length], map[y + length][x], map[y][x]) - (salt * roughness); 
    }


    salt = rand() % length;

    if(should_add())
    {
        map[y + length / 2][x + length / 2] = average(4, map[y][x + length], map[y + length][x], map[y + length][x + length], map[y][x]) + (salt * roughness);
    }
    else
    {
        map[y + length / 2][x + length / 2] = average(4, map[y][x + length], map[y + length][x], map[y + length][x + length], map[y][x]) - (salt * roughness);  
    }
}



/*
 *This function builds the blocks from the heightmap
 */
void build_blocks(struct Map* map, int** elevs)
{
    minHeight = maxHeight = elevs[0][0];

    printf("Building map blocks...\n");
    for(int y = 0; y < map->height; y++)
    {
        for(int x = 0; x < map->width; x++)
        {
            map->blocks[y][x]->elevation = elevs[y][x];
            if(elevs[y][x] < minHeight) minHeight = elevs[y][x];
            if(elevs[y][x] > maxHeight) maxHeight = elevs[y][x];
        }
    }
}

void id_blocks(struct Map* map)
{
    for(int y = 0; y < map->height; y++)
    {
        for(int x = 0; x < map->width; x++)
        {
           int range = maxHeight - minHeight;
           float percentage = ((float) map->blocks[y][x]->elevation - minHeight) / range;

           if(percentage < 1)
           {
               // mountain
               map->blocks[y][x]->id = 6;
           }
           if(percentage < 0.7f)
           {
               // hills
               map->blocks[y][x]->id = 5;
           }
           if(percentage < 0.6f)
           {
               // flatlands
               map->blocks[y][x]->id = 4;
           }
           if(percentage < 0.45f)
           {
               // beach
               map->blocks[y][x]->id = 3;
           }
           if(percentage < 0.35f)
           {
               // shallow water
               map->blocks[y][x]->id = 2;
           }
           if(percentage < 0.2f)
           {
               // Deep ocean
               map->blocks[y][x]->id = 1;
           }

        }
    }
}



int main(int argc, char **argv)
{

    if(argc < 3)
    {
        halp(argv[0]);
        return EXIT_FAILURE;
    }
    if(strcmp(argv[1], "--help") == 0)
    {
        halp(argv[0]);
        return EXIT_SUCCESS;
    }

    int c,width, height;
    float roughness;
    char filename[BUFSIZ];
    while((c = getopt(argc, argv, "h:w:f:r:")) > 0)
    {
        switch(c)
        {
            case 'h':
                height = atoi(optarg);
                break;
            case 'w':
                width = atoi(optarg);
                break;
            case 'f':
                strcpy(filename, optarg);
                break;
            case 'r':
                roughness = atof(optarg);
        }
    }

    printf("parameters parsed\n");
    printf("map is %dx%d\n", width, height);

    int **heightmap = malloc_heightmap(width, height);
    //printf("Original:\n");
    //print_map(map, width, height);
    printf("Building map...\n");
    generate_heightmap(heightmap, width, height, roughness);
    clip_heightmap(heightmap, width, height, 10000, -10000);

    struct Map* map = malloc_map(width, height);
    build_blocks(map, heightmap); 
    id_blocks(map);

    printf("Writing map to file: %s\n", filename);
    //write_heightmap(filename, map);
    write_map(filename, map);
    return EXIT_SUCCESS;
}









