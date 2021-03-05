#ifndef _VIDEO_DEFINES_H_
#define _VIDEO_DEFINES_H_

/*
            VIDEO MODE DEFINES
*/


#define MIN_X_PIXEL       0   /*!< Minimum x pixel */
#define MIN_Y_PIXEL       0   /*!< Minimum y pixel */
#define MAX_X_PIXEL     799   /*!< Maximum x pixel */
#define MAX_Y_PIXEL     599   /*!< Maximum y pixel */
#define SCREEN_RES_X    800   /*!< x screen resolution */
#define SCREEN_RES_Y    600   /*!< y screen resolution */


/*
            DRAWING DEFINES
*/

#define PROJECTILE_COLOR      0xE0D244     /*!< the color of the player projectiles */

#define CURSOR_DRAW_OFFSET    -16          /*!< offset between where we're drawing the cursor, and what its actual position is */

#define TITLE_DRAW_X          123   /*!< x pos where we start drawing all title animations */
#define TITLE1_DRAW_Y         100   /*!< y pos where we start drawing the first title animation */
#define TITLE2_DRAW_Y         80    /*!< y pos where we start drawing the second title animation */
#define HS_NAME_START_X       105   /*!< where we start drawing the highscore menu's highscore name */
#define HS_TIME_UNIT_X        445   /*!< where we start drawing the highscore menu's highscore time */
#define HS_SCORE_UNIT_X       666   /*!< where we start drawing the highscore menu's highscore score */
#define HS_DOT_1              408   /*!< x where we draw the first dot  */
#define HS_DOT_2              344   /*!< x where we draw the second dot */
#define HIGHSCORE_START_Y     159   /*!< y where we draw the dots */
#define HIGHSCORE_Y_INCREMENT 75  
#define NEW_HIGHSCORE_X       200 
#define NEW_HIGHSCORE_Y       45  
#define NEW_HS_NAME_X         316 
#define NEW_HS_NAME_Y         347 
#define NEW_HS_X_INCREMENT    37  

#endif /* _VIDEO_DEFINES_H_ */
