#ifndef INPUTSTRUCTURE_H
#define INPUTSTRUCTURE_H

struct KeyboardStateStruct
{
	bool key_w_pressed = false;
	bool key_a_pressed = false;
	bool key_s_pressed = false;
	bool key_d_pressed = false;
	bool key_enter_pressed = false;
	bool key_space_pressed = false;
	bool key_escape_pressed = false;
};

struct MouseStateStruct
{
	float x_pos = 0.0f;
	float y_pos = 0.0f;
	bool btn_left_pressed = false;
	bool btn_right_pressed = false;
};

#endif