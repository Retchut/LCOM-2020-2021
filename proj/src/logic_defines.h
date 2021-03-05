#ifndef _LOGIC_DEFINES_H_
#define _LOGIC_DEFINES_H_

/*
              DEFINES FOR CONTROLING OBJECTS AND INITIALIZING OBJECT INSTANCES
*/

#define SIZEOF(x)   (sizeof(x)/sizeof(*x))  /**< @brief Macro to calculate the size of x*/

#define MAX_HIGHSCORES    5       /*!< maximum number of highscores we're allowing */
#define HS_NAME_MAX_SIZE  6       /*!< size of each highscore (COUNTING THE NULL CHARACTER) */


//UI elements defines
#define UI_BAR_HEIGHT        64
#define UI_ELEM_FIRST_Y     552
#define HP_UNITS_X          160
#define AMMO_UNITS_X        384
#define POINTS_UNITS_X      736

#define ENEMY_NUMBER     66
#define ENEMIES_PER_LINE (ENEMY_NUMBER / 6)
#define ENEMY_MOVES        6    
#define START_MOVE_TIME  90
#define ENEMY_START_X    16
#define ENEMY_X_SPEED    16
// #define ENEMY_START_Y    32
#define ENEMY_START_Y    64
#define ENEMY_Y_SPEED    16
#define ENEMY_1_VALUE    20
#define ENEMY_2_VALUE    10
#define SPECIAL_VALUE   150

#define INIT_PLAYER_LIVES 3
#define PLAYER_SPEED     16
#define SHOOT_COOLDOWN   30 //60 ticks = 1 sec

#define MAX_PLAYER_PROJ  15
#define MAX_ENEMY_PROJ    5
#define PROJECTILE_SIZE  20
#define PROJECTILE_SPEED 10

#endif  /* _LOGIC_DEFINES_H_ */
