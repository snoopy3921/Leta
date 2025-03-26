#include "gui.h"
#include "os_kernel.h"
#include "os_task.h"
#include "main.h"
#include "math.h"
#include "gui_port.h"

static uint8_t const ICON[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0xe0, 0xc0, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0xf8, 
	0xf8, 0xf0, 0xe0, 0xc0, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x7f, 0x7f, 
	0xff, 0xff, 0xff, 0xff, 0xfe, 0xfe, 0xfc, 0xfc, 0xf9, 0xf1, 0xe3, 0xe7, 0xcf, 0x9f, 0x1e, 0xfc, 
	0xf8, 0xf0, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0xc0, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x3f, 0x3f, 0x7f, 0xff, 0xff, 0xff, 
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xf0, 0xf7, 0xff, 0xff, 0xff, 0xfc, 0xfe, 0xff, 0xff, 
	0xff, 0xff, 0x3f, 0x03, 0x03, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0xc0, 0xc0, 
	0xe0, 0xe0, 0xe0, 0xf0, 0xf0, 0xf3, 0xfb, 0xf9, 0x79, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 0x7f, 
	0x7f, 0x7f, 0x3f, 0x3f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x07, 0x07, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x0f, 0x07, 0x07, 0x03, 
	0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	/* (40 X 40 )*/
};

/* Tuning section */
static float turnfactor = 0.5;
static int16_t visual_range = 30;
static int16_t visual_range_squared = visual_range * visual_range;
static int16_t protected_range = 5;
static int16_t protected_range_squared = protected_range*protected_range;
static float centering_factor = 0.01;
static float matching_factor = 0.05;
static float avoidfactor = 0.05;
static int16_t maxspeed = 6;
static int16_t minspeed = 2;

static float predator_centering_factor = 0.02;
static int16_t predator_range = 20;
static int16_t predator_range_squared = predator_range*predator_range;
// maxbias: 0.01
// bias_increment: 0.00004
// default biasval: 0.001 (user-changeable, or updated dynamically)
/*************/

#define NUM_OF_BOIDS 90


typedef struct 
{
      int16_t x;
      int16_t y;
      float vx;
      float vy;
}boid;


static float xpos_avg, ypos_avg, xvel_avg, yvel_avg;
static uint16_t neighboring_boids;
static int16_t close_dx, close_dy;

static boid my_boids[NUM_OF_BOIDS];

static float target_xpos_avg, target_ypos_avg;
static bool use_predator = true;
static boid predator;

static uint8_t const pacman_icon[] = {
	0x30, 0xfc, 0xfe, 0xff, 0xfb, 0xf9, 0xcf, 0xcf, 0x86, 0x00, 0x00, 0x00, 0x01, 0x03, 0x03, 0x03, 
	0x03, 0x03, 0x01, 0x00
};


static gui_free_style gui_fs;

void my_free_style_draw(gui_obj *free_style_obj)
{
      for(uint16_t main_boid = 0; main_boid < NUM_OF_BOIDS; main_boid++)
      {
            gui_window_draw_pixel(&window_screen_size, my_boids[main_boid].x, my_boids[main_boid].y, DRAW_STATE_ON);
      }
      if(use_predator) gui_window_draw_bmp(&window_screen_size, predator.x , predator.y , 10, 10, pacman_icon, DRAW_STATE_ON);
}

static void setup(void * p_arg)
{
      gui_fs.create(0, 0, GUI_CFG_DISPLAY_W, GUI_CFG_DISPLAY_H, my_free_style_draw);

      // Stop the display from sleeping
      os_timer_set_period(p_timer_countdown_screen_time, 30000);
	os_timer_reset(p_timer_countdown_screen_time);

      predator.x = 30;
      predator.y = 30; 
      predator.vx = 3;
      predator.vy = 3;

      for(uint16_t main_boid = 0; main_boid < NUM_OF_BOIDS; main_boid++)
      {
            my_boids[main_boid].x = rand() % (GUI_CFG_DISPLAY_W - 0 + 1) + 0;
            my_boids[main_boid].y = rand() % (GUI_CFG_DISPLAY_H - 0 + 1) + 0;
            my_boids[main_boid].vx = rand() % (maxspeed - minspeed + 1) + minspeed;
            my_boids[main_boid].vy = rand() % (maxspeed - minspeed + 1) + minspeed;
      }

}

/**
 * Thanks to vẻy helpful resource: https://vanhunteradams.com/Pico/Animal_Movement/Boids-algorithm.html
 */
static void loop(void * p_arg)
{
    os_task_delay(5);
    
    target_xpos_avg = 0;
    target_ypos_avg = 0;
      // For every boid . . .
      for(uint16_t main_boid = 0; main_boid < NUM_OF_BOIDS; main_boid++)
      {
            // Zero all accumulator variables (can't do this in one line in C)
            xpos_avg = 0.0f;
            ypos_avg = 0.0f;
            xvel_avg = 0.0f;
            yvel_avg = 0.0f;

            neighboring_boids = 0; 
            close_dx = 0;          
            close_dy = 0;         
            
            // For every other boid in the flock . . .
            for(uint16_t other_boid = 0; other_boid < NUM_OF_BOIDS; other_boid++)
            {
                  if(main_boid == other_boid) continue;

                  //Compute differences in x and y coordinates
                  int16_t dx = my_boids[main_boid].x - my_boids[other_boid].x;
                  int16_t dy = my_boids[main_boid].y - my_boids[other_boid].y;

                  // Are both those differences less than the visual range?
                  if (abs(dx)<visual_range && abs(dy)<visual_range)
                  {
                        // If so, calculate the squared distance
                        int squared_distance = dx*dx + dy*dy;

                        // Is squared distance less than the protected range?
                        if (squared_distance < protected_range_squared)
                        {
                              // If so, calculate difference in x/y-coordinates to nearfield boid
                              close_dx += my_boids[main_boid].x - my_boids[other_boid].x;
                              close_dy += my_boids[main_boid].y - my_boids[other_boid].y;
                        }
            
                        else if (squared_distance < visual_range_squared)
                        {
                              // Add other boid's x/y-coord and x/y vel to accumulator variables
                              xpos_avg += my_boids[other_boid].x ;
                              ypos_avg += my_boids[other_boid].y ;
                              xvel_avg += my_boids[other_boid].vx ;
                              yvel_avg += my_boids[other_boid].vy ;
              
                              // Increment number of boids within visual range
                              neighboring_boids += 1 ;
                        } 
                  }
            }

            // If there were any boids in the visual range . . .            
            if (neighboring_boids > 0)
            {
                  // Divide accumulator variables by number of boids in visual range
                  xpos_avg = xpos_avg/neighboring_boids ;
                  ypos_avg = ypos_avg/neighboring_boids ;
                  xvel_avg = xvel_avg/neighboring_boids ;
                  yvel_avg = yvel_avg/neighboring_boids ;
          
                  // Add the centering/matching contributions to velocity
                  my_boids[main_boid].vx = (my_boids[main_boid].vx + 
                             (xpos_avg - my_boids[main_boid].x)*centering_factor + 
                             (xvel_avg - my_boids[main_boid].vx)*matching_factor);
          
                  my_boids[main_boid].vy = (my_boids[main_boid].vy + 
                             (ypos_avg - my_boids[main_boid].y)*centering_factor + 
                             (yvel_avg - my_boids[main_boid].vy)*matching_factor);
            }


            /** Predator section */
            if(use_predator)
            {
                  //Compute differences in x and y coordinates
                  int16_t dx = my_boids[main_boid].x - predator.x;
                  int16_t dy = my_boids[main_boid].y - predator.y;

                  if (abs(dx)<predator_range && abs(dy)<predator_range)
                  {
                        // calculate the squared distance
                        int squared_distance = dx*dx + dy*dy;

                        // Is squared distance less than the protected range?
                        if (squared_distance < predator_range_squared)
                        {
                              // If so, calculate difference in x/y-coordinates to nearfield boid
                              close_dx += dx*5;
                              close_dy += dy*5;
                        }
                  }
                  /** ***** */
            }


            // Add the avoidance contribution to velocity
            my_boids[main_boid].vx = my_boids[main_boid].vx + (close_dx*avoidfactor);
            my_boids[main_boid].vy = my_boids[main_boid].vy + (close_dy*avoidfactor);

            // If the boid is near an edge, make it turn by turnfactor
            // (this describes a box, will vary based on boundary conditions)
            if (my_boids[main_boid].y < 5) // Top
                  my_boids[main_boid].vy = my_boids[main_boid].vy + turnfactor;
            if (my_boids[main_boid].x > GUI_CFG_DISPLAY_W - 5) // Right
                  my_boids[main_boid].vx =  my_boids[main_boid].vx - turnfactor;
            if (my_boids[main_boid].x < 5) // Left
                  my_boids[main_boid].vx =  my_boids[main_boid].vx + turnfactor;
            if (my_boids[main_boid].y > GUI_CFG_DISPLAY_H - 5)
                  my_boids[main_boid].vy = my_boids[main_boid].vy - turnfactor;

            // Calculate the boid's speed
            // Slow step! Lookup the "alpha max plus beta min" algorithm
            float speed = sqrt(my_boids[main_boid].vx*my_boids[main_boid].vx + my_boids[main_boid].vy*my_boids[main_boid].vy);
            
            // Enforce min and max speeds
            if (speed < minspeed)
            {
                  my_boids[main_boid].vx = (my_boids[main_boid].vx/speed)*minspeed;
                  my_boids[main_boid].vy = (my_boids[main_boid].vy/speed)*minspeed;
            }
            if (speed > maxspeed)
            {
                  my_boids[main_boid].vx = (my_boids[main_boid].vx/speed)*maxspeed;
                  my_boids[main_boid].vy = (my_boids[main_boid].vy/speed)*maxspeed;
            }
            // Update boid's position
            my_boids[main_boid].x = my_boids[main_boid].x + my_boids[main_boid].vx;
            my_boids[main_boid].y = my_boids[main_boid].y + my_boids[main_boid].vy;


            /** Predator section */
            if(use_predator)
            {
                  target_xpos_avg += my_boids[main_boid].x;
                  target_ypos_avg += my_boids[main_boid].y;
            }
            /***** */
      }

      /** Predator section */
      if(use_predator)
      {
            target_xpos_avg = target_xpos_avg / NUM_OF_BOIDS;
            target_ypos_avg = target_ypos_avg / NUM_OF_BOIDS;

            predator.vx = predator.vx + (target_xpos_avg - predator.x)*predator_centering_factor;
            predator.vy = predator.vy + (target_ypos_avg - predator.y)*predator_centering_factor;

            // If the predator is near an edge, make it turn by turnfactor
            // (this describes a box, will vary based on boundary conditions)
            // This will also limit the speed as i didnt set speed limit
            if (predator.y < 5) // Top
                  predator.vy = predator.vy + turnfactor;
            if (predator.x > GUI_CFG_DISPLAY_W - 5) // Right
                  predator.vx =  predator.vx - turnfactor;
            if (predator.x < 5) // Left
                  predator.vx =  predator.vx + turnfactor;
            if (predator.y > GUI_CFG_DISPLAY_H - 5)
                  predator.vy = predator.vy - turnfactor;

            // Update predator's position
            predator.x = predator.x + predator.vx;
            predator.y = predator.y + predator.vy;
      }
      /***** */
}
static void event_hdler(void * p_event)
{
	msg_t *p_msg = (msg_t *)p_event;
	switch (p_msg->sig)
	{
    case SIG_BTN_DR_PRESSED:
		/* code */
		gui_fs.destroy();
		os_timer_set_period(p_timer_countdown_screen_time, config.screen_time * 1000);
		os_timer_reset(p_timer_countdown_screen_time);
		page_jump_to(PAGE_WATCH_ID);
		break;
    case SIG_BTN_UL_PRESSED:
		/* code */
            protected_range = 15;
            protected_range_squared = protected_range*protected_range;
		break;
    case SIG_BTN_DL_PRESSED:
		/* code */
            protected_range = 2;
            protected_range_squared = protected_range*protected_range;
		break;
      case SIG_BTN_UR_PRESSED:
		/* code */
            protected_range = 5;
            protected_range_squared = protected_range*protected_range;
		break;
      case SIG_BTN_UR_LONG_PRESSED:
		/* code */
            use_predator = !use_predator;
		break;
	default:
		break;
	}
}
void page_boids_reg(void)
{
	page_create(PAGE_BOIDS_ID, "Boids", ICON, setup, loop, event_hdler);
}
