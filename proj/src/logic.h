#ifndef _LOGIC_H_
#define _LOGIC_H_

/** @defgroup logic logic
 * @{
 * 
 * Functions for handling the logic of not just the game, but the entire program as a whole
 */


/**
 * @brief Enumerated type for controling the game state
 */
typedef enum state{
  menu_open = 0,        /*!< open the menu */
  menu = 1,             /*!< while the menu is open */
  highscore_open = 2,   /*!< open the highscore screen */
  highscore = 3,        /*!< while the highscore screen is open */
  game_start = 4,       /*!< start the game */
  game_ok = 5,          /*!< while the game is running */
  game_over = 6,        /*!< the game just ended */
  new_hs = 7,           /*!< getting new highscore info */
  save_new_hs = 8,      /*!< save the new highscore*/
  close_program = 9     /*!< close the program */
} state;


/**
 * @brief Enumerated type for specifying which enemy type we're accessing on the logic functions
 */
typedef enum enemy_type{
    normal_enemy,   /*!< normal enemy */
    special_enemy   /*!< special enemy */
} enemy_type;


/**
 * @brief Enumerated type for specifying the horizontal direction an entity is moving
 */
typedef enum horizontal {
    left = -1,      /*!< moving left */
    none = 0,       /*!< not moving horizontally */
    right = 1       /*!< moving right */
} horizontal;


/*
              OBJECTS
*/


/**
 * @brief Forward define of the Menu items struct
 * @see Menu.h
 */
struct MenuItems;


/**
 * @brief Forward define of the Image struct
 * @see Menu.h
 */
struct Image;


/**
 * @brief Struct that holds the cursor's position and image
 */
typedef struct Cursor{
    int x, y;               /*!< the cursor's current position */
    struct Image *img;      /*!< the cursor's image */
} Cursor;


/**
 * @brief Struct that data related to the game's logic
 */
typedef struct logic_data{
  enum horizontal enemy_next_horizontal;    /*!< the direction the enemies are moving next */
  int enemy_moves;                          /*!< the number of modes the enemies did */
  int enemy_move_time;                      /*!< the time between each enemy's move */
  int destr_normal;                         /*!< the number destroyed normal enemies */
  enum horizontal player_next_horizontal;   /*!< the direction the player is moving next */
  int shoot_cooldown;                       /*!< the player's shot cooldown */
} logic_data;


/**
 * @brief Struct that holds the player's data
 */
typedef struct Player {
    unsigned int hp;                /*!< the player's hp */
    unsigned int points;            /*!< the player's points */
    int x, y;                       /*!< the player's current position */
    unsigned int speed;             /*!< the player's speed */
    unsigned int width, height;     /*!< the width and height */
    unsigned int shoot_x, shoot_y;  /*!< the shooting position */
    struct Image *img;              /*!< the player's image */
} Player;


/**
 * @brief Struct that holds a player projectile's data
 */
typedef struct Player_projectile{
    int *line;      /*!< pointer to the line array (PROJECTILE_SIZE * 2 elements) */
    int vx, vy;     /*!< x and y velocities of the projectile object */
} Player_projectile;


/**
 * @brief Struct that holds an enemy's data
 */
typedef struct Enemy {
    int x, y;               /*!< the enemy's current position */
    int x_speed, y_speed;   /*!< the enemy's speed */
    int width, height;      /*!< the width and height */
    unsigned int value;     /*!< the enemy's point value */
    struct Image *img_1;    /*!< the enemys 1st image */
    struct Image *img_2;    /*!< the enemys 2nd image */
    horizontal direction;   /*!< the enemy's horizontal direction */
    bool version1;          /*!< true if we're drawing img_1 next */
} Enemy;


/**
 * @brief Struct that holds a player projectile's data
 */
typedef struct Enemy_projectile{
    int x, y;               /*!< the projectile's current position */
    int speed;              /*!< y velocity of the projectile object */
    int width;              /*!< width of the projectile object */
    int height;             /*!< height of the projectile object */
    struct Image *img;      /*!< the enemy projectile's image */
} Enemy_projectile;


/*
                CURSOR GENERATION FUNCTION
*/

/**
 * @brief Generates the cursor object, with the correct values
 * 
 * @details Allocates memory for the cursor object, and initializes its parameters with the starting values
 * @return 0 on success, non-zero otherwise
 */
int generate_cursor();


/*
                OBJECT GENERATION FUNCTIONS
*/

/**
 * @brief Generates the player object, with the correct values
 * 
 * @details Allocates memory for the player object, and initializes its parameters with the starting values
 * @return 0 on success, non-zero otherwise
 */
int generate_player();

/**
 * @brief Creates an enemy object based on some parameters, dinamically alocating memory, and returns a pointer to it
 * 
 * @param is_enemy_1 True if we want to load enemy_1's image
 * @param start_x    Enemy's starting position on the x axis (first pixel to be drawn)
 * @param start_y    Enemy's starting position on the y axis (first pixel to be drawn)
 * @param vx         Enemy's velocity on the x axis (how many pixels to the right/left it moves per movement command)
 * @param vy         Enemy's velocity on the y axis (how many pixels it moves down per movement command)
 * @return Pointer to the created object on success, NULL on failure
 */
Enemy *create_enemy(bool is_enemy_1, uint16_t start_x, uint16_t start_y, uint16_t vx, uint16_t vy);

/**
 * @brief Initializes the enemy array, with the correct values for each object
 * 
 * @details Each line has 11 enemies, and there are 6 lines (66 enemies in total)
 * @return 0 on success, non-zero otherwise
 */
int generate_enemies();

/**
 * @brief   Generates all game objects to be used by the game
 * 
 * @details Calls every object generation function, save for the projectile generation one
 * @return 0 on success, non-zero otherwise
 */
int generate_game_objects();

/**
 * @brief Generates a special enemy, that gives 150 points when destroyed
 * 
 * @return 0 on success, non-zero otherwise
 */
int generate_special(horizontal direction);

/**
 * @brief Generates the Player_projectile's array of points
 * 
 * @details  Uses bresenham's algorithm, and returns the points in an array with the format [x0, y0, x1, y1, x2, y2, x3, .....] code apropriated from http://members.chello.at/~easyfilter/bresenham.html
 * @param x0 The first point's x value
 * @param y0 The first point's y value
 * @param xf The target point's x value
 * @param yf The target point's y value
 * @return  Pointer to the generated array, of size 2*PROJECTILE_SIZE
 */
int *bresenham_line_algorithm(int x0, int y0, int xf, int yf);

/**
 * @brief Creates a new Player_projectile object and initializes its values
 * 
 * @param mouse_x Target mouse position's x value
 * @param mouse_y Target mouse position's y value
 * @return Pointer to the created object on success, NULL on failure
 */
Player_projectile *create_enemy_projectile(int16_t mouse_x, int16_t mouse_y);

/**
 * @brief Creates a Player_projectile object, and adds it to player_projectiles
 * 
 * @details Saves the object in the first available position in the array of MAX_PLAYER_PROJ Player_projectile pointers. If none are available, nothing happens and the function returns 0
 * @param mouse_x Target mouse position's x value
 * @param mouse_y Target mouse position's y value
 * @return 0 on success, non-zero otherwise
 */
int add_player_projectile(int16_t mouse_x, int16_t mouse_y);

/**
 * @brief Shoots towards the current cursor positions
 * 
 * @details Calls the function add_player_projectile, which adds a Player_projectile object pointer to the player_projectiles array in the first available position of the projectiles array
 * @return 0 on success, non-zero otherwise
 */
int shoot();

/**
 * @brief Creates a new Enemy_projectile object to the enemy_projectiles
 * 
 * @details Saves the object in the first available position in the array of Enemy_projectile pointers. If none are available, nothing happens and the function returns 0
 * @param spawner_index The enemy in front of which we'll spawn the projectile
 * @return 0 on success, non-zero otherwise
 */
int add_enemy_projectile(int spawner_index);

/*
                MENU INTERACTION FUNCTIONS
*/

/**
 * @brief Checks if the player pressed the lb of the mouse on top of a button
 */
state menu_option(struct MenuItems *buttons);

/**
 * @brief Checks if the player has achieved a new highscore, and saves it, if that's the case
 * 
 * @param s Pointer to the array of scores
 * @return 0 if there's no new highscore, the new score value otherwise
 */
int check_new_highscore(int *s);
/*
                GAME MOVEMENT LOGIC FUNCTIONS
*/

/**
 * @brief Moves the player, according to the parameters passed to the function
 * 
 * @details Increments the position variables by the speed of the object, depending on the type of movement
 * @param horizontal_direction Enum that holds the direction the player is moving
 */
void move_player(horizontal direction);

/**
 * @brief Moves the cursor
 * 
 * @details IMPORTANT: rightwards is positive, upwards is positive
 * @param dx Variation on the x axis
 * @param dy Variation on the y axis
 */
void move_cursor(int16_t dx, int16_t dy);

/**
 * @brief Moves a single enemy, according to the parameters passed to the function
 * 
 * @details Increments the position variables by the speed of the object, depending on the type of movement
 * @param to_move   Pointer to the Enemy object we're moving
 * @param move_down True if we're moving the object down (altering its y position)
 * @param move_right True if we're moving the object to the right, false if we're moving it to the left
 */
void move_enemy(Enemy *to_move, bool move_down, horizontal direction);

/**
 * @brief Moves all enemies, according to the parameters passed to the function
 * 
 * @details Increments the position variables by the speed of the object, depending on the type of movement
 * @param down                Bool that signals whether or not we're moving down
 * @param direction Enum that says which direction we're moving on the horizontal axis
 */
void move_all_enemies(bool down, horizontal direction);

/**
 * @brief Moves the special enemy, according to the parameter passed to the function
 * 
 * @details Increments the position variables by the speed of the object, depending on the type of movement
 */
void move_special();

/**
 * @brief Moves a single projectile
 * 
 * @details Increments the position variables by the speeds of the object
 * @param to_move Pointer to the object we want to move
 */
void move_player_projectile(Player_projectile *to_move);

/**
 * @brief Moves a single projectile
 * 
 * @details Increments the position variables by the speeds of the object
 * @param to_move Pointer to the object we want to move
 */
void move_enemy_projectile(Enemy_projectile *to_move);

/**
 * @brief Moves all projectiles
 * 
 * @details Increments the position variables by each object's speeds
 */
void move_all_projectiles();

/*
                COLLISION AND OBJECT DESTRUCTION LOGIC FUNCTIONS
*/

/**
 * @brief Destroys one enemy object
 * 
 * @details Frees the alocated memory for the object at the index passed as parameter and changes the corresponding pointer's value to NULL
 * @param index Index on the enemies array of the object to be destroyed
 */
void destroy_enemy(int index);

/**
 * @brief Destroys the special enemy object
 * 
 * @details Frees the alocated memory for the object and changes the corresponding pointer's value to NULL
 */
void destroy_special();

/**
 * @brief Destroys the player object
 * 
 * @details Frees the alocated memory for the object and changes the corresponding pointer's value to NULL
 */
void destroy_player();

/**
 * @brief Destroys one player projectile object
 * 
 * @details Frees the alocated memory for the object at the index passed as parameter and changes the corresponding pointer's value to NULL
 * @param index Index on the projectiles array of the object to be destroyed
 */
void destroy_player_projectile(int index);

/**
 * @brief Destroys one enemy projectile object
 * 
 * @details Frees the alocated memory for the object at the index passed as parameter and changes the corresponding pointer's value to NULL
 * @param index Index on the projectiles array of the object to be destroyed
 */
void destroy_enemy_projectile(int index);

/**
 * @brief Destroys ALL projectiles, both the player's and the enemies'
 */
void destroy_all_projectiles();

/**
 * @brief Destroys ALL game entities, and frees their memory
 */
void destroy_all_game_entities();

/**
 * @brief Checks for player projectiles colisions, and destroys the objects involved
 * 
 * @details Checks for colisions between the player projectiles and enemies, and checks if the player projectiles have gone out of bounds. If those conditions are met, the player projectiles are destroyed, and so are the enemy objects, in the former case
 * @return Number of projectiles that collided with normal enemies
 */
unsigned int projectile_col();

/**
 * @brief Checks if the player directly collided with an enemy
 * 
 * @return game_over if the game has been lost, game_ok if the game can resume
 * @see enum state
 */
state check_player_enemy_col();

/**
 * @brief Checks if the player's hp dropped to 0
 * 
 * @return game_over if the player's hp is 0 or lower, 0 if the game can resume
 * @see enum state
 */
state check_player_hp();

/**
 * @brief Function that resets the game at the start of a new round
 * 
 * @details Removes projectiles from the screen and re-initializes the normal enemies
 * @return 0 on success, non-zero otherwise
 */
state reset_gamestate(logic_data *to_update);

#endif /* _LOGIC_H_ */
