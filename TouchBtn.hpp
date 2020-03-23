#ifndef __TOUCH_BTN_H__
#define __TOUCH_BTN_H__

#include <Arduino.h>
#include <functional>
#include <driver/touch_pad.h>

using CallBackType = std::function<void(void)>;
enum _TouchBtnState_
{
	TOUCH_BTN_PRESSED	= 1<<0,
	TOUCH_BTN_CHECKED	= 1<<1,
	TOUCH_BTN_REPEAT	= 1<<2
};

class TouchBtn
{
	public:
		TouchBtn();
		~TouchBtn();

		void begin(void);

		void calibrate(void);

		void config_btn(uint8_t TouchPad_Id, const uint8_t threshold=75, uint16_t repeat=0, CallBackType CallBack=nullptr);
		void delete_btn(uint8_t TouchPad_Id);

		uint8_t state(uint8_t TouchPad_Id);
		uint8_t check(uint8_t TouchPad_Id);

		uint8_t value(uint8_t TouchPad_Id) {return TouchBtn::Value_Pct[TouchPad_Id];}
		uint8_t count(uint8_t TouchPad_Id) {return TouchBtn::Count[TouchPad_Id];}

	protected:
		static void TouchBtn_Read_Filter_cb(uint16_t *raw_value, uint16_t *filtered_value);

		static constexpr int filter_period = 10;

		static bool Enabled[TOUCH_PAD_MAX];

		static uint16_t Value_Max[TOUCH_PAD_MAX];

		static uint16_t Value_Filtered[TOUCH_PAD_MAX];
		static uint16_t Value_Raw[TOUCH_PAD_MAX];
		static uint8_t Value_Pct[TOUCH_PAD_MAX];
		static uint8_t Threshold[TOUCH_PAD_MAX];

		static uint16_t Count[TOUCH_PAD_MAX];
		static bool Pressed[TOUCH_PAD_MAX];
		static bool Checked[TOUCH_PAD_MAX];
		static uint16_t Repeat[TOUCH_PAD_MAX];

		static CallBackType CallBackFcn[TOUCH_PAD_MAX];
};

#endif