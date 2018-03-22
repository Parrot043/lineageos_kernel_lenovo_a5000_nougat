#ifndef BUILD_LK
#include <linux/string.h>
#endif

#include "lcm_drv.h"

#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
	#include <platform/mt_i2c.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
	#include <mach/mt_gpio.h>
	#include <linux/i2c.h>
#endif


// ---------------------------------------------------------------------------
// Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH (720)
#define FRAME_HEIGHT (1280)

#define REGFLAG_DELAY 0XFD
#define REGFLAG_END_OF_TABLE 0xFE // END OF REGISTERS MARKER

#define LCM_DSI_CMD_MODE 0   
#define LCM_ID 0x1284


#define LCD_MODUL_ID (0x02)

#if 0
#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

static unsigned int lcm_esd_test = FALSE; ///only for ESD test

#endif

#ifdef BUILD_LK
#define LCM_DBG printf
#else
#define LCM_DBG printk
#endif

// ---------------------------------------------------------------------------
// Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v) (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
// Local Functions
// ---------------------------------------------------------------------------

#define dsi_set_cmdq_V2(cmd, count, ppara, force_update) lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update) lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd) lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums) lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd) lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size) lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size) 

static struct LCM_setting_table {
unsigned char cmd;
unsigned char count;
unsigned char para_list[64];
}lcm_initialization_setting[],lcm_deep_sleep_mode_in_setting[];

static struct LCM_setting_table lcm_initialization_setting[] = {
	
	/*
	Note :

	Data ID will depends on the following rule.
	
		count of parameters > 1	=> Data ID = 0x39
		count of parameters = 1	=> Data ID = 0x15
		count of parameters = 0	=> Data ID = 0x05

	Structure Format :

	{DCS command, count of parameters, {parameter list}}
	{REGFLAG_DELAY, milliseconds of time, {}},

	...

	Setting ending by predefined flag
	
	{REGFLAG_END_OF_TABLE, 0x00, {}}
	*/

//TM FFS 495 FWVGA__Video

	{0x00, 1, {0x00}},
	{0xff, 3, {0x12,0x84,0x01}},

	{0x00, 1, {0x80}},
	{0xff, 2, {0x12,0x84}},

	{0x00, 1, {0x80}},
	{0xc0, 9, {0x00,0x64,0x00,0x0f,0x11,0x00,0x64,0x0f,0x11}},

	{0x00, 1, {0x90}},
	{0xc0, 6, {0x00,0x5c,0x00,0x01,0x00,0x04}},

	{0x00, 1, {0xa4}},
	{0xc0, 1, {0x00}},

	{0x00, 1, {0xb3}},
	{0xc0, 2, {0x00,0x55}},

	{0x00, 1, {0x81}},
	{0xc1, 1, {0x55}},

	{0x00, 1, {0xA0}},
	{0xc1, 1, {0x02}},
	
	{0x00, 1, {0x90}},
	{0xf5, 4, {0x02,0x11,0x02,0x15}},

	{0x00, 1, {0x90}},
	{0xc5, 1, {0x50}},

	{0x00, 1, {0x94}},
	{0xc5, 1, {0x66}},

	{0x00, 1, {0xb2}},
	{0xf5, 2, {0x00,0x00}},

	{0x00, 1, {0xb4}},
	{0xf5, 2, {0x00,0x00}},

	{0x00, 1, {0xb6}},
	{0xf5, 2, {0x00,0x00}},

	{0x00, 1, {0xb8}},
	{0xf5, 2, {0x00,0x00}},

	{0x00, 1, {0x94}},
	{0xf5, 2, {0x00,0x00}},

	{0x00, 1, {0xd2}},
	{0xf5, 2, {0x06,0x15}},

	{0x00, 1, {0xb4}},
	{0xc5, 1, {0xcc}},

	{0x00, 1, {0xa0}},
	{0xc4, 14, {0x05,0x10,0x06,0x02,0x05,0x15,0x10,0x05,0x10,0x07,0x02,0x05,0x15,0x10}},

	{0x00, 1, {0xb0}},
	{0xc4, 2, {0x00,0x00}},

	{0x00, 1, {0x91}},
	{0xc5, 2, {0x19,0x52}},

	{0x00, 1, {0x00}},
	{0xd8, 2, {0xbc,0xbc}},

	{0x00, 1, {0xb3}},
	{0xc5, 1, {0x44}},

	{0x00, 1, {0xbb}},
	{0xc5, 1, {0x8a}},

	{0x00, 1, {0x82}},
	{0xc4, 1, {0x0a}},

	{0x00, 1, {0x87}},
	{0xc4, 1, {0x18}},

	{0x00, 1, {0x88}},
	{0xc4, 1, {0x80}},

	{0x00, 1, {0xc6}},
	{0xb0, 1, {0x03}},

	{0x00, 1, {0xc2}},
	{0xf5, 1, {0x40}},

	{0x00, 1, {0xc3}},
	{0xf5, 1, {0x85}},

	{0x00, 1, {0x00}},
	{0xe1, 20, {0x05,0x10,0x17,0x22,0x2F,0x3B,0x3C,0x67,0x5A,0x74,0x8C,0x75,0x84,0x5B,0x55,0x45,0x32,0x21,0x12,0x09}},

	{0x00, 1, {0x00}},
	{0xe2, 20, {0x05,0x0F,0x17,0x22,0x2F,0x3B,0x3C,0x67,0x5A,0x74,0x8C,0x75,0x83,0x5B,0x55,0x45,0x32,0x21,0x12,0x09}},

	{0x00, 1, {0x80}},
	{0xcb, 11, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0x90}},
	{0xcb, 15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xa0}},
	{0xcb, 15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xb0}},
	{0xcb, 15, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xc0}},
	{0xcb, 15, {0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xd0}},
	{0xcb, 15, {0x00,0x00,0x00,0x00,0x00,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x05,0x00,0x00}},

	{0x00, 1, {0xe0}},
	{0xcb, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x05,0x05}},

	{0x00, 1, {0xf0}},
	{0xcb, 11, {0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff}},

	{0x00, 1, {0x80}},
	{0xcc, 15, {0x0a,0x0c,0x0e,0x10,0x02,0x04,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0x90}},
	{0xcc, 15, {0x00,0x00,0x00,0x00,0x00,0x2e,0x2d,0x09,0x0b,0x0d,0x0f,0x01,0x03,0x00,0x00}},

	{0x00, 1, {0xa0}},
	{0xcc, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2e,0x2d}},

	{0x00, 1, {0xb0}},
	{0xcc, 15, {0x0f,0x0d,0x0b,0x09,0x03,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xc0}},
	{0xcc, 15, {0x00,0x00,0x00,0x00,0x00,0x2d,0x2e,0x10,0x0e,0x0c,0x0a,0x04,0x02,0x00,0x00}},

	{0x00, 1, {0xd0}},
	{0xcc, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x2d,0x2e}},

	{0x00, 1, {0x80}},
	{0xce, 12, {0x8f,0x03,0x00,0x8e,0x03,0x00,0x8d,0x03,0x00,0x8c,0x03,0x00}},

	{0x00, 1, {0x90}},
	{0xce, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xa0}},
	{0xce, 14, {0x38,0x0b,0x05,0x00,0x00,0x0a,0x0a,0x38,0x0a,0x05,0x01,0x00,0x0a,0x0a}},

	{0x00, 1, {0xb0}},
	{0xce, 14, {0x38,0x09,0x05,0x02,0x00,0x0a,0x0a,0x38,0x08,0x05,0x03,0x00,0x0a,0x0a}},

	{0x00, 1, {0xc0}},
	{0xce, 14, {0x38,0x07,0x05,0x04,0x00,0x0a,0x0a,0x38,0x06,0x05,0x05,0x00,0x0a,0x0a}},

	{0x00, 1, {0xd0}},
	{0xce, 14, {0x38,0x05,0x05,0x06,0x00,0x0a,0x0a,0x38,0x04,0x05,0x07,0x00,0x0a,0x0a}},

	{0x00, 1, {0x80}},
	{0xcf, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0x90}},
	{0xcf, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xa0}},
	{0xcf, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xb0}},
	{0xcf, 14, {0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00}},

	{0x00, 1, {0xc0}},
	{0xcf, 11, {0x01,0x01,0x20,0x20,0x00,0x00,0x01,0x02,0x00,0x00,0x08}},

	{0x00, 1, {0xb5}},
	{0xc5, 6, {0x33,0xf1,0xff,0x33,0xf1,0xff}},

	{0x00, 1, {0x00}},
	{0xff, 3, {0xff,0xff,0xff}},


//--------------------------------------

//{0x00,1,{0x00}},//Address shift
	{0x11,1,{0x00}},
	   
   //printf(" Sleep out \n");
{REGFLAG_DELAY, 120, {}},
   
   
   //{0x00,1,{0x00}},//Address shift
	{0x29,1,{0x00}},
	{REGFLAG_DELAY, 50, {}},
	{REGFLAG_END_OF_TABLE, 0x00, {}}
};



static struct LCM_setting_table lcm_sleep_out_setting[] = {
    // Sleep Out
    {0x11, 1, {0x00}},
    {REGFLAG_DELAY, 120, {}},

    // Display ON
    {0x29, 1, {0x00}},
    {REGFLAG_DELAY, 20, {}},
    {REGFLAG_END_OF_TABLE, 0x00, {}}
};


static struct LCM_setting_table lcm_deep_sleep_mode_in_setting[] = {
     // Display off sequence
     {0x28, 1, {0x00}},
     {REGFLAG_DELAY, 20, {}},
     // Sleep Mode Ondiv1_real*div2_real
     {0x10, 1, {0x00}},
     {REGFLAG_DELAY, 120, {}},
     {REGFLAG_END_OF_TABLE, 0x00, {}}
};

//static int vcom=0x8a;
static void push_table(struct LCM_setting_table *table, unsigned int count, unsigned char force_update)
{
    unsigned int i;

    for(i = 0; i < count; i++) {

        unsigned cmd;
        cmd = table[i].cmd;

        switch (cmd) {

       /*case 0xd9:
       table[i].para_list[0]=vcom;
       dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
       vcom+=1;
       break;*/

        case REGFLAG_DELAY :
            MDELAY(table[i].count);
            break;
        case REGFLAG_END_OF_TABLE :
            break;
        default:
            dsi_set_cmdq_V2(cmd, table[i].count, table[i].para_list, force_update);
        }
    }
}


// ---------------------------------------------------------------------------
// LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		memset(params, 0, sizeof(LCM_PARAMS));
	
		params->type   = LCM_TYPE_DSI;

		params->width  = FRAME_WIDTH;
		params->height = FRAME_HEIGHT;
		params->physical_width  = 62;
		params->physical_height = 110;
#ifdef SLT_DEVINFO_LCM
		params->module="boe";
		params->vendor="boe";
		params->ic="otm1284";
		params->info="720*1280";
#endif
		// enable tearing-free
		//params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
		//params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;

#if (LCM_DSI_CMD_MODE)
		params->dsi.mode   = CMD_MODE;
#else
		params->dsi.mode   = SYNC_PULSE_VDO_MODE;
#endif
	
		// DSI
		/* Command mode setting */
		params->dsi.LANE_NUM				= LCM_FOUR_LANE;
		//The following defined the fomat for data coming from LCD engine.
		params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
		params->dsi.data_format.trans_seq   = LCM_DSI_TRANS_SEQ_MSB_FIRST;
		params->dsi.data_format.padding     = LCM_DSI_PADDING_ON_LSB;
		params->dsi.data_format.format      = LCM_DSI_FORMAT_RGB888;

		// Highly depends on LCD driver capability.
		// Not support in MT6573
		//params->dsi.packet_size=256;

		// Video mode setting		
		//params->dsi.intermediat_buffer_num = 2;

		params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;

		params->dsi.vertical_sync_active				= 4;
		params->dsi.vertical_backporch					= 14;//8
		params->dsi.vertical_frontporch					= 16;//8
		params->dsi.vertical_active_line				= FRAME_HEIGHT; 

		params->dsi.horizontal_sync_active				= 4;//6
		params->dsi.horizontal_backporch				= 42;//37
		params->dsi.horizontal_frontporch				= 44;//37
		params->dsi.horizontal_active_pixel				= FRAME_WIDTH;

		params->dsi.PLL_CLOCK=208;
}


#ifdef BUILD_LK
static struct mt_i2c_t otm1284_i2c;
#define OTM1284_SLAVE_ADDR 0x3e
/**********************************************************
  *
  *   [I2C Function For Read/Write otm1284] 
  *
  *********************************************************/
static kal_uint32 otm1284_write_byte(kal_uint8 addr, kal_uint8 value)
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;

    write_data[0]= addr;
    write_data[1] = value;

    otm1284_i2c.id = 1;
    /* Since i2c will left shift 1 bit, we need to set otm1284 I2C address to >>1 */
    otm1284_i2c.addr = (OTM1284_SLAVE_ADDR);
    otm1284_i2c.mode = ST_MODE;
    otm1284_i2c.speed = 100;
    len = 2;

    ret_code = i2c_write(&otm1284_i2c, write_data, len);
    //printf("%s: i2c_write: ret_code: %d\n", __func__, ret_code);

    return ret_code;
}

#endif
static void KTD2125_enable(char en)
{
	lcm_util.set_gpio_mode(GPIO_LCD_BIAS_ENP_PIN, GPIO_LCD_BIAS_ENP_PIN_M_GPIO);
	lcm_util.set_gpio_mode(GPIO_LCD_BIAS_ENN_PIN, GPIO_LCD_BIAS_ENN_PIN_M_GPIO);
	lcm_util.set_gpio_dir(GPIO_LCD_BIAS_ENP_PIN, GPIO_DIR_OUT);
	lcm_util.set_gpio_dir(GPIO_LCD_BIAS_ENN_PIN, GPIO_DIR_OUT);
	if (en)
	{
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
		MDELAY(12);
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ONE);
		MDELAY(12);
		#ifdef BUILD_LK
		otm1284_write_byte(0x00, 0x0f);
		otm1284_write_byte(0x01, 0x0f);
		#endif
	}
	else
	{
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ZERO);
		MDELAY(12);
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ZERO);
		MDELAY(12);
	}
	
}

static unsigned int lcm_compare_id(void)
{

#if 1
    int array[4];
    char buffer[5];
    char id_high=0;
    char id_low=0;
    int id=0;
	KTD2125_enable(1);
	SET_RESET_PIN(1);
    MDELAY(25);
	SET_RESET_PIN(0);
    MDELAY(10);
	SET_RESET_PIN(1);
    MDELAY(10);
	//KTD2125_enable(1);
    //MDELAY(100);

    array[0] = 0x00053700;// read id return two byte,version and id
    dsi_set_cmdq(array, 1, 1);
    read_reg_v2(0xA1,buffer, 5);

    id_high = buffer[2]; //should be 0x12
    id_low = buffer[3]; //should be 0x84
    id = (id_high<<8)|id_low; //should be 0x1284

    LCM_DBG("%s, id = 0x%08x\n", __func__, id);

    return (LCM_ID == id)?1:0;
#else
    return ((LCD_MODUL_ID == which_lcd_module())? 1:0);
#endif

}



static void lcm_init(void)
{
	KTD2125_enable(1);
    SET_RESET_PIN(1);
    MDELAY(10);
    SET_RESET_PIN(0);
    MDELAY(30);
    SET_RESET_PIN(1);
    MDELAY(30);
	//KTD2125_enable(1);
    //MDELAY(200);

    push_table(lcm_initialization_setting, sizeof(lcm_initialization_setting) / sizeof(struct LCM_setting_table), 1);
}


static void lcm_suspend(void)
{

    push_table(lcm_deep_sleep_mode_in_setting, sizeof(lcm_deep_sleep_mode_in_setting) / sizeof(struct LCM_setting_table), 1);
	KTD2125_enable(0);
}

static void lcm_resume(void)
{
    lcm_init();

    //push_table(lcm_sleep_out_setting, sizeof(lcm_sleep_out_setting) / sizeof(struct LCM_setting_table), 1);
}


#if (LCM_DSI_CMD_MODE)
static void lcm_update(unsigned int x, unsigned int y,
unsigned int width, unsigned int height)
{
    unsigned int x0 = x;
    unsigned int y0 = y;
    unsigned int x1 = x0 + width - 1;
    unsigned int y1 = y0 + height - 1;

    unsigned char x0_MSB = ((x0>>8)&0xFF);
    unsigned char x0_LSB = (x0&0xFF);
    unsigned char x1_MSB = ((x1>>8)&0xFF);
    unsigned char x1_LSB = (x1&0xFF);
    unsigned char y0_MSB = ((y0>>8)&0xFF);
    unsigned char y0_LSB = (y0&0xFF);
    unsigned char y1_MSB = ((y1>>8)&0xFF);
    unsigned char y1_LSB = (y1&0xFF);

    unsigned int data_array[16];

	data_array[0]= 0x00053902;
	data_array[1]= (x1_MSB<<24)|(x0_LSB<<16)|(x0_MSB<<8)|0x2a;
	data_array[2]= (x1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x00053902;
	data_array[1]= (y1_MSB<<24)|(y0_LSB<<16)|(y0_MSB<<8)|0x2b;
	data_array[2]= (y1_LSB);
	dsi_set_cmdq(data_array, 3, 1);

	data_array[0]= 0x002c3909;
	dsi_set_cmdq(data_array, 1, 0);
}
#endif


static unsigned int lcm_esd_check(void)
{
	unsigned int ret = FALSE;
	char buffer[6];
	int array[4];
	
	array[0] = 0x00013700;
	dsi_set_cmdq(array,1,1);

	read_reg_v2(0x0A,buffer,1);
	LCM_DBG("otm1284a esd buffer[0] = %x\n",buffer[0]);

    if(buffer[0] == 0x9C)
    {
        ret = FALSE;
    }
    else
    {
        ret = TRUE;
    }
	return ret;
}

static unsigned int lcm_esd_recover(void)
{

	lcm_init();

    return TRUE;
}



LCM_DRIVER otm1284_hd720_dsi_vdo_boe_lcm_drv = 
{
    .name			= "otm1284_hd720_dsi_vdo_boe",
    .set_util_funcs = lcm_set_util_funcs,
    .compare_id = lcm_compare_id,
    .get_params = lcm_get_params,
    .init = lcm_init,
    .suspend = lcm_suspend,
    .resume = lcm_resume,
    .esd_check = lcm_esd_check,
    .esd_recover = lcm_esd_recover,
    #if (LCM_DSI_CMD_MODE)
    //.set_backlight = lcm_setbacklight,
    .update = lcm_update,
    #endif
};

