#include "level.h"

static void init(GameLevel *level, unsigned int lvlwidth, unsigned int lvlheight);

static void print_level(GameLevel *level);

GameLevel load_level(const char *path, unsigned int width, unsigned int height)
{
    GameLevel level;
    level.len = 0;

    char s[1024];
    FILE *fp = fopen(path, "r");

    if (fp == NULL)
    {
        perror("Error opening file");
        exit(1);
    }

    while (fgets(s, sizeof s, fp) != NULL)
    {
        TileRow row;
        row.len = 0;
        for (int i = 0; s[i] != '\0'; i++)
        {
            if (s[i] == ' ')
                continue;

            if (s[i] == '\n')
                break;

            long num = strtol(&s[i], NULL, 10);
            row.tiles[row.len] = num;
            row.len++;
        }
        level.rows[level.len] = row;
        level.len++;
    }

    fclose(fp);
    init(&level, width, height);
    return level;
}

void draw_level(GameLevel *level, Shader shader) {
    for(unsigned int x = 0; x < level->b_counter; ++x) {
            draw_object(&level->bricks[x], shader);
        }


}

static void init(GameLevel *level, unsigned int lvlwidth, unsigned int lvlheight)
{

    unsigned int height = level->len;
    unsigned int width = level->rows[0].len;
    float unit_width = (float)lvlwidth / (float)width;
    float unit_height = (float)lvlheight / (float)height;

    level->bricks = calloc(sizeof(Object), height * width);
    level->b_counter = 0;

    for (unsigned int y = 0; y < height; ++y)
    {
        for (unsigned int x = 0; x < width; ++x)
        {
            vec2s pos = {{unit_width * x, unit_height * y}};
            vec2s size = {{unit_width, unit_height}};

            Object obj = {

                .position = pos,
                .size = size,
                .isSolid = false,
                .rotation = 0.0f,
                .destroyed = false,
                .velocity = {{0.0f, 0.0f }},
            };
            // check block type from level data (2D level array)
            if (level->rows[y].tiles[x] == 1) // solid
            {

                obj.sprite = get_texture("block_solid");
                obj.isSolid = true;
                obj.color = (vec3s){{0.8f, 0.8f, 0.7f}};

                level->bricks[level->b_counter++] = obj;
            }
            else if (level->rows[y].tiles[x] > 1)
            {
                vec3s color = {{1.0f, 1.0f, 1.0f}}; // original: white
                if (level->rows[y].tiles[x] == 2)
                    color = (vec3s){{0.2f, 0.6f, 1.0f}};
                else if (level->rows[y].tiles[x] == 3)
                    color = (vec3s){{0.0f, 0.7f, 0.0f}};
                else if (level->rows[y].tiles[x] == 4)
                    color = (vec3s){{0.8f, 0.8f, 0.4f}};
                else if (level->rows[y].tiles[x] == 5)
                    color = (vec3s){{1.0f, 0.5f, 0.0f}};

                obj.color = color;
                obj.sprite = get_texture("block");
                level->bricks[level->b_counter++] = obj;
            }
        }
    }
}


void reset_level(GameLevel *level) {
    for (unsigned int i = 0; i < level->b_counter; ++i) {
        level->bricks[i].destroyed = false;
    }
}

static void print_level(GameLevel *level)
{
    for (int i = 0; i < level->len; i++)
    {
        printf("[ ");
        for (int j = 0; j < level->rows[i].len; j++)
        {
            printf("%d ", level->rows[i].tiles[j]);
        }
        printf("]\n");
    }
}