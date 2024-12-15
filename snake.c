#include <stdio.h>
#include <stdbool.h>
#include <time.h>
#include <SDL2/SDL.h>

#define GRID_SIZE 60  // Number of rows/columns

enum 
{
    SNAKE_UP,
    SNAKE_DOWN,
    SNAKE_LEFT,
    SNAKE_RIGHT,
};

struct snake
{
    int x;
    int y;
    int dir;

    struct snake *next;
};

typedef struct snake Snake;

typedef struct {
    int x;
    int y;
} apple;
apple Apple;

Snake *head;
Snake *tail;

void init_snake()
{
    Snake *new = malloc(sizeof(Snake));
    new->x = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->y = rand() % (GRID_SIZE / 2) + (GRID_SIZE / 4);
    new->dir = SNAKE_UP;
    new->next = NULL; 

    head = new;
    tail = new;

    return;
}

void increase_snake()
{
    Snake *new = malloc(sizeof(Snake));

    switch (tail->dir)
    {
        case SNAKE_UP:
            new->x = tail->x;
            new->y = tail->y + 1;
            break;
        case SNAKE_DOWN:
            new->x = tail->x;
            new->y = tail->y - 1;
            break;
        case SNAKE_LEFT:
            new->x = tail->x + 1;
            new->y = tail->y;
            break;
        case SNAKE_RIGHT:
            new->x = tail->x - 1;
            new->y = tail->y;
            break;
    
    default:
        break;
    }

    // new->x = tail->x;
    // new->y = tail->y -1;
    new->dir = tail->dir;

    new->next = NULL;
    tail->next = new;

    tail = new;
}

void move_snake()
{
    int prev_x = head->x;
    int prev_y = head->y;
    int prev_dir = head->dir;
    
    switch (head->dir)
    {
        case SNAKE_UP:
            head->y -= 1;
            break;
        case SNAKE_DOWN:
            head->y += 1;
            break;
        case SNAKE_LEFT:
            head->x -= 1;
            break;
        case SNAKE_RIGHT:
            head->x += 1;
            break;
        default:
            break;
    }

    Snake *track = head->next;

    // if(track != NULL)
    // {
    //     track = track->next;
    // }

    while (track != NULL)
    {
        int save_x = track->x;
        int save_y = track->y;
        int save_dir = track->dir;
        
        track->x = prev_x;
        track->y = prev_y;
        track->dir = prev_dir;

        prev_x = save_x;
        prev_y = save_y;
        prev_dir = save_dir;

        track = track->next;
    }
    

    return;
}

void reset_snake ()
{
    Snake *track = head;
    Snake *temp;

    while (track != NULL)
    {
        temp = track;
        track = track->next;
        free(temp);
    }

    init_snake();
    increase_snake();
    increase_snake();

    return;
}

void render_snake(SDL_Renderer *renderer, int x, int y, int seg_size)
{
    SDL_SetRenderDrawColor(renderer, 0x00, 0xff, 0x00, 255);
    
    SDL_Rect seg;
    seg.w = seg_size;
    seg.h = seg_size;

    Snake *track = head;

    while (track != NULL)
    {
        seg.x = x + track->x * seg_size;
        seg.y = y + track->y * seg_size;

        SDL_RenderFillRect(renderer, &seg);

        track = track->next;
    }
    

    return;
}

void render_grid(SDL_Renderer *renderer, int x, int y, int cell_size, int grid_dim)
{
    SDL_SetRenderDrawColor(renderer, 0x55, 0x55, 0xff, 255);
#if 0
    SDL_Rect cell;
    cell.w = cell_size;
    cell.h = cell_size;

    for (int i = 0; i < GRID_SIZE; i++)
    {
        for (int j = 0; j < GRID_SIZE; j++)
        {
            cell.x = x + (i * cell_size);
            cell.y = y + (j * cell_size);
            SDL_RenderDrawRect(renderer, &cell);
        }
    }
#else
    SDL_Rect outline;
    outline.x = x;
    outline.y = y;
    outline.w = grid_dim;
    outline.h = grid_dim;

    SDL_RenderDrawRect(renderer, &outline);
#endif
}

void gen_apple ()
{
    bool in_snake;

    do {
        in_snake = false;
        Apple.x = rand() % GRID_SIZE;
        Apple.y = rand() % GRID_SIZE;

        Snake *track = head;

        while (track != NULL)
        {
            if (track->x == Apple.x && track->y == Apple.y)
            {
                in_snake = true;
            }
            
            track = track->next;
        }
    }
    while(in_snake);

}

void render_apple (SDL_Renderer *renderer, int x, int y, int apple_size)
{
    SDL_SetRenderDrawColor(renderer, 0xff, 0x00, 0x00, 255);
    
    SDL_Rect app;
    app.w = apple_size;
    app.h = apple_size;
    app.x = x + Apple.x * apple_size;
    app.y = y + Apple.y * apple_size;

    SDL_RenderFillRect(renderer, &app);
}

void detect_apple ()
{
    if (head->x == Apple.x && head->y == Apple.y)
    {
        gen_apple();
        increase_snake();
    }
    return;
}

void detect_crash ()
{
    if (head->x < 0 || head->x >= GRID_SIZE || head->y < 0 || head->y >= GRID_SIZE)
    {
        reset_snake();
    }

    Snake *track = head;

    if (track->next != NULL)
    {
        track = track->next;
    }

    while (track != NULL)
    {
        if (track->x == head->x && track->y == head->y)
        {
            reset_snake();
        }
        track = track->next;
    }
    

    return;
}


int main(int argc, char *argv[]) 
{
    srand(time(0));
    
    init_snake();
    increase_snake();
    increase_snake();
    
    gen_apple();
     
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) 
    {
        fprintf(stderr, "ERROR: SDL_Init: %s\n", SDL_GetError());
        return 1;
    }

    // Get desktop display mode to dynamically fetch screen resolution
    SDL_DisplayMode desktop_mode;
    if (SDL_GetDesktopDisplayMode(0, &desktop_mode) != 0) 
    {
        fprintf(stderr, "ERROR: SDL_GetDesktopDisplayMode: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    int screen_width = desktop_mode.w;
    int screen_height = desktop_mode.h;

    // Dynamically calculate window dimensions (1/3rd of screen size)
    int window_width = screen_width * .58;
    int window_height = screen_height * .8;
    int window_x = (screen_width - window_width) / 2;
    int window_y = (screen_height - window_height) / 2;

    // Create window
    SDL_Window *window = SDL_CreateWindow(
        "Responsive Snake",
        window_x,
        window_y,
        window_width,
        window_height,
        SDL_WINDOW_BORDERLESS
    );

    if (!window) 
    {
        fprintf(stderr, "ERROR: SDL_CreateWindow: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    // Create renderer
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) 
    {
        fprintf(stderr, "ERROR: SDL_CreateRenderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    // Calculate cell size and grid dimensions
    int cell_size = (window_width < window_height ? window_width : window_height) / GRID_SIZE;
    int seg_size = (window_width < window_height ? window_width : window_height) / GRID_SIZE;
    int apple_size = (window_width < window_height ? window_width : window_height) / GRID_SIZE;
    int grid_dim = cell_size * GRID_SIZE;
    int grid_x = (window_width - grid_dim) / 2;
    int grid_y = (window_height - grid_dim) / 2;

    // Main loop
    bool quit = false;
    SDL_Event event;

    while (!quit)
    {
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.sym)
                {
                    case SDLK_UP:
                        if (head->dir == SNAKE_DOWN) 
                            head->dir = SNAKE_DOWN;
                        else
                        {
                            head->dir = SNAKE_UP;
                        }
                        break;
                    case SDLK_DOWN:
                        if (head->dir == SNAKE_UP) 
                            head->dir = SNAKE_UP;
                        else
                        {
                            head->dir = SNAKE_DOWN;
                        }
                        break;
                    case SDLK_LEFT:
                        if (head->dir == SNAKE_RIGHT) 
                            head->dir = SNAKE_RIGHT;
                        else
                        {
                            head->dir = SNAKE_LEFT;
                        }
                        
                        break;
                    case SDLK_RIGHT:
                        if (head->dir == SNAKE_LEFT) 
                            head->dir = SNAKE_LEFT;
                        else
                        {
                            head->dir = SNAKE_RIGHT;
                        }
                            
                        break;
                    case SDLK_ESCAPE:
                        quit = true; // Exit the game when Esc is pressed
                        break;
                default:
                    break;
                }
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym)
                {
                    case SDLK_ESCAPE:
                        quit = true;
                        break;
                    case SDLK_UP:
                    case SDLK_w:
                        if (head->dir != SNAKE_DOWN) head->dir = SNAKE_UP;
                        break;
                    case SDLK_DOWN:
                    case SDLK_s:
                        if (head->dir != SNAKE_UP) head->dir = SNAKE_DOWN;
                        break;
                    case SDLK_LEFT:
                    case SDLK_a:
                        if (head->dir != SNAKE_RIGHT) head->dir = SNAKE_LEFT;
                        break;
                    case SDLK_RIGHT:
                    case SDLK_d:
                        if (head->dir != SNAKE_LEFT) head->dir = SNAKE_RIGHT;
                        break;
                
                default:
                    break;
                }
            
            default:
                break;
            }
        }

        move_snake();
        detect_apple();
        detect_crash();

        // Clear screen
        SDL_SetRenderDrawColor(renderer, 0x11, 0x11, 0x11, 255);
        SDL_RenderClear(renderer);

        // Render objects
        render_grid(renderer, grid_x, grid_y, cell_size, grid_dim);
        render_snake(renderer, grid_x, grid_y, seg_size);
        render_apple(renderer, grid_x, grid_y, apple_size);

        // Present the updated frame
        SDL_RenderPresent(renderer);

        SDL_Delay(100);
    }

    // Cleanup resources
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
