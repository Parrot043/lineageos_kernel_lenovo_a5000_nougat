#ifndef BUILD_LK
#include <linux/string.h>
#endif
#include "lcm_drv.h"
#include <cust_gpio_usage.h>
#ifdef BUILD_LK
	#include <platform/mt_gpio.h>
	#include <string.h>
	
	#include <platform/mt_i2c.h>
#elif defined(BUILD_UBOOT)
	#include <asm/arch/mt_gpio.h>
#else
    #include <linux/delay.h>
	#include <mach/mt_gpio.h>
	#include <linux/i2c.h>
#endif

// ---------------------------------------------------------------------------
//  Local Constants
// ---------------------------------------------------------------------------

#define FRAME_WIDTH  (720)
#define FRAME_HEIGHT (1280)

#define REGFLAG_DELAY             							0xFE
#define REGFLAG_END_OF_TABLE      							0xFF   // END OF REGISTERS MARKER

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

#ifndef BUILD_LK
static unsigned int lcm_esd_test = FALSE;      ///only for ESD test
#endif
// ---------------------------------------------------------------------------
//  Local Variables
// ---------------------------------------------------------------------------

static LCM_UTIL_FUNCS lcm_util;

#define SET_RESET_PIN(v)    (lcm_util.set_reset_pin((v)))

#define UDELAY(n) (lcm_util.udelay(n))
#define MDELAY(n) (lcm_util.mdelay(n))


// ---------------------------------------------------------------------------
//  Local Functions
// ---------------------------------------------------------------------------
#define dsi_set_cmdq_V3(para_tbl,size,force_update)        lcm_util.dsi_set_cmdq_V3(para_tbl,size,force_update)
#define dsi_set_cmdq_V2(cmd, count, ppara, force_update)	        lcm_util.dsi_set_cmdq_V2(cmd, count, ppara, force_update)
#define dsi_set_cmdq(pdata, queue_size, force_update)		lcm_util.dsi_set_cmdq(pdata, queue_size, force_update)
#define wrtie_cmd(cmd)										lcm_util.dsi_write_cmd(cmd)
#define write_regs(addr, pdata, byte_nums)					lcm_util.dsi_write_regs(addr, pdata, byte_nums)
#define read_reg(cmd)											lcm_util.dsi_dcs_read_lcm_reg(cmd)
#define read_reg_v2(cmd, buffer, buffer_size)   				lcm_util.dsi_dcs_read_lcm_reg_v2(cmd, buffer, buffer_size)

#define   LCM_DSI_CMD_MODE							0



static LCM_setting_table_V3 lcm_initialization_setting[] = {
	
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
#if 1
	//================================================Dr.IC Initial Code
	//  B9
	{0x39, 0xB9, 3, {0xFF,0x83,0x94}},

	//  BA
	{0x39, 0xBA, 2, {0x32,0x83}},

	//  B1
	{0x39, 0xB1, 15, {0x6C,0x11,0x11,0x24,0x04,0x11,0xF1,0x80,0xEA,0x96,0x23,0x80,0xC0,0xD2,0x58}},

	//  B2
	{0x39, 0xB2,11, {0x00,0x64,0x10,0x07,0x32,0x1C,0x08,0x08,0x1C,0x4D,0x00}},

	//  BC
	{0x15, 0xBC, 1, {0x07}},

	//  BF
	{0x39, 0xBF, 3, {0x41,0x0E,0x01}},


	//  B4
	{0x39, 0xB4, 12, {0x00,0xFF,0x40,0x50,0x40,0x50,0x40,0x50,0x01,0x6A,0x01,0x6A}},

	//  D3
	{0x39, 0xD3, 30, {0x00,0x06,0x00,0x40,0x07,0x00,0x00,0x32,0x10,0x08,
                         0x00,0x08,0x52,0x15,0x0F,0x05,0x0F,0x32,0x10,0x00,
                         0x00,0x00,0x47,0x44,0x0C,0x0C,0x47,0x0C,0x0C,0x47}},

	//  D5
	{0x39, 0xD5, 44, {0x20,0x21,0x00,0x01,0x02,0x03,0x04,0x05,0x06,0x07,
                         0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
                         0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
                         0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x19,0x19,
                         0x18,0x18,0x24,0x25}},

	//  D6
	{0x39, 0xD6, 44, {0x24,0x25,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,
                         0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x58,0x58,
                         0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
                         0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,0x18,
                         0x19,0x19,0x20,0x21}},

	//  E0
	{0x39, 0xE0, 42, {0x00,0x01,0x05,0x31,0x38,0x3F,0x15,0x3D,0x06,0x09,0x0B,0x16,0x0E,0x12,0x15,0x13,0x14,0x07,0x11,0x13,0x18,
                         0x00,0x01,0x05,0x31,0x37,0x3F,0x14,0x3E,0x06,0x09,0x0B,0x17,0x0F,0x12,0x15,0x13,0x14,0x07,0x11,0x12,0x16}},
	//DGC C1  R
	{0x15, 0xBD, 1, {0x00}},
	{0x39, 0xC1, 43, {0x01,0x00,0x08,0x10,0x17,0x1F,0x26,0x2E,0x35,0x3D,
                         0x44,0x4B,0x53,0x5B,0x62,0x6A,0x72,0x79,0x81,0x89,
                         0x91,0x99,0xA1,0xA9,0xB1,0xB9,0xC1,0xCA,0xD2,0xDA,
                         0xE3,0xEC,0xF5,0xFF,0x16,0x27,0xFB,0x29,0xD5,0x45,
                         0x22,0xFF,0xC0}},
	 //C1  G
	{0x15, 0xBD, 1, {0x01}},
	{0x39, 0xC1, 42, {0x00,0x08,0x10,0x17,0x1F,0x26,0x2E,0x35,0x3D,0x44,
                         0x4B,0x53,0x5B,0x62,0x6A,0x72,0x79,0x81,0x89,0x91,
                         0x99,0xA1,0xA9,0xB1,0xB9,0xC1,0xCA,0xD2,0xDA,0xE3,
                         0xEC,0xF5,0xFF,0x16,0x27,0xFB,0x29,0xD5,0x45,0x22,
                         0xFF,0xC0}},
	//C1  B
	{0x15, 0xBD, 1, {0x02}},
	{0x39, 0xC1, 42, {0x00,0x08,0x10,0x17,0x1F,0x26,0x2E,0x35,0x3D,0x44,
                         0x4B,0x53,0x5B,0x62,0x6A,0x72,0x79,0x81,0x89,0x91,
                         0x99,0xA1,0xA9,0xB1,0xB9,0xC1,0xCA,0xD2,0xDA,0xE3,
                         0xEC,0xF5,0xFF,0x16,0x27,0xFB,0x29,0xD5,0x45,0x22,
                         0xFF,0xC0}},

	//  B6
	{0x39, 0xB6, 2, {0x70,0x70}},

	//  CC
	{0x15, 0xCC, 1, {0x09}},

	//  C0
	{0x39, 0xC0, 2, {0x30,0x14}},

	//  C7
	{0x39, 0xC7, 4, {0x00,0xC0,0x40,0xC0}},


	{0x05, 0x11, 0, {}},
	{0x05, 0x29, 0, {}},




	//***CABC***///
	{0x15, 0x51, 1, {0xFF}},//Write display brightness
	{0x39, 0x53, 1, {0x24}},//Write CTRL display
	{0x39, 0x55, 1, {0x00}},//Write content adaptive brightness control,01~UI mode, 02~ Still mode,03~Moving mode,00~Off
#endif
};

// ---------------------------------------------------------------------------
//  LCM Driver Implementations
// ---------------------------------------------------------------------------

static void lcm_set_util_funcs(const LCM_UTIL_FUNCS *util)
{
    memcpy(&lcm_util, util, sizeof(LCM_UTIL_FUNCS));
}


static void lcm_get_params(LCM_PARAMS *params)
{
		//	lcm_debug("%s %d\n", __func__,__LINE__);
			memset(params, 0, sizeof(LCM_PARAMS));
		
			params->type   = LCM_TYPE_DSI;
	
			params->width  = FRAME_WIDTH;
			params->height = FRAME_HEIGHT;
			#ifdef SLT_DEVINFO_LCM
			params->module="tianma";
			params->vendor="tianma";
			params->ic="hx8394d";
			params->info="720*1280";
			#endif
			// enable tearing-free
			params->dbi.te_mode 				= LCM_DBI_TE_MODE_VSYNC_ONLY;
			params->dbi.te_edge_polarity		= LCM_POLARITY_RISING;
	
#if (LCM_DSI_CMD_MODE)
			params->dsi.mode   = CMD_MODE;
#else
			params->dsi.mode   = BURST_VDO_MODE;//SYNC_PULSE_VDO_MODE;//BURST_VDO_MODE;
#endif
		
			// DSI
			/* Command mode setting */
			params->dsi.LANE_NUM				= LCM_FOUR_LANE;
			//The following defined the fomat for data coming from LCD engine.
			params->dsi.data_format.color_order = LCM_COLOR_ORDER_RGB;
			params->dsi.data_format.trans_seq	= LCM_DSI_TRANS_SEQ_MSB_FIRST;
			params->dsi.data_format.padding 	= LCM_DSI_PADDING_ON_LSB;
			params->dsi.data_format.format		= LCM_DSI_FORMAT_RGB888;
	
			params->dsi.PS=LCM_PACKED_PS_24BIT_RGB888;
	
			params->dsi.vertical_sync_active				= 4;
			params->dsi.vertical_backporch					= 12;
			params->dsi.vertical_frontporch 				= 16;//16;
			params->dsi.vertical_active_line				= FRAME_HEIGHT; 
	
			params->dsi.horizontal_sync_active				= 55;
			params->dsi.horizontal_backporch				= 100;
			params->dsi.horizontal_frontporch				= 100;
			params->dsi.horizontal_active_pixel 			= FRAME_WIDTH;
	
			params->dsi.PLL_CLOCK = 232;//dsi clock customization: should config clock value directly


}
#ifdef BUILD_LK
static struct mt_i2c_t hx8394d_i2c;
#define HX8394D_SLAVE_ADDR 0x3e
/**********************************************************
  *
  *   [I2C Function For Read/Write hx8394d] 
  *
  *********************************************************/
static kal_uint32 hx8394d_write_byte(kal_uint8 addr, kal_uint8 value)
{
    kal_uint32 ret_code = I2C_OK;
    kal_uint8 write_data[2];
    kal_uint16 len;

    write_data[0]= addr;
    write_data[1] = value;

    hx8394d_i2c.id = 1;
    /* Since i2c will left shift 1 bit, we need to set hx8394d I2C address to >>1 */
    hx8394d_i2c.addr = (HX8394D_SLAVE_ADDR);
    hx8394d_i2c.mode = ST_MODE;
    hx8394d_i2c.speed = 100;
    len = 2;

    ret_code = i2c_write(&hx8394d_i2c, write_data, len);
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
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ONE);
		mdelay(12);
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ONE);
		mdelay(12);
		#ifdef BUILD_LK
		hx8394d_write_byte(0x00, 0x0f);
		hx8394d_write_byte(0x01, 0x0f);
		#endif
	}
	else
	{
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENP_PIN, GPIO_OUT_ZERO);
		mdelay(12);
		lcm_util.set_gpio_out(GPIO_LCD_BIAS_ENN_PIN, GPIO_OUT_ZERO);
		mdelay(12);
	}
	
}
static void lcm_init(void)
{
	unsigned int data_array[16];
		#ifdef BUILD_LK
		  printf("[LK]---cmd---hx8394d----%s------\n",__func__);
    #else
		  printk("[KERNEL]---cmd---hx8394d----%s------\n",__func__);
    #endif	
	
			//unsigned int data_array[35];

	KTD2125_enable(1);
	mdelay(50);
	
	SET_RESET_PIN(1);
	mdelay(10); 
	SET_RESET_PIN(0);
	mdelay(10); 
	SET_RESET_PIN(1);
	mdelay(130); 
	
	
	
	
	data_array[0]=0x00043902;
	data_array[1]=0x9483ffb9;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);
	
	data_array[0]=0x000c3902;
	data_array[1]=0x204373ba; //data_array[1]=0xA88333ba;  增加驱动能力 
    data_array[2]=0x0909B265;
	data_array[3]=0x00001040;
	dsi_set_cmdq(data_array, 4, 1);
	//mdelay(1);
	
	data_array[0]=0x00103902;
	data_array[1]=0x11116Cb1;
	data_array[2]=0xF1110437;//vgl=-5.55*2
	data_array[3]=0x23949F80;
	data_array[4]=0x18D2C080;
	dsi_set_cmdq(data_array, 5, 1);
	//mdelay(10);
	
	data_array[0]=0x000C3902;
	data_array[1]=0x0E6400b2;
	data_array[2]=0x0823320D;
    data_array[3]=0x004D1C08;
	dsi_set_cmdq(data_array, 4, 1);
	//mdelay(1);
	
		
	data_array[0]=0x000D3902;
	data_array[1]=0x03FF00b4;
	data_array[2]=0x03460346;
	data_array[3]=0x01680146;//016A0146
	data_array[4]=0x00000068;//0000006A
	dsi_set_cmdq(data_array, 5, 1);
	//mdelay(1);
	
	data_array[0]=0x00043902;
	data_array[1]=0x010E41BF;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);

	data_array[0]=0x00263902;
	data_array[1]=0x000700D3;
	data_array[2]=0x00100000;
	data_array[3]=0x00051032;
	data_array[4]=0x00103205;
	data_array[5]=0x10320000;
	data_array[6]=0x36000000;
	data_array[7]=0x37090903;   
	data_array[8]=0x00370000;
	data_array[9]=0x0A000000;
	data_array[10]=0x00000100;
	dsi_set_cmdq(data_array, 11, 1);
	//mdelay(1);
	

	
	data_array[0]=0x002D3902;
	data_array[1]=0x000302d5;
	data_array[2]=0x04070601;
	data_array[3]=0x22212005;
	data_array[4]=0x18181823;
	data_array[5]=0x18181818;
	data_array[6]=0x18181818;
	data_array[7]=0x18181818;   
	data_array[8]=0x18181818;
	data_array[9]=0x18181818;
	data_array[10]=0x24181818;
	data_array[11]=0x19181825;
	data_array[12]=0x00000019;
	dsi_set_cmdq(data_array, 13, 1);
	mdelay(10);


	data_array[0]=0x002D3902;
	data_array[1]=0x070405D6;
	data_array[2]=0x03000106;
	data_array[3]=0x21222302;
	data_array[4]=0x18181820;
	data_array[5]=0x58181818;
	data_array[6]=0x18181858;
	data_array[7]=0x18181818;   
	data_array[8]=0x18181818;
	data_array[9]=0x18181818;
	data_array[10]=0x25181818;
	data_array[11]=0x18191924;
	data_array[12]=0x00000018;
	dsi_set_cmdq(data_array, 13, 1);
	mdelay(10);

	data_array[0]=0x002b3902;
	data_array[1]=0x0A0A02e0;
	data_array[2]=0x203F2829;//	data_array[2]=0x1A332924;
	data_array[3]=0x0F0E0C3F;//	data_array[3]=0x0D0B083C;
	data_array[4]=0x14120E17;//	data_array[4]=0x15120F17;
	data_array[5]=0x12081413;
	data_array[6]=0x0A021916;
	data_array[7]=0x3F28290A;   
	data_array[8]=0x0E0C3F20;//	data_array[8]=0x0B083C1A;
	data_array[9]=0x120E170F;//	data_array[9]=0x120F170D;
	data_array[10]=0x08141314;//	data_array[10]=0x08141315;
	data_array[11]=0x00191612;
	dsi_set_cmdq(data_array, 12, 1);
	//mdelay(1);

	data_array[0]=0x00023902;
	data_array[1]=0x000009cc;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);

	data_array[0]=0x00053902;
	data_array[1]=0x40C000c7;
	data_array[2]=0x000000C0;
	dsi_set_cmdq(data_array, 3, 1);
	//mdelay(1);
	
	//data_array[0]=0x00033902;
    //data_array[1]=0x007F69b6;//VCOM
	//dsi_set_cmdq(data_array, 2, 1);
	//MDELAY(1);

	data_array[0]=0x00033902;
    data_array[1]=0x001430C0;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);

	data_array[0]=0x00023902;
    data_array[1]=0x000007BC;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);

/*
	data_array[0]=0x00023902;
    data_array[1]=0x0000FF51;
	dsi_set_cmdq(data_array, 2, 1);
	mdelay(1);

	data_array[0]=0x00023902;
    data_array[1]=0x00002453;
	dsi_set_cmdq(data_array, 2, 1);
	mdelay(1);

	data_array[0]=0x00023902;
    data_array[1]=0x00000055;
	dsi_set_cmdq(data_array, 2, 1);
	mdelay(20);
	*/

	data_array[0]=0x00033902;
    data_array[1]=0x000152E4;
	dsi_set_cmdq(data_array, 2, 1);
	mdelay(5); 

	data_array[0]=0x00023902;
	data_array[1]=0x00000035;
	dsi_set_cmdq(data_array, 2, 1);
	

	
	data_array[0]= 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	mdelay(200);
	
	data_array[0]= 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	mdelay(10);   
 
}



static void lcm_suspend(void)
{
	unsigned int data_array[16];

	//data_array[0]=0x00280500; // Display Off
	//dsi_set_cmdq(data_array, 1, 1);
	
	data_array[0] = 0x00100500; // Sleep In
	dsi_set_cmdq(data_array, 1, 1);
	mdelay(120);

	
	//SET_RESET_PIN(1);	
    //mdelay(20); 
	SET_RESET_PIN(0);
	mdelay(10); // 1ms
	
	SET_RESET_PIN(1);
	mdelay(120);   
	KTD2125_enable(0);  
	//lcm_util.set_gpio_out(GPIO_LCD_ENN, GPIO_OUT_ZERO);
	//lcm_util.set_gpio_out(GPIO_LCD_ENP, GPIO_OUT_ZERO); 
}

static void lcm_resume(void)
{
	lcm_init();
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

static unsigned int lcm_compare_id(void)
{
/*
  	unsigned int ret = 0;

	ret = mt_get_gpio_in(GPIO92);
#if defined(BUILD_LK)
	printf("%s, [jx]hx8394d GPIO92 = %d \n", __func__, ret);
#endif	
	//return 1; // by zjz for test
	
	return (ret == 0)?1:0; 
*/
	int   array[4];
	char  buffer[3];
	char  id0=0;
	char  id1=0;
	char  id2=0;
			int  id=0;
		KTD2125_enable(0);
		SET_RESET_PIN(1);
		mdelay(20); 
		SET_RESET_PIN(0);
		mdelay(10); 
		SET_RESET_PIN(1);
		mdelay(10); 
		KTD2125_enable(1);
		mdelay(200);
	
			array[0] = 0x00013700;// read id return two byte,version and id
		dsi_set_cmdq(array, 1, 1);
		
		read_reg_v2(0xda, buffer, 1);
			id0 = buffer[0]; //should be 0x83
	
	   
			array[0] = 0x00013700;// read id return two byte,version and id
		dsi_set_cmdq(array, 1, 1);
		
		read_reg_v2(0xdb, buffer, 1);
			id1 = buffer[0]; //should be 0x94
	
				//	 id0=read_reg(0xda);
				//	 id1=read_reg(0xdb);
	
		   id = (id0<<8)|id1; //we only need ID
#ifdef BUILD_LK
	printf("%s, LK hx8394d id0 = 0x%08x\n", __func__, id0);
	printf("%s, LK hx8394d id1 = 0x%08x\n", __func__, id1);
	printf("%s, lk hx8394d id = 0x%08x\n", __func__, id);
#else
	printk("%s, Kernel hx8394d id0 = 0x%08x\n", __func__, id0);
	printk("%s, Kernel hx8394d id1 = 0x%08x\n", __func__, id1);
	printk("%s, Kernel hx8394d id = 0x%08x\n", __func__, id);
#endif
	
	if(id == 0X8394)//8394 zjz add for seprate DJ and Yushun
			return 1;
	else
		return 0;

}


static unsigned int lcm_esd_check(void)
{
  #ifndef BUILD_LK
	char  buffer[4];
    char  buffer1[1];
	char  buffer2[1];
	int   array[4];

	if(lcm_esd_test)
	{
		lcm_esd_test = FALSE;
		return TRUE;
	}
/*
	array[0] = 0x00043902; 						 
		array[1] = 0x9483FFB9; 				
	        dsi_set_cmdq(&array, 2, 1); 

	array[0] = 0x00103902;			   
		 array[1] = 0x011116CB1;   
		 array[2] = 0xF1110424; 
		 array[3] = 0x2396EA80; 
		 array[4] = 0x58D2C080;  
		 dsi_set_cmdq(&array, 5, 1);
*/
	array[0] = 0x00043700;
	dsi_set_cmdq(array, 1, 1);
	read_reg_v2(0x09, buffer, 4);

	array[0] = 0x00013700;
		dsi_set_cmdq(array, 1, 1);
		read_reg_v2(0x0a, buffer1, 1);

		array[0] = 0x00013700;
		dsi_set_cmdq(array, 1, 1);
		read_reg_v2(0xD9, buffer2, 1);

	
	//array[0] = 0x00013700;
	//dsi_set_cmdq(array, 1, 1);

	//read_reg_v2(0x0A, &buffer[2], 1);
	//printk("<%s:%d>buffer[%x][%x][%x][%x]\n", __func__, __LINE__, buffer[0], buffer[1], buffer[2], buffer[3]);
	if((buffer[0]==0x80) && (buffer[1]==0x73) && (buffer[2]==0x06) && (buffer1[0]==0x1c)&& (buffer2[0]==0x80))
	{
		return FALSE;
	}
	else
	{			 
		return TRUE;
	}
#else
	return FALSE;
#endif
}

static unsigned int lcm_esd_recover(void)
{
	unsigned int data_array[16];
	
	SET_RESET_PIN(1);
	mdelay(10); 
	SET_RESET_PIN(0);
	mdelay(10); 
	SET_RESET_PIN(1);
	mdelay(130); 
	
	
	
	
	data_array[0]=0x00043902;
	data_array[1]=0x9483ffb9;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);
	
	data_array[0]=0x000c3902;
	data_array[1]=0x204373ba; //data_array[1]=0xA88333ba;  增加驱动能力 
    data_array[2]=0x0909B265;
	data_array[3]=0x00001040;
	dsi_set_cmdq(data_array, 4, 1);
	//mdelay(1);
	
	data_array[0]=0x00103902;
	data_array[1]=0x11116Cb1;
	data_array[2]=0xF1110437;//vgl=-5.55*2
	data_array[3]=0x23949F80;
	data_array[4]=0x18D2C080;
	dsi_set_cmdq(data_array, 5, 1);
	//mdelay(10);
	
	data_array[0]=0x000C3902;
	data_array[1]=0x0E6400b2;
	data_array[2]=0x0823320D;
    data_array[3]=0x004D1C08;
	dsi_set_cmdq(data_array, 4, 1);
	//mdelay(1);
	
		
	data_array[0]=0x000D3902;
	data_array[1]=0x03FF00b4;
	data_array[2]=0x03460346;
	data_array[3]=0x01680146;//016A0146
	data_array[4]=0x00000068;//0000006A
	dsi_set_cmdq(data_array, 5, 1);
	//mdelay(1);
	
	data_array[0]=0x00043902;
	data_array[1]=0x010E41BF;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);

	data_array[0]=0x00263902;
	data_array[1]=0x000700D3;
	data_array[2]=0x00100000;
	data_array[3]=0x00051032;
	data_array[4]=0x00103205;
	data_array[5]=0x10320000;
	data_array[6]=0x36000000;
	data_array[7]=0x37090903;   
	data_array[8]=0x00370000;
	data_array[9]=0x0A000000;
	data_array[10]=0x00000100;
	dsi_set_cmdq(data_array, 11, 1);
	//mdelay(1);
	

	
	data_array[0]=0x002D3902;
	data_array[1]=0x000302d5;
	data_array[2]=0x04070601;
	data_array[3]=0x22212005;
	data_array[4]=0x18181823;
	data_array[5]=0x18181818;
	data_array[6]=0x18181818;
	data_array[7]=0x18181818;   
	data_array[8]=0x18181818;
	data_array[9]=0x18181818;
	data_array[10]=0x24181818;
	data_array[11]=0x19181825;
	data_array[12]=0x00000019;
	dsi_set_cmdq(data_array, 13, 1);
	mdelay(10);


	data_array[0]=0x002D3902;
	data_array[1]=0x070405D6;
	data_array[2]=0x03000106;
	data_array[3]=0x21222302;
	data_array[4]=0x18181820;
	data_array[5]=0x58181818;
	data_array[6]=0x18181858;
	data_array[7]=0x18181818;   
	data_array[8]=0x18181818;
	data_array[9]=0x18181818;
	data_array[10]=0x25181818;
	data_array[11]=0x18191924;
	data_array[12]=0x00000018;
	dsi_set_cmdq(data_array, 13, 1);
	mdelay(10);

	data_array[0]=0x002b3902;
	data_array[1]=0x0A0A02e0;
	data_array[2]=0x203F2829;//	data_array[2]=0x1A332924;
	data_array[3]=0x0F0E0C3F;//	data_array[3]=0x0D0B083C;
	data_array[4]=0x14120E17;//	data_array[4]=0x15120F17;
	data_array[5]=0x12081413;
	data_array[6]=0x0A021916;
	data_array[7]=0x3F28290A;   
	data_array[8]=0x0E0C3F20;//	data_array[8]=0x0B083C1A;
	data_array[9]=0x120E170F;//	data_array[9]=0x120F170D;
	data_array[10]=0x08141314;//	data_array[10]=0x08141315;
	data_array[11]=0x00191612;
	dsi_set_cmdq(data_array, 12, 1);
	//mdelay(1);

	data_array[0]=0x00023902;
	data_array[1]=0x000009cc;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);

	data_array[0]=0x00053902;
	data_array[1]=0x40C000c7;
	data_array[2]=0x000000C0;
	dsi_set_cmdq(data_array, 3, 1);
	//mdelay(1);
	
	//data_array[0]=0x00033902;
    //data_array[1]=0x007F69b6;//VCOM
	//dsi_set_cmdq(data_array, 2, 1);
	//MDELAY(1);

	data_array[0]=0x00033902;
    data_array[1]=0x001430C0;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);

	data_array[0]=0x00023902;
    data_array[1]=0x000007BC;
	dsi_set_cmdq(data_array, 2, 1);
	//mdelay(1);



	data_array[0]=0x00033902;
    data_array[1]=0x000152E4;
	dsi_set_cmdq(data_array, 2, 1);
	mdelay(5); 

	data_array[0]=0x00023902;
	data_array[1]=0x00000035;
	dsi_set_cmdq(data_array, 2, 1);
	

	
	data_array[0]= 0x00110500;
	dsi_set_cmdq(data_array, 1, 1);
	mdelay(200);
	
	data_array[0]= 0x00290500;
	dsi_set_cmdq(data_array, 1, 1);
	mdelay(10);   

	
	return true;
}



LCM_DRIVER hx8394d_hd720_dsi_vdo_tianma_lcm_drv = 
{
    .name			= "hx8394d_hd720_dsi_vdo_tianma",
	.set_util_funcs = lcm_set_util_funcs,
	.get_params     = lcm_get_params,
	.init           = lcm_init,
	.suspend        = lcm_suspend,
	.resume         = lcm_resume,
	.compare_id     = lcm_compare_id,
	.esd_check = lcm_esd_check,
	.esd_recover = lcm_esd_recover,
#if (LCM_DSI_CMD_MODE)
    .update         = lcm_update,
#endif
    };
