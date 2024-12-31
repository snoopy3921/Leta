#include "gui.h"
#include "os_kernel.h"
#include "os_task.h"
#include "main.h"


static uint8_t const ICON[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x18, 0x3c, 0x3c, 0x18, 0x18, 0xf0, 0xf8, 0xf8, 0xfc, 0xf8, 0xf8, 0xf0, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x70, 0x78, 0x78, 0x3c, 0x1e, 0x1e, 0x0f, 0x07, 0x0f, 0xcf, 0xfe, 0xfe, 0xfe, 0xfe, 0xfc, 
	0xfc, 0x79, 0x01, 0x01, 0x01, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x80, 0x38, 
	0x7c, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc7, 0x81, 0x00, 0x1e, 0x1f, 0x3c, 0x3c, 0x3c, 0x3c, 
	0x3c, 0x3c, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1c, 0x1c, 0x3c, 0x3c, 
	0x3c, 0x3c, 0x3c, 0x3c, 0x3e, 0x1f, 0x1f, 0x0e, 0x06, 0x00, 0x01, 0x01, 0x03, 0x03, 0xc7, 0xff, 
	0xff, 0xff, 0x3e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0x1f, 0x0f, 0x07, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
	/* (40 X 40 )*/
};


static const uint8_t signGameOver[] = { // 90x20
	0x00, 0x00, 0xf0, 0xf0, 0xf8, 0x18, 0x08, 0x08, 0x78, 0x78, 0x70, 0x00, 0x00, 0xf0, 0xf8, 0xf8, 
	0x08, 0x08, 0x18, 0xf8, 0xf0, 0xf0, 0x00, 0xf8, 0xf0, 0xf0, 0xc0, 0x80, 0x80, 0xc0, 0xf0, 0xf0, 
	0xf8, 0x00, 0xf8, 0xf8, 0xf8, 0x18, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0xf0, 0xf8, 0xfc, 0x0c, 0x0c, 0x0c, 0xfc, 0xf8, 0xf0, 0x00, 0xf8, 0xf8, 0xf8, 0x00, 0x00, 
	0x00, 0xf8, 0xf8, 0xf8, 0x00, 0xf8, 0xf8, 0xf8, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0xf8, 
	0xf8, 0xf8, 0xf8, 0x08, 0x08, 0xf8, 0xf8, 0xf0, 0xe0, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80, 
	0x80, 0x82, 0xfe, 0xfe, 0x7e, 0x00, 0x00, 0xff, 0xff, 0xff, 0x0c, 0x0c, 0x1c, 0xff, 0xff, 0xff, 
	0x00, 0xff, 0xff, 0xff, 0x01, 0x03, 0x03, 0x01, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff, 0xc7, 
	0x82, 0x82, 0x82, 0x82, 0x80, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0x80, 0x80, 
	0x80, 0xff, 0xff, 0x7f, 0x00, 0x03, 0x07, 0x3f, 0xfc, 0xfc, 0xfc, 0x3f, 0x1f, 0x03, 0x00, 0xff, 
	0xff, 0xff, 0x87, 0x82, 0x82, 0x82, 0x82, 0x80, 0x00, 0xff, 0xff, 0xff, 0x07, 0x06, 0x06, 0x07, 
	0xff, 0xfd, 0xf8, 0x00, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 
	0x08, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x08, 
	0x08, 0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
	0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 
	0x08, 0x08, 0x09, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x09, 0x09, 0x09, 0x09, 
	0x09, 0x09, 0x08, 0x08, 0x09, 0x09, 0x08, 0x08, 0x08, 0x08, 0x09, 0x09, 0x08, 0x08
};

static const uint8_t imgPlayer[] = { // 20x20
    0x00, 0x00, 0x00, 0x01, 0x08, 0x00, 0x00, 0x90, 0x00, 0x00, 0xf0, 0x00, 0x00, 0xf0, 0x00, 0x00, 
    0xf0, 0x00, 0x00, 0x00, 0x00, 0x01, 0xf8, 0x00, 0x7e, 0x67, 0xe0, 0xff, 0x6f, 0xf0, 0x7e, 0xf7, 
    0xe0, 0x30, 0x00, 0xc0, 0x1c, 0xf3, 0x80, 0x08, 0x01, 0x00, 0x01, 0xf8, 0x00, 0x00, 0x00, 0x00, 
    0x01, 0xf8, 0x00, 0x00, 0xf0, 0x00, 0x00, 0xe0, 0x00, 0x00, 0x60, 0x00
};

typedef enum
{
    TURN_LEFT,
    TURN_RIGHT,
    GO_AHEAD // Default
} turn_t;


#define GAME_STATE_PLAY		(0)
#define GAME_STATE_OVER		(1)
/*  Defines  */
#define SCR_WIDTH 128
#define SCR_HEIGHT 64

#define SCR_WIDTH_CENTER SCR_WIDTH / 2
#define SCR_HEIGHT_CENTER SCR_HEIGHT / 2

#define FROM_EYES_TO_SCR 32
#define MAX_DEPTH 32 * 4

#define MAX_STAR_HIGH 128

#define MAX_COLUMN_WIDTH 24

#define STEP_X_TO_TURN 2
#define STEP_DEPTH 1

#define MAX_NUM_COLUMN 8
#define MAX_NUM_STAR 30

class column
{
private:
    /* data */
    uint8_t w;
    int16_t x;
    float depth;


public:
    column(/* args */){}
    ~column(){}

    int16_t getX() { return x; }
    void setX(int16_t val) { x = val; }
    float getDepth() { return depth; }
    void setDepth(float val) { depth = val; }

    float getScale() { return (float)(FROM_EYES_TO_SCR) / (FROM_EYES_TO_SCR + getDepth()); }

    void init()
    {
        x = (rand() % (SCR_WIDTH * 4)) - (SCR_WIDTH * 3 / 2) ;
        w = MAX_COLUMN_WIDTH;
        depth = (rand() % (MAX_DEPTH / 2)) + MAX_DEPTH / 2;
    }
    void deinit()
    {
        //cv.deinit();
    }

    void draw()
    {
        gui_window_draw_rect(&window_screen_size, SCR_WIDTH_CENTER - (SCR_WIDTH_CENTER - x) * getScale() - w / 2, (SCR_HEIGHT_CENTER - w) * getScale(), w * getScale(), w * 4 * getScale(), DRAW_STATE_ON);
        //cv.canvas_fillRoundRect(SCR_WIDTH_CENTER - (SCR_WIDTH_CENTER - x) * getScale() - w / 2, (SCR_HEIGHT_CENTER - w) * getScale(), w * getScale(), w * 4 * getScale(), 2);
        //cv.setFade(getDepth() / (MAX_DEPTH / 4));
    }
};

class star
{
private:
    /* data */

    int16_t x;
    uint8_t h;
    float depth;

public:
    star(/* args */){}
    ~star(){}

    int16_t getX() { return x; }
    void setX(int16_t val) { x = val; }
    float getDepth() { return depth; }
    void setDepth(float val) { depth = val; }
    uint8_t getH() { return h; }
    void setH(int16_t val) { h = val; }

    float getScale() { return (float)(FROM_EYES_TO_SCR) / (FROM_EYES_TO_SCR + getDepth()); }

    void init()
    {
        x = (rand() % (SCR_WIDTH * 3/4)) + SCR_WIDTH / 4;
        h = (rand() % MAX_STAR_HIGH);
        depth = rand() % MAX_DEPTH;
    }
    void deinit()
    {
        //cv.deinit();
    }

    void draw()
    {
        gui_window_draw_pixel(&window_screen_size, SCR_WIDTH_CENTER - (SCR_WIDTH_CENTER - x) * getScale(), h * getScale(), DRAW_STATE_ON);
    }
};




static column columns[MAX_NUM_COLUMN];
static star stars[MAX_NUM_STAR];
static uint32_t score;
static uint32_t game_state;

static uint32_t turn_tick;
static uint8_t game_update_rate;

static turn_t direction;


static void drawColumns(void);
static void updateColumns(turn_t turn_type);
static void initColumns(bool state);

static void drawStars(void);
static void updateStars(turn_t turn_type);
static void initStars(bool state);
static void handleGame(uint8_t state);


static void fs_draw_func(gui_obj *p_obj)
{
    drawColumns();
    drawStars();
}



static gui_bmp bmp1;
static gui_label lb1;
static gui_free_style fs1;

static void setup(void * p_arg)
{
    game_state = GAME_STATE_PLAY;

    game_update_rate = 10;
    
    score = 0;
    turn_tick = 0;
    direction = GO_AHEAD;
    

    initColumns(true);
    initStars(true);

    bmp1.create(0, 0, 90, 20, signGameOver);
    bmp1.set_allign(GUI_ALIGN_HOR_MID);
    bmp1.set_allign(GUI_ALIGN_VER_MID);
    bmp1.add_state(GUI_STATE_DISABLE);

    lb1.create(0, 0);

    fs1.create(0, 0, 128, 64, fs_draw_func);


	os_timer_set_period(p_timer_countdown_screen_time, 30000);
	os_timer_reset(p_timer_countdown_screen_time);
}


static void loop(void * p_arg)
{
    os_task_delay(game_update_rate);
    handleGame(game_state);
    updateColumns(direction);
    updateStars(direction);
    lb1.printf("%d", score);
}
static void event_hdler(void * p_event)
{
	msg_t *p_msg = (msg_t *)p_event;
	switch (p_msg->sig)
	{
    case SIG_BTN_DR_PRESSED:
		/* code */
        bmp1.destroy();
        lb1.destroy();
        fs1.destroy();
		os_timer_set_period(p_timer_countdown_screen_time, config.screen_time * 1000);
		os_timer_reset(p_timer_countdown_screen_time);
		page_jump_to(PAGE_WATCH_ID);
		break;
    case SIG_BTN_UL_PRESSED:
        direction = TURN_LEFT;
        turn_tick = 0;
        break;
    case SIG_BTN_UR_PRESSED:
        direction = TURN_RIGHT;
        turn_tick = 0;
        break;
    
	default:
		break;
	}
}
void page_runner_game_reg(void)
{
	page_create(PAGE_RUNNER_GAME_ID, "Runner game", ICON, setup, loop, event_hdler);
}



static void initColumns(bool state)
{
    column *cl = columns;
    if (state == true)
        for (int i = 0; i < MAX_NUM_COLUMN; i++)
        {
            cl->init();
            cl++;
        }
    else
        for (int i = 0; i < MAX_NUM_COLUMN; i++)
        {
            cl->deinit();
            cl++;
        }
}
static void updateColumns(turn_t turn_type)
{
    if(game_state != GAME_STATE_OVER) score += STEP_DEPTH;
    column *cl = columns;
    switch (turn_type)
    {
    case TURN_LEFT:
        for (int i = 0; i < MAX_NUM_COLUMN; i++)
        {
            cl->setDepth(cl->getDepth() - STEP_DEPTH);
            cl->setX((cl->getX() + STEP_X_TO_TURN));
            cl++;
        }
        break;
    case TURN_RIGHT:
        for (int i = 0; i < MAX_NUM_COLUMN; i++)
        {
            cl->setDepth(cl->getDepth() - STEP_DEPTH);
            cl->setX((cl->getX() - STEP_X_TO_TURN));
            cl++;
        }
        break;
    default:
        for (int i = 0; i < MAX_NUM_COLUMN; i++)
        {
            cl->setDepth(cl->getDepth() - STEP_DEPTH);
            cl++;
        }
        break;
    }
}
static void drawColumns(void)
{
    column *cl = columns;
    for (int i = 0; i < MAX_NUM_COLUMN; i++)
    {
        if (cl->getDepth() <= 0)
        {
            if (cl->getX() <= SCR_WIDTH_CENTER + (MAX_COLUMN_WIDTH / 2) && cl->getX() >= SCR_WIDTH_CENTER - (MAX_COLUMN_WIDTH / 2))
            {
                /*Slow timer*/
                game_update_rate = 100;
                game_state = GAME_STATE_OVER;
                bmp1.clear_state(GUI_STATE_DISABLE);
            }
            cl->setX((rand() % (SCR_WIDTH * 4)) - (SCR_WIDTH * 3 / 2) );
            cl->setDepth((rand() % (MAX_DEPTH / 2)) + MAX_DEPTH / 2);
        }

        cl->draw();
        cl++;
    }
}

static void drawStars(void)
{
    star *st = stars;
    for (int i = 0; i < MAX_NUM_STAR; i++)
    {
        if (st->getDepth() <= 0)
        {
            st->setX((rand() % (SCR_WIDTH * 3/4)) + SCR_WIDTH / 4);
            st->setDepth((rand() % (MAX_DEPTH / 4)) + MAX_DEPTH / 4);
        }
        st->draw();
        st++;
    }
}
static void updateStars(turn_t turn_type)
{
    star *st = stars;
    switch (turn_type)
    {
    case TURN_LEFT:
        for (int i = 0; i < MAX_NUM_STAR; i++)
        {
            st->setDepth(st->getDepth() - STEP_DEPTH);
            st->setX((st->getX() + STEP_X_TO_TURN));
            st++;
        }
        break;
    case TURN_RIGHT:
        for (int i = 0; i < MAX_NUM_STAR; i++)
        {
            st->setDepth(st->getDepth() - STEP_DEPTH);
            st->setX((st->getX() - STEP_X_TO_TURN));
            st++;
        }
        break;
    default:
        for (int i = 0; i < MAX_NUM_STAR; i++)
        {
            st->setDepth(st->getDepth() - STEP_DEPTH);
            st++;
        }
        break;
    }
}
static void initStars(bool state)
{
    star *st = stars;
    if (state == true)
        for (int i = 0; i < MAX_NUM_STAR; i++)
        {
            st->init();
            st++;
        }
    else
        for (int i = 0; i < MAX_NUM_STAR; i++)
        {
            st->deinit();
            st++;
        }
}

static void handleGame(uint8_t state)
{
    switch (state)
    {
    case GAME_STATE_PLAY:
        /* code */
        turn_tick++;
        if (turn_tick > 30)
        {
            direction = GO_AHEAD;
            turn_tick = 0;
        }
        break;
    case GAME_STATE_OVER:
        /* code */
        break;
    
    default:
        break;
    }
}