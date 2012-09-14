/* linux/drivers/media/video/mt9m113.h
 *
 * Copyright (c) 2010 Samsung Electronics Co., Ltd.
 *	http://www.samsung.com/
 *
 * Driver for MT9M113 (SXGA camera) from Samsung Electronics
 * 1/6" 1.3Mp CMOS Image Sensor SoC with an Embedded Image Processor
 * supporting MIPI CSI-2
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#define MT9M113_COMPLETE
#undef MT9M113_COMPLETE
#ifndef __MT9M113_H__
#define MT9M113_H

#define MT9M113_DRIVER_NAME	"MT9M113"

#define DEFAULT_FMT V4L2_PIX_FMT_UYVY

enum mt9m113_runmode {
	MT9M113_RUNMODE_NOTREADY,
	MT9M113_RUNMODE_IDLE,
	MT9M113_RUNMODE_RUNNING,
};
#if 0
struct mt9m113_state {
	struct v4l2_subdev sd;
	/*
	 * req_fmt is the requested format from the application.
	 * set_fmt is the output format of the camera. Finally FIMC
	 * converts the camera output(set_fmt) to the requested format
	 * with hardware scaler.
	 */
	struct v4l2_pix_format req_fmt;
	struct v4l2_pix_format set_fmt;
	enum mt9m113_runmode runmode;
	int fps;

	int default_width;
	int default_height;
	int capture_width;
	int capture_height;

	int check_dataline;
	int set_vhflip;
	int revision;
};

static inline struct mt9m113_state *to_state(struct v4l2_subdev *sd) {
	return container_of(sd, struct mt9m113_state, sd);
}
#endif 
struct mt9m113_reg {
	unsigned short waddr;
	unsigned short wdata;
	unsigned short mdelay_time;
};

struct mt9m113_tbl {
	const struct mt9m113_reg *mt9m113_regs;
	int size;
};

struct mt9m113_reg set_resol_640x480[] =
{
	
{ 0x098C, 0x2703,0},// output a
{ 0x0990, 0x0280,0},////640 w
{  0x098C, 0x2705,0},// output a
 { 0x0990, 0x01E0,0},//// 480 w

{  0x098C, 0x2739,0},// crop x0 a
{  0x0990, 0x0000,0},//
{  0x098C, 0x273B,0},// crop x1 a
{  0x0990, 0x027F,0},//
{  0x098C, 0x273D,0},// crop y0 a
{  0x0990, 0x0000,0},//
{  0x098C, 0x273F,0},//crop y1 a
{  0x0990, 0x01DF,0},//
};

struct mt9m113_reg set_resol_1280x960[]={
	
{   0x098C, 0x2703,0},// output a
{   0x0990, 1280,0},//
{   0x098C, 0x2705,0},// output a
{   0x0990, 960,0},//

{   0x098C, 0x2739,0},// crop x0 a
{   0x0990, 0x0000,0},//
{   0x098C, 0x273B,0},// crop x1 a
{   0x0990, 1280-1,0},
{   0x098C, 0x273D,0},// crop y0 a
{   0x0990, 0x0000,0},//
{   0x098C, 0x273F,0},//crop y1 a
{   0x0990, 960-1,0},
};

struct mt9m113_reg set_cap_resol_640x480[] =
{
	
{ 0x098C, 0x2703,0},// output a
{ 0x0990, 0x0280,0},////640 w
{  0x098C, 0x2705,0},// output a
 { 0x0990, 0x01E0,0},//// 480 w

{ 0x098C, 0x2747, 0 }, //	MCU_ADDRESS	[MODE_CROP_X0_B	[=	0]]
{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
{ 0x098C, 0x2749, 0 }, //	MCU_ADDRESS	[MODE_CROP_X1_B	[=	1279]]
{ 0x0990, 640-1, 0 }, //	MCU_DATA_0

{ 0x098C, 0x274B, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y0_B	[=	22]]
{ 0x0990, 0x0016, 0 }, //	MCU_DATA_0
{ 0x098C, 0x274D, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y1_B	[=	1001]]
{ 0x0990, 480-1, 0 }, //	MCU_DATA_0

};

struct mt9m113_reg set_cap_resol_1280x960[]={
	
{   0x098C, 0x2703,0},// output a
{   0x0990, 1280,0},//
{   0x098C, 0x2705,0},// output a
{   0x0990, 960,0},//

{ 0x098C, 0x2747, 0 }, //	MCU_ADDRESS	[MODE_CROP_X0_B	[=	0]]
{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
{ 0x098C, 0x2749, 0 }, //	MCU_ADDRESS	[MODE_CROP_X1_B	[=	1279]]
{ 0x0990, 0x04FF, 0 }, //	MCU_DATA_0

{ 0x098C, 0x274B, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y0_B	[=	22]]
{ 0x0990, 0x0016, 0 }, //	MCU_DATA_0
{ 0x098C, 0x274D, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y1_B	[=	1001]]
{ 0x0990, 0x03E9, 0 }, //	MCU_DATA_0

};


struct mt9m113_reg Refresh[]={

	//enter refresh mode

{   0x098C,0xA103 ,0},//

{   0x0990,0x0006 ,300},//

{   0x098C,0xA103 ,0},//

{   0x0990,0x0005 ,300},//

};
	
struct mt9m113_reg mt9m113_init_regs[] =
{
	{ 0x001C, 0x0001, 0 }, //	MCU_BOOT_MODE
	{ 0x001C, 0x0000, 10 }, //	MCU_BOOT_MODE
	{ 0x001A, 0x0011, 0 }, //	RESET_AND_MISC_CONTROL
	{ 0x001A, 0x0010, 0 }, //	RESET_AND_MISC_CONTROL
	{ 0x0016, 0x00FF, 0 }, //	CLOCKS_CONTROL
//	{ 0x0016, 0x0400, 0 }, //	CLOCKS_CONTROL
	{ 0x0018, 0x0028, 0 }, //	STANDBY_CONTROL
	{ 0x0014, 0x2145, 0 }, //	PLL_CONTROL
	{ 0x0010, 0x0114, 0 }, //	PLL_DIVIDERS
	{ 0x0012, 0x1FF1, 0 }, //	PLL_P_DIVIDERS
	{ 0x0014, 0x2545, 0 }, //	PLL_CONTROL
	{ 0x0014, 0x2547, 0 }, //	PLL_CONTROL
	{ 0x0014, 0x3447, 10 }, //	PLL_CONTROL
	{ 0x0014, 0x3047, 0 }, //	PLL_CONTROL
	{ 0x0014, 0x3046, 0 }, //	PLL_CONTROL

   { 0x001E, 0x0777, 0 }, //   SYSCTL Register

	{ 0x321C, 0x0003, 0 }, //	By Pass TxFIFO = 3

	{ 0x098C, 0x2703, 0 }, //	MCU_ADDRESS	[MODE_OUTPUT_WIDTH_A	[=	640]]
	{ 0x0990, 0x0280, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2705, 0 }, //	MCU_ADDRESS	[MODE_OUTPUT_HEIGHT_A	[=	480]]
	{ 0x0990, 0x01E0, 0 }, //	MCU_DATA_0
	
	{ 0x098C, 0x2707, 0 }, //	MCU_ADDRESS	[MODE_OUTPUT_WIDTH_B	[=	1280]]
	{ 0x0990, 0x0500, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2709, 0 }, //	MCU_ADDRESS	[MODE_OUTPUT_HEIGHT_B	[=	960]]
	{ 0x0990, 0x03C0, 0 }, //	MCU_DATA_0

	{ 0x098C, 0x270D, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_ROW_START_A	[=	0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x270F, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_COL_START_A	[=	0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2711, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_ROW_END_A	[=	973]]
	{ 0x0990, 0x03CD, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2713, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_COL_END_A	[=	1293]]
	{ 0x0990, 0x050D, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2715, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_ROW_SPEED_A	[=	8465]]
	{ 0x0990, 0x2111, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2717, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_READ_MODE_A	[=	1132]]
	{ 0x0990, 0x046C, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2719, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FINE_CORRECTION_A	[=	172]]
	{ 0x0990, 0x00AC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x271B, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FINE_IT_MIN_A	[=	497]]
	{ 0x0990, 0x01F1, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x271D, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FINE_IT_MAX_MARGIN_A	[=	319]]
	{ 0x0990, 0x013F, 0 }, //	MCU_DATA_0

	{ 0x098C, 0x271F, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FRAME_LENGTH_A	[=	872]]
	{ 0x0990, 0x0368, 0 }, //	MCU_DATA_0

	{ 0x098C, 0x2721, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_LINE_LENGTH_PCK_A	[=	1228]]
	{ 0x0990, 0x04CC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2723, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_ROW_START_B	[=	4]]
	{ 0x0990, 0x0004, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2725, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_COL_START_B	[=	4]]
	{ 0x0990, 0x0004, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2727, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_ROW_END_B	[=	1035]]
	{ 0x0990, 0x040B, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2729, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_COL_END_B	[=	1291]]
	{ 0x0990, 0x050B, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x272B, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_ROW_SPEED_B	[=	8465]]
	{ 0x0990, 0x2111, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x272D, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_READ_MODE_B	[=	36]]
	{ 0x0990, 0x0024, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x272F, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FINE_CORRECTION_B	[=	76]]
	{ 0x0990, 0x004C, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2731, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FINE_IT_MIN_B	[=	249]]
	{ 0x0990, 0x00F9, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2733, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FINE_IT_MAX_MARGIN_B	[=	167]]
	{ 0x0990, 0x00A7, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2735, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_FRAME_LENGTH_B	[=	1121]]
	{ 0x0990, 0x0461, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2737, 0 }, //	MCU_ADDRESS	[MODE_SENSOR_LINE_LENGTH_PCK_B	[=	1826]]
	{ 0x0990, 0x0722, 0 }, //	MCU_DATA_0

	{ 0x098C, 0x2739, 0 }, //	MCU_ADDRESS	[MODE_CROP_X0_A	[=	0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x273B, 0 }, //	MCU_ADDRESS	[MODE_CROP_X1_A	[=	639]]
	{ 0x0990, 0x027F, 0 }, //	MCU_DATA_0

	{ 0x098C, 0x273D, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y0_A	[=	0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x273F, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y1_A	[=	479]]
	{ 0x0990, 0x01DF, 0 }, //	MCU_DATA_0
	
	{ 0x098C, 0x2747, 0 }, //	MCU_ADDRESS	[MODE_CROP_X0_B	[=	0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2749, 0 }, //	MCU_ADDRESS	[MODE_CROP_X1_B	[=	1279]]
	{ 0x0990, 0x04FF, 0 }, //	MCU_DATA_0
#if 1
	{ 0x098C, 0x274B, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y0_B	[=	22]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x274D, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y1_B	[=	1001]]
	{ 0x0990, 960-1, 0 }, //	MCU_DATA_0
#else
	{ 0x098C, 0x274B, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y0_B	[=	22]]
	{ 0x0990, 0x0016, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x274D, 0 }, //	MCU_ADDRESS	[MODE_CROP_Y1_B	[=	1001]]
	{ 0x0990, 0x03E9, 0 }, //	MCU_DATA_0
#endif	
	{ 0x098C, 0x222D, 0 }, //	MCU_ADDRESS	[AE_R9_STEP	[=	204]]
	{ 0x0990, 0x00CC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA404, 0 }, //	MCU_ADDRESS	[FD_MODE	[=	16]]
	{ 0x0990, 0x0010, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA408, 0 }, //	MCU_ADDRESS	[FD_SEARCH_F1_50	[=	50]]
	{ 0x0990, 0x0032, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA409, 0 }, //	MCU_ADDRESS	[FD_SEARCH_F2_50	[=	52]]
	{ 0x0990, 0x0034, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA40A, 0 }, //	MCU_ADDRESS	[FD_SEARCH_F1_60	[=	60]]
	{ 0x0990, 0x003C, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA40B, 0 }, //	MCU_ADDRESS	[FD_SEARCH_F2_60	[=	62]]
	{ 0x0990, 0x003E, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2411, 0 }, //	MCU_ADDRESS	[FD_R9_STEP_F60_A	[=	204]]
	{ 0x0990, 0x00CC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2413, 0 }, //	MCU_ADDRESS	[FD_R9_STEP_F50_A	[=	244]]
	{ 0x0990, 0x00F4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2415, 0 }, //	MCU_ADDRESS	[FD_R9_STEP_F60_B	[=	137]]
	{ 0x0990, 0x0089, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2417, 0 }, //	MCU_ADDRESS	[FD_R9_STEP_F50_B	[=	164]]
	{ 0x0990, 0x00A4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA40D, 0 }, //	MCU_ADDRESS	[FD_STAT_MIN	[=	2]]
	{ 0x0990, 0x0002, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA40E, 0 }, //	MCU_ADDRESS	[FD_STAT_MAX	[=	3]]
	{ 0x0990, 0x0003, 0 }, //	MCU_DATA_0

	{ 0x098C, 0xA410, 0 }, //	MCU_ADDRESS	[FD_MIN_AMPLITUDE	[=	10]]
	{ 0x0990, 0x000A, 0 }, //	MCU_DATA_0

	{ 0x3658, 0x0210, 0 }, //	P_RD_P0Q0
	{ 0x365A, 0x98AD, 0 }, //	P_RD_P0Q1
	{ 0x365C, 0x07F1, 0 }, //	P_RD_P0Q2
	{ 0x365E, 0x33EC, 0 }, //	P_RD_P0Q3
	{ 0x3660, 0x73CE, 0 }, //	P_RD_P0Q4
	{ 0x3680, 0x5B6C, 0 }, //	P_RD_P1Q0
	{ 0x3682, 0x7FCB, 0 }, //	P_RD_P1Q1
	{ 0x3684, 0x8350, 0 }, //	P_RD_P1Q2
	{ 0x3686, 0xFD0D, 0 }, //	P_RD_P1Q3
	{ 0x3688, 0x1B51, 0 }, //	P_RD_P1Q4
	{ 0x36A8, 0x7A50, 0 }, //	P_RD_P2Q0
	{ 0x36AA, 0x1D0E, 0 }, //	P_RD_P2Q1
	{ 0x36AC, 0x8770, 0 }, //	P_RD_P2Q2
	{ 0x36AE, 0x8B52, 0 }, //	P_RD_P2Q3
	{ 0x36B0, 0x71B2, 0 }, //	P_RD_P2Q4
	{ 0x36D0, 0xB78C, 0 }, //	P_RD_P3Q0
	{ 0x36D2, 0xF14F, 0 }, //	P_RD_P3Q1
	{ 0x36D4, 0x56D2, 0 }, //	P_RD_P3Q2
	{ 0x36D6, 0x6C30, 0 }, //	P_RD_P3Q3
	{ 0x36D8, 0xEDD3, 0 }, //	P_RD_P3Q4
	{ 0x36F8, 0xBE0F, 0 }, //	P_RD_P4Q0
	{ 0x36FA, 0xAF91, 0 }, //	P_RD_P4Q1
	{ 0x36FC, 0x6772, 0 }, //	P_RD_P4Q2
	{ 0x36FE, 0x5D53, 0 }, //	P_RD_P4Q3
	{ 0x3700, 0xACD2, 0 }, //	P_RD_P4Q4
	{ 0x364E, 0x0550, 0 }, //	P_GR_P0Q0
	{ 0x3650, 0x66AE, 0 }, //	P_GR_P0Q1
	{ 0x3652, 0x1BB1, 0 }, //	P_GR_P0Q2
	{ 0x3654, 0xD22B, 0 }, //	P_GR_P0Q3
	{ 0x3656, 0xA48E, 0 }, //	P_GR_P0Q4
	{ 0x3676, 0x6FAC, 0 }, //	P_GR_P1Q0
	{ 0x3678, 0x0BEE, 0 }, //	P_GR_P1Q1
	{ 0x367A, 0x8110, 0 }, //	P_GR_P1Q2
	{ 0x367C, 0xC80D, 0 }, //	P_GR_P1Q3
	{ 0x367E, 0x2C51, 0 }, //	P_GR_P1Q4
	{ 0x369E, 0x6DF0, 0 }, //	P_GR_P2Q0
	{ 0x36A0, 0x4FCC, 0 }, //	P_GR_P2Q1
	{ 0x36A2, 0xEEB1, 0 }, //	P_GR_P2Q2
	{ 0x36A4, 0x00CE, 0 }, //	P_GR_P2Q3
	{ 0x36A6, 0x5293, 0 }, //	P_GR_P2Q4
	{ 0x36C6, 0xE82D, 0 }, //	P_GR_P3Q0
	{ 0x36C8, 0xEF6D, 0 }, //	P_GR_P3Q1
	{ 0x36CA, 0x5072, 0 }, //	P_GR_P3Q2
	{ 0x36CC, 0xB471, 0 }, //	P_GR_P3Q3
	{ 0x36CE, 0x9314, 0 }, //	P_GR_P3Q4
	{ 0x36EE, 0xBFB0, 0 }, //	P_GR_P4Q0
	{ 0x36F0, 0x2191, 0 }, //	P_GR_P4Q1
	{ 0x36F2, 0x1FF4, 0 }, //	P_GR_P4Q2
	{ 0x36F4, 0xB314, 0 }, //	P_GR_P4Q3
	{ 0x36F6, 0xF1B5, 0 }, //	P_GR_P4Q4
	{ 0x3662, 0x0350, 0 }, //	P_BL_P0Q0
	{ 0x3664, 0x444E, 0 }, //	P_BL_P0Q1
	{ 0x3666, 0x70F0, 0 }, //	P_BL_P0Q2
	{ 0x3668, 0x810E, 0 }, //	P_BL_P0Q3
	{ 0x366A, 0x2A8B, 0 }, //	P_BL_P0Q4
	{ 0x368A, 0xF2EB, 0 }, //	P_BL_P1Q0
	{ 0x368C, 0x8BCC, 0 }, //	P_BL_P1Q1
	{ 0x368E, 0x9290, 0 }, //	P_BL_P1Q2
	{ 0x3690, 0x924D, 0 }, //	P_BL_P1Q3
	{ 0x3692, 0x2311, 0 }, //	P_BL_P1Q4
	{ 0x36B2, 0x5370, 0 }, //	P_BL_P2Q0
	{ 0x36B4, 0x6BCE, 0 }, //	P_BL_P2Q1
	{ 0x36B6, 0xC590, 0 }, //	P_BL_P2Q2
	{ 0x36B8, 0xE1B0, 0 }, //	P_BL_P2Q3
	{ 0x36BA, 0x2B52, 0 }, //	P_BL_P2Q4
	{ 0x36DA, 0xBA8D, 0 }, //	P_BL_P3Q0
	{ 0x36DC, 0xA54F, 0 }, //	P_BL_P3Q1
	{ 0x36DE, 0x2F12, 0 }, //	P_BL_P3Q2
	{ 0x36E0, 0x4730, 0 }, //	P_BL_P3Q3
	{ 0x36E2, 0x9693, 0 }, //	P_BL_P3Q4
	{ 0x3702, 0xF74F, 0 }, //	P_BL_P4Q0
	{ 0x3704, 0xA4F0, 0 }, //	P_BL_P4Q1
	{ 0x3706, 0x02D2, 0 }, //	P_BL_P4Q2
	{ 0x3708, 0x3D0D, 0 }, //	P_BL_P4Q3
	{ 0x370A, 0x0B53, 0 }, //	P_BL_P4Q4
	{ 0x366C, 0x01F0, 0 }, //	P_GB_P0Q0
	{ 0x366E, 0xC24D, 0 }, //	P_GB_P0Q1
	{ 0x3670, 0x0691, 0 }, //	P_GB_P0Q2
	{ 0x3672, 0x588A, 0 }, //	P_GB_P0Q3
	{ 0x3674, 0xC2CC, 0 }, //	P_GB_P0Q4
	{ 0x3694, 0x8149, 0 }, //	P_GB_P1Q0
	{ 0x3696, 0x25EE, 0 }, //	P_GB_P1Q1
	{ 0x3698, 0x8F90, 0 }, //	P_GB_P1Q2
	{ 0x369A, 0x908E, 0 }, //	P_GB_P1Q3
	{ 0x369C, 0x1D71, 0 }, //	P_GB_P1Q4
	{ 0x36BC, 0x73B0, 0 }, //	P_GB_P2Q0
	{ 0x36BE, 0x080E, 0 }, //	P_GB_P2Q1
	{ 0x36C0, 0xEF51, 0 }, //	P_GB_P2Q2
	{ 0x36C2, 0xA3B2, 0 }, //	P_GB_P2Q3
	{ 0x36C4, 0x6473, 0 }, //	P_GB_P2Q4
	{ 0x36E4, 0x802E, 0 }, //	P_GB_P3Q0
	{ 0x36E6, 0xB56F, 0 }, //	P_GB_P3Q1
	{ 0x36E8, 0x2732, 0 }, //	P_GB_P3Q2
	{ 0x36EA, 0x88CE, 0 }, //	P_GB_P3Q3
	{ 0x36EC, 0x83D3, 0 }, //	P_GB_P3Q4
	{ 0x370C, 0xCAF0, 0 }, //	P_GB_P4Q0
	{ 0x370E, 0x81B2, 0 }, //	P_GB_P4Q1
	{ 0x3710, 0x2A94, 0 }, //	P_GB_P4Q2
	{ 0x3712, 0x5DB4, 0 }, //	P_GB_P4Q3
	{ 0x3714, 0xFA15, 0 }, //	P_GB_P4Q4
	{ 0x3210, 0x01B8, 0 }, //	COLOR_PIPELINE_CONTROL
	{ 0x3644, 0x028C, 0 }, //	POLY_ORIGIN_C
	{ 0x3642, 0x0208, 0 }, //	POLY_ORIGIN_R
	{ 0x098C, 0xAB3C, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_0	[MCU_DATA_0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB3D, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_1	[MCU_DATA_0]]
	{ 0x0990, 0x0007, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB3E, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_2	[MCU_DATA_0]]
	{ 0x0990, 0x0015, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB3F, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_3	[MCU_DATA_0]]
	{ 0x0990, 0x0032, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB40, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_4	[MCU_DATA_0]]
	{ 0x0990, 0x0054, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB41, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_5	[MCU_DATA_0]]
	{ 0x0990, 0x006E, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB42, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_6	[MCU_DATA_0]]
	{ 0x0990, 0x0084, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB43, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_7	[MCU_DATA_0]]
	{ 0x0990, 0x0097, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB44, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_8	[MCU_DATA_0]]
	{ 0x0990, 0x00A6, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB45, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_9	[MCU_DATA_0]]
	{ 0x0990, 0x00B4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB46, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_10	[MCU_DATA_0]]
	{ 0x0990, 0x00BF, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB47, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_11	[MCU_DATA_0]]
	{ 0x0990, 0x00CA, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB48, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_12	[MCU_DATA_0]]
	{ 0x0990, 0x00D4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB49, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_13	[MCU_DATA_0]]
	{ 0x0990, 0x00DC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB4A, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_14	[MCU_DATA_0]]
	{ 0x0990, 0x00E4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB4B, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_15	[MCU_DATA_0]]
	{ 0x0990, 0x00EC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB4C, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_16	[MCU_DATA_0]]
	{ 0x0990, 0x00F2, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB4D, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_17	[MCU_DATA_0]]
	{ 0x0990, 0x00F9, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB4E, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_A_18	[MCU_DATA_0]]
	{ 0x0990, 0x00FF, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB4F, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_0	[MCU_DATA_0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB50, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_1	[MCU_DATA_0]]
	{ 0x0990, 0x0008, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB51, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_2	[MCU_DATA_0]]
	{ 0x0990, 0x0019, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB52, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_3	[MCU_DATA_0]]
	{ 0x0990, 0x0035, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB53, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_4	[MCU_DATA_0]]
	{ 0x0990, 0x0056, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB54, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_5	[MCU_DATA_0]]
	{ 0x0990, 0x006F, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB55, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_6	[MCU_DATA_0]]
	{ 0x0990, 0x0085, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB56, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_7	[MCU_DATA_0]]
	{ 0x0990, 0x0098, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB57, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_8	[MCU_DATA_0]]
	{ 0x0990, 0x00A7, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB58, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_9	[MCU_DATA_0]]
	{ 0x0990, 0x00B4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB59, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_10	[MCU_DATA_0]]
	{ 0x0990, 0x00C0, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB5A, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_11	[MCU_DATA_0]]
	{ 0x0990, 0x00CA, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB5B, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_12	[MCU_DATA_0]]
	{ 0x0990, 0x00D4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB5C, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_13	[MCU_DATA_0]]
	{ 0x0990, 0x00DC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB5D, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_14	[MCU_DATA_0]]
	{ 0x0990, 0x00E4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB5E, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_15	[MCU_DATA_0]]
	{ 0x0990, 0x00EC, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB5F, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_16	[MCU_DATA_0]]
	{ 0x0990, 0x00F3, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB60, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_17	[MCU_DATA_0]]
	{ 0x0990, 0x00F9, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB61, 0 }, //	MCU_ADDRESS	[HG_GAMMA_TABLE_B_18	[MCU_DATA_0]]
	{ 0x0990, 0x00FF, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2306, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_0	[MCU_DATA_0]]
	{ 0x0990, 0x0170, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2308, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_1	[MCU_DATA_0]]
	{ 0x0990, 0xFFEA, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x230A, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_2	[MCU_DATA_0]]
	{ 0x0990, 0xFFE9, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x230C, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_3	[MCU_DATA_0]]
	{ 0x0990, 0xFF8F, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x230E, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_4	[MCU_DATA_0]]
	{ 0x0990, 0x01FA, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2310, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_5	[MCU_DATA_0]]
	{ 0x0990, 0xFFA3, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2312, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_6	[MCU_DATA_0]]
	{ 0x0990, 0x0021, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2314, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_7	[MCU_DATA_0]]
	{ 0x0990, 0xFF2A, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2316, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_8	[MCU_DATA_0]]
	{ 0x0990, 0x01FA, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2318, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_9	[MCU_DATA_0]]
	{ 0x0990, 0x0020, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x231A, 0 }, //	MCU_ADDRESS	[AWB_CCM_L_10	[MCU_DATA_0]]
	{ 0x0990, 0x004C, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x231C, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_0	[MCU_DATA_0]]
	{ 0x0990, 0x005C, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x231E, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_1	[MCU_DATA_0]]
	{ 0x0990, 0xFF59, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2320, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_2	[MCU_DATA_0]]
	{ 0x0990, 0x004A, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2322, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_3	[MCU_DATA_0]]
	{ 0x0990, 0x0032, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2324, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_4	[MCU_DATA_0]]
	{ 0x0990, 0xFFF1, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2326, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_5	[MCU_DATA_0]]
	{ 0x0990, 0xFFDD, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2328, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_6	[MCU_DATA_0]]
	{ 0x0990, 0xFFFE, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x232A, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_7	[MCU_DATA_0]]
	{ 0x0990, 0x00A4, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x232C, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_8	[MCU_DATA_0]]
	{ 0x0990, 0xFF47, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x232E, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_9	[MCU_DATA_0]]
	{ 0x0990, 0x001C, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2330, 0 }, //	MCU_ADDRESS	[AWB_CCM_RL_10	[MCU_DATA_0]]
	{ 0x0990, 0xFFD8, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA24F, 0 }, //	MCU_ADDRESS	[AE_BASETARGET	[AE_BASETARGET 50 -> 55]]
	{ 0x0990, 0x003C, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA207, 0 }, //	MCU_ADDRESS	MCU_ADDRESS [AE_GATE [AE_GATE]]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA20C, 0 }, //	MCU_ADDRESS	[AE_MAX_INDEX]
	{ 0x0990, 0x0010, 0 }, //	MCU_DATA_0          7.5fps  // 6.3 fps
	{ 0x327A, 0x002c, 0 }, //	BLACK_LEVEL_1ST_RED
	{ 0x327C, 0x002c, 0 }, //	BLACK_LEVEL_1ST_GREEN1
	{ 0x327E, 0x002c, 0 }, //	BLACK_LEVEL_1ST_GREEN2
	{ 0x3280, 0x002c, 0 }, //	BLACK_LEVEL_1ST_BLUE
	{ 0x098C, 0xA348, 0 }, //	MCU_ADDRESS	[AWB_GAIN_BUFFER_SPEED	[MCU_DATA_0]]
	{ 0x0990, 0x0008, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA349, 0 }, //	MCU_ADDRESS	[AWB_JUMP_DIVISOR	[MCU_DATA_0]]
	{ 0x0990, 0x0002, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA34A, 0 }, //	MCU_ADDRESS	[AWB_GAIN_MIN	[MCU_DATA_0]]
	{ 0x0990, 0x0059, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA34B, 0 }, //	MCU_ADDRESS	[AWB_GAIN_MAX	[MCU_DATA_0]]
	{ 0x0990, 0x00A6, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA351, 0 }, //	MCU_ADDRESS	[AWB_CCM_POSITION_MIN	[MCU_DATA_0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA352, 0 }, //	MCU_ADDRESS	[AWB_CCM_POSITION_MAX	[MCU_DATA_0]]
	{ 0x0990, 0x007F, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA354, 0 }, //	MCU_ADDRESS	[AWB_SATURATION	[MCU_DATA_0]]
	{ 0x0990, 0x0043, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA355, 0 }, //	MCU_ADDRESS	[AWB_MODE	[MCU_DATA_0]]
	{ 0x0990, 0x0001, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA35D, 0 }, //	MCU_ADDRESS	[AWB_STEADY_BGAIN_OUT_MIN	[MCU_DATA_0]]
	{ 0x0990, 0x0078, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA35E, 0 }, //	MCU_ADDRESS	[AWB_STEADY_BGAIN_OUT_MAX	[MCU_DATA_0]]
	{ 0x0990, 0x0086, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA35F, 0 }, //	MCU_ADDRESS	[AWB_STEADY_BGAIN_IN_MIN	[MCU_DATA_0]]
	{ 0x0990, 0x007E, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA360, 0 }, //	MCU_ADDRESS	[AWB_STEADY_BGAIN_IN_MAX	[MCU_DATA_0]]
	{ 0x0990, 0x0082, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2361, 0 }, //	MCU_ADDRESS	[RESERVED_AWB_61	[MCU_DATA_0]]
	{ 0x0990, 0x0040, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA363, 0 }, //	MCU_ADDRESS	[RESERVED_AWB_63	[MCU_DATA_0]]
	{ 0x0990, 0x00D2, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA364, 0 }, //	MCU_ADDRESS	[RESERVED_AWB_64	[MCU_DATA_0]]
	{ 0x0990, 0x00F6, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA302, 0 }, //	MCU_ADDRESS	[AWB_WINDOW_POS	[MCU_DATA_0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA303, 0 }, //	MCU_ADDRESS	[AWB_WINDOW_SIZE	[MCU_DATA_0]]
	{ 0x0990, 0x00EF, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA366, 0 }, //	MCU_ADDRESS	[AWB_KR_L	[MCU_DATA_0]]
	{ 0x0990, 0x00B6, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA367, 0 }, //	MCU_ADDRESS	[AWB_KG_L	[MCU_DATA_0]]
	{ 0x0990, 0x0096, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA368, 0 }, //	MCU_ADDRESS	[AWB_KB_L	[MCU_DATA_0]]
	{ 0x0990, 0x0080, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA369, 0 }, //	MCU_ADDRESS	[AWB_KR_R	[MCU_DATA_0]]
	{ 0x0990, 0x0087, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA36A, 0 }, //	MCU_ADDRESS	[AWB_KG_R	[MCU_DATA_0]]
	{ 0x0990, 0x0082, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA36B, 0 }, //	MCU_ADDRESS	[AWB_KB_R	[MCU_DATA_0]]
	{ 0x0990, 0x0082, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB04, 0 }, //	MCU_ADDRESS	[HG_MAX_DLEVEL	[MCU_DATA_0]]
	{ 0x0990, 0x001D, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB06, 0 }, //	MCU_ADDRESS	[HG_PERCENT	[MCU_DATA_0]]
	{ 0x0990, 0x0009, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB08, 0 }, //	MCU_ADDRESS	[HG_DLEVEL	[MCU_DATA_0]]
	{ 0x0990, 0x0023, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB20, 0 }, //	MCU_ADDRESS	[HG_LL_SAT1	[MCU_DATA_0]]
	{ 0x0990, 0x0038, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB21, 0 }, //	MCU_ADDRESS	[HG_LL_INTERPTHRESH1	[MCU_DATA_0]]
	{ 0x0990, 0x0020, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB22, 0 }, //	MCU_ADDRESS	[HG_LL_APCORR1	[MCU_DATA_0]]
	{ 0x0990, 0x0004, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB23, 0 }, //	MCU_ADDRESS	[HG_LL_APTHRESH1	[MCU_DATA_0]]
	{ 0x0990, 0x000A, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB24, 0 }, //	MCU_ADDRESS	[HG_LL_SAT2	[MCU_DATA_0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB25, 0 }, //	MCU_ADDRESS	[HG_LL_INTERPTHRESH2	[MCU_DATA_0]]
	{ 0x0990, 0x0014, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB26, 0 }, //	MCU_ADDRESS	[HG_LL_APCORR2	[MCU_DATA_0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB27, 0 }, //	MCU_ADDRESS	[HG_LL_APTHRESH2	[MCU_DATA_0]]
	{ 0x0990, 0x0015, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2B28, 0 }, //	MCU_ADDRESS	[HG_LL_BRIGHTNESSSTART	[MCU_DATA_0]]
	{ 0x0990, 0x2000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x2B2A, 0 }, //	MCU_ADDRESS	[HG_LL_BRIGHTNESSSTOP	[MCU_DATA_0]]
	{ 0x0990, 0x7000, 0 }, //	MCU_DATA_0
	{ 0x326C, 0x1515, 0 }, //	APERTURE_PARAMETERS
	{ 0x326C, 0x1500, 0 }, //	APERTURE_PARAMETERS
	{ 0x098C, 0xAB2C, 0 }, //	MCU_ADDRESS	[HG_NR_START_R	[MCU_DATA_0]]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB2D, 0 }, //	MCU_ADDRESS	[HG_NR_START_G	[MCU_DATA_0]]
	{ 0x0990, 0x0015, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB2E, 0 }, //	MCU_ADDRESS	[HG_NR_START_B	[MCU_DATA_0]]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB2F, 0 }, //	MCU_ADDRESS	[HG_NR_START_OL	[MCU_DATA_0]]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB30, 0 }, //	MCU_ADDRESS	[HG_NR_STOP_R	[MCU_DATA_0]]
	{ 0x0990, 0x001E, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB31, 0 }, //	MCU_ADDRESS	[HG_NR_STOP_G	[MCU_DATA_0]]
	{ 0x0990, 0x000E, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB32, 0 }, //	MCU_ADDRESS	[HG_NR_STOP_B	[MCU_DATA_0]]
	{ 0x0990, 0x001E, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB33, 0 }, //	MCU_ADDRESS	[HG_NR_STOP_OL	[MCU_DATA_0]]
	{ 0x0990, 0x001E, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB34, 0 }, //	MCU_ADDRESS	[HG_NR_GAINSTART	[MCU_DATA_0]]
	{ 0x0990, 0x0008, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xAB35, 0 }, //	MCU_ADDRESS	[HG_NR_GAINSTOP	[MCU_DATA_0]]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD	[MCU_DATA_0]]
	{ 0x0990, 0x0006, 30 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD	[MCU_DATA_0]]
	{ 0x0990, 0x0005, 30 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_preview_regs[] =
{
 	{ 0x098C, 0xA115, 0 }, //	MCU_ADDRESS	[SEQ_CAP_MODE]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0001, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA404, 0 }, //	MCU_ADDRESS	[FD_MODE]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_capture_regs[] =
{
	{ 0x098C, 0xA115, 0 }, //	MCU_ADDRESS	[SEQ_CAP_MODE]
	{ 0x0990, 0x0072, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0002, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA404, 0 }, //	MCU_ADDRESS	[FD_MODE]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_wb_auto_regs[] =
{
	{ 0x098C, 0xA351, 0 }, //	AWB_CCM_POSITION_MIN
	{ 0x0990, 0x0000, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA352, 0 }, //	AWB_CCM_POSITION_MAX
	{ 0x0990, 0x007F, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34A, 0 }, //	AWB_GAINMIN_R
	{ 0x0990, 0x0059, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34B, 0 }, //	AWB_GAINMAX_R
	{ 0x0990, 0x00A6, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34C, 0 }, //	AWB_GAINMIN_B
	{ 0x0990, 0x0059, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34D, 0 }, //	AWB_GAINMAX_B
	{ 0x0990, 0x00A6, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_wb_daylight_regs[] =
{
	{ 0x098C, 0xA351, 0 }, //	AWB_CCM_POSITION_MIN
	{ 0x0990, 0x007F, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA352, 0 }, //	AWB_CCM_POSITION_MAX
	{ 0x0990, 0x007F, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA353, 0 }, //	AWB_CCM_POSITION
	{ 0x0990, 0x007F, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34A, 0 }, //	AWB_GAINMIN_R
	{ 0x0990, 0x00A6, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34B, 0 }, //	AWB_GAINMAX_R
	{ 0x0990, 0x00A6, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34C, 0 }, //	AWB_GAINMIN_B
	{ 0x0990, 0x007C, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34D, 0 }, //	AWB_GAINMAX_B
	{ 0x0990, 0x007C, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_wb_incandescent_regs[] =
{
	{ 0x098C, 0xA351, 0 }, //	AWB_CCM_POSITION_MIN
	{ 0x0990, 0x0003, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA352, 0 }, //	AWB_CCM_POSITION_MAX
	{ 0x0990, 0x0003, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA353, 0 }, //	AWB_CCM_POSITION
	{ 0x0990, 0x0003, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34A, 0 }, //	AWB_GAINMIN_R
	{ 0x0990, 0x00AD, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34B, 0 }, //	AWB_GAINMAX_R
	{ 0x0990, 0x00AD, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34C, 0 }, //	AWB_GAINMIN_B
	{ 0x0990, 0x0081, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34D, 0 }, //	AWB_GAINMAX_B
	{ 0x0990, 0x0081, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_wb_fluorescen_regs[] =
{
	{ 0x098C, 0xA351, 0 }, //	AWB_CCM_POSITION_MIN
	{ 0x0990, 0x004E, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA352, 0 }, //	AWB_CCM_POSITION_MAX
	{ 0x0990, 0x004E, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA353, 0 }, //	AWB_CCM_POSITION
	{ 0x0990, 0x004E, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34A, 0 }, //	AWB_GAINMIN_R
	{ 0x0990, 0x009A, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34B, 0 }, //	AWB_GAINMAX_R
	{ 0x0990, 0x009A, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34C, 0 }, //	AWB_GAINMIN_B
	{ 0x0990, 0x0084, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34D, 0 }, //	AWB_GAINMAX_B
	{ 0x0990, 0x0084, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_wb_cloudy_regs[] =
{
	{ 0x098C, 0xA351, 0 }, //	AWB_CCM_POSITION_MIN
	{ 0x0990, 0x007F, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA352, 0 }, //	AWB_CCM_POSITION_MAX
	{ 0x0990, 0x007F, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA353, 0 }, //	AWB_CCM_POSITION
	{ 0x0990, 0x007F, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34A, 0 }, //	AWB_GAINMIN_R
	{ 0x0990, 0x00AF, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34B, 0 }, //	AWB_GAINMAX_R
	{ 0x0990, 0x00AF, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34C, 0 }, //	AWB_GAINMIN_B
	{ 0x0990, 0x0079, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA34D, 0 }, //	AWB_GAINMAX_B
	{ 0x0990, 0x0079, 0 }, //	MCU_ADDRESS
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_effect_none_regs[] =
{
{ 0x098C, 0x2759, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_A]
	{ 0x0990, 0x6440, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x275B, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_B]
	{ 0x0990, 0x6440, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0005, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_mono_regs[] =
{
	{ 0x098C, 0x2759, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_A]
	{ 0x0990, 0x6441, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x275B, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_B]
	{ 0x0990, 0x6441, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0005, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_sepia_regs[] =
{
	{ 0x098C, 0x2759, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_A]
	{ 0x0990, 0x6442, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x275B, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_B]
	{ 0x0990, 0x6442, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0005, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_negative_regs[] =
{
	{ 0x098C, 0x2759, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_A]
	{ 0x0990, 0x6443, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x275B, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_B]
	{ 0x0990, 0x6443, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0005, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_solarize_regs[] =
{
	{ 0x098C, 0x2759, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_A]
	{ 0x0990, 0x6445, 0 }, //	MCU_DATA_0
	{ 0x098C, 0x275B, 0 }, //	MCU_ADDRESS	[MODE_SPEC_EFFECTS_B]
	{ 0x0990, 0x6445, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0005, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg mt9m113_bright_regs[] =
{
	{ 0x098C, 0xA24F, 0 }, //	MCU_ADDRESS
	{ 0x0990, 0x0000, 0 }, //	AE_BASETARGET
	{ 0x098C, 0xA207, 0 }, //	MCU_ADDRESS
	{ 0x0990, 0x0000, 0 }, //	AE_GATE
};

struct mt9m113_reg const mt9m113_window_average_regs[] =
{
	{ 0x098C, 0xA202, 0 }, //	MCU_ADDRESS	[AE_WINDOW_POS]
	{ 0x0990, 0x0000, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA203, 0 }, //	MCU_ADDRESS	[AE_WINDOW_SIZE]
	{ 0x0990, 0x00FF, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_window_center_regs[] =
{
	{ 0x098C, 0xA202, 0 }, //	MCU_ADDRESS	[AE_WINDOW_POS]
	{ 0x0990, 0x0022, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA203, 0 }, //	MCU_ADDRESS	[AE_WINDOW_SIZE]
	{ 0x0990, 0x00BB, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_reg const mt9m113_window_spot_regs[] =
{
	{ 0x098C, 0xA202, 0 }, //	MCU_ADDRESS	[AE_WINDOW_POS]
	{ 0x0990, 0x0056, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA203, 0 }, //	MCU_ADDRESS	[AE_WINDOW_SIZE]
	{ 0x0990, 0x0053, 0 }, //	MCU_DATA_0
	{ 0x098C, 0xA103, 0 }, //	MCU_ADDRESS	[SEQ_CMD]
	{ 0x0990, 0x0006, 0 }, //	MCU_DATA_0
};

struct mt9m113_tbl const mt9m113_wb_tbl[] =
{
	{ mt9m113_wb_auto_regs, ARRAY_SIZE(mt9m113_wb_auto_regs) }, // WHITE_BALANCE_AUTO
	{ mt9m113_wb_daylight_regs, ARRAY_SIZE(mt9m113_wb_daylight_regs) }, // WHITE_BALANCE_SUNNY
	{ mt9m113_wb_cloudy_regs, ARRAY_SIZE(mt9m113_wb_cloudy_regs) }, // WHITE_BALANCE_CLOUDY
	{ mt9m113_wb_incandescent_regs, ARRAY_SIZE(mt9m113_wb_incandescent_regs) }, // WHITE_BALANCE_TUNGSTEN
	{ mt9m113_wb_fluorescen_regs, ARRAY_SIZE(mt9m113_wb_fluorescen_regs) }, // WHITE_BALANCE_FLUORESCENT
};

struct mt9m113_tbl const mt9m113_effect_tbl[] =
{
	{ mt9m113_effect_none_regs, ARRAY_SIZE(mt9m113_effect_none_regs) }, // IMAGE_EFFECT_NONE
	{ mt9m113_mono_regs, ARRAY_SIZE(mt9m113_mono_regs) }, // IMAGE_EFFECT_NONE
	{ mt9m113_sepia_regs, ARRAY_SIZE(mt9m113_sepia_regs) }, // IMAGE_EFFECT_SEPIA
	{NULL, 0}, // IMAGE_EFFECT_AQUA
	{NULL, 0}, // IMAGE_EFFECT_ANTIQUE
	{ mt9m113_negative_regs, ARRAY_SIZE(mt9m113_negative_regs) }, // IMAGE_EFFECT_NEGATIVE
	{ mt9m113_solarize_regs, ARRAY_SIZE(mt9m113_solarize_regs) }, // IMAGE_EFFECT_SHARPEN
};

struct mt9m113_tbl const mt9m113_window_tbl[] =
{
	{ mt9m113_window_average_regs, ARRAY_SIZE(mt9m113_window_average_regs) },
	{ mt9m113_window_center_regs, ARRAY_SIZE(mt9m113_window_center_regs) },
	{ mt9m113_window_spot_regs, ARRAY_SIZE(mt9m113_window_spot_regs) },
};

unsigned short const mt9m113_brightness[9][2] =
{
	{ 0x0018, 0x0002 }, // EV_MINUS_4
	{ 0x0026, 0x0004 }, // EV_MINUS_3
	{ 0x0034, 0x0005 }, // EV_MINUS_2
	{ 0x0042, 0x0006 }, // EV_MINUS_1
	{ 0x0055, 0x0006 }, // EV_DEFAULT
	{ 0x005F, 0x0006 }, // EV_PLUS_1
	{ 0x006A, 0x0000 }, // EV_PLUS_2
	{ 0x0077, 0x0000 }, // EV_PLUS_3
	{ 0x0084, 0x0000 }, // EV_PLUS_4
};

#endif

