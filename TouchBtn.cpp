#include "TouchBtn.hpp"


TouchBtn::TouchBtn()
{
	touch_pad_init();
	touch_pad_set_fsm_mode(TOUCH_FSM_MODE_DEFAULT);
	touch_pad_set_voltage(TOUCH_HVOLT_2V7, TOUCH_LVOLT_0V5, TOUCH_HVOLT_ATTEN_1V);
	for (int i=0; i<TOUCH_PAD_MAX; ++i)
	{
		TouchBtn::Enabled[i] = false;
		
		TouchBtn::Value_Max[i]=0;

		TouchBtn::Value_Filtered[i]=0;
		TouchBtn::Value_Raw[i]=0;
		TouchBtn::Value_Pct[i]=0;
		TouchBtn::Threshold[i]=0;

		TouchBtn::Count[i] = 0;
		TouchBtn::Repeat[i] = 0;
		TouchBtn::Pressed[i]=false;
		TouchBtn::Checked[i]=false;

		TouchBtn::CallBackFcn[i]=nullptr;
	}
}
TouchBtn::~TouchBtn()
{
	touch_pad_deinit();
}

void TouchBtn::begin(void)
{
	for (int i=0; i<TOUCH_PAD_MAX; ++i)
		if(TouchBtn::Enabled[i])
			touch_pad_config(static_cast<touch_pad_t>(i), 0);
	
	touch_pad_filter_start(TouchBtn::filter_period);

	TouchBtn::calibrate();

	touch_pad_set_filter_read_cb(TouchBtn_Read_Filter_cb);
}

void TouchBtn::calibrate(void)
{
	uint16_t val;
	for (int i=0; i<TOUCH_PAD_MAX; ++i)
		if(TouchBtn::Enabled[i])
		{
			touch_pad_read_filtered(static_cast<touch_pad_t>(i), &val);
			TouchBtn::Value_Max[i] = val;
		}
}

void TouchBtn::config_btn(uint8_t TouchPad_Id, const uint8_t threshold, uint16_t repeat, CallBackType CallBack)
{
	TouchBtn::Enabled[TouchPad_Id] = true;
	TouchBtn::Threshold[TouchPad_Id] = threshold;
	TouchBtn::Repeat[TouchPad_Id] = repeat;
	TouchBtn::CallBackFcn[TouchPad_Id] = CallBack;
}

void TouchBtn::delete_btn(uint8_t TouchPad_Id)
{
	TouchBtn::Enabled[TouchPad_Id] = false;
	TouchBtn::Threshold[TouchPad_Id] = 0;
	TouchBtn::Repeat[TouchPad_Id] = 0;
	TouchBtn::CallBackFcn[TouchPad_Id] = nullptr;
}

uint8_t TouchBtn::state(uint8_t TouchPad_Id)
{
	uint8_t ret=0;
	if(TouchBtn::Pressed[TouchPad_Id])
		ret += TOUCH_BTN_PRESSED;
	if(TouchBtn::Checked[TouchPad_Id])
		ret += TOUCH_BTN_CHECKED;
	if(TouchBtn::Repeat[TouchPad_Id] && TouchBtn::Count[TouchPad_Id] > TouchBtn::Repeat[TouchPad_Id])
		ret += TOUCH_BTN_REPEAT;
	
	return ret;
}
uint8_t  TouchBtn::check(uint8_t TouchPad_Id)
{
	if (TouchBtn::Pressed[TouchPad_Id])
	{
		TouchBtn::Checked[TouchPad_Id] = true;
		return 1;
	}
	else if(TouchBtn::Checked[TouchPad_Id] && TouchBtn::Repeat[TouchPad_Id])
	{
		if(TouchBtn::Count[TouchPad_Id] > TouchBtn::Repeat[TouchPad_Id])
			return 2;
		else
			return 0;
	}
	return 0;
}

void TouchBtn::TouchBtn_Read_Filter_cb(uint16_t *raw_value, uint16_t *filtered_value)
{
	for (int i=0; i<TOUCH_PAD_MAX; ++i)
		if(TouchBtn::Enabled[i])
		{
			TouchBtn::Value_Filtered[i]=filtered_value[i];
			TouchBtn::Value_Raw[i]=raw_value[i];
			TouchBtn::Value_Pct[i] = TouchBtn::Value_Max[i]?(100*TouchBtn::Value_Filtered[i]/TouchBtn::Value_Max[i]):0;
			
			if(TouchBtn::Value_Pct[i] < TouchBtn::Threshold[i])
			{
				if(TouchBtn::Checked[i])
					TouchBtn::Pressed[i] = false;
				else
					TouchBtn::Pressed[i] = true;

				TouchBtn::Count[i]++;
			}
			else
			{
				if(TouchBtn::Checked[i])	TouchBtn::Pressed[i]=false;
				TouchBtn::Count[i] = 0;
				TouchBtn::Checked[i] = false;
			}
		}
}

bool TouchBtn::Enabled[TOUCH_PAD_MAX];

uint16_t TouchBtn::Value_Max[TOUCH_PAD_MAX];

uint16_t TouchBtn::Value_Filtered[TOUCH_PAD_MAX];
uint16_t TouchBtn::Value_Raw[TOUCH_PAD_MAX];
uint8_t TouchBtn::Value_Pct[TOUCH_PAD_MAX];
uint8_t TouchBtn::Threshold[TOUCH_PAD_MAX];

uint16_t TouchBtn::Count[TOUCH_PAD_MAX];
bool TouchBtn::Pressed[TOUCH_PAD_MAX];
bool TouchBtn::Checked[TOUCH_PAD_MAX];
uint16_t TouchBtn::Repeat[TOUCH_PAD_MAX];

CallBackType TouchBtn::CallBackFcn[TOUCH_PAD_MAX];