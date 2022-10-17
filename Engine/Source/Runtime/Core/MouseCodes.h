#pragma once

namespace X
{
	typedef enum class MouseCode : uint16_t
	{
		// From glfw3.h
		Button0 = 0,
		Button1 = 1,
		Button2 = 2,
		Button3 = 3,
		Button4 = 4,
		Button5 = 5,
		Button6 = 6,
		Button7 = 7,

		ButtonLast = Button7,
		ButtonLeft = Button0,
		ButtonRight = Button1,
		ButtonMiddle = Button2
	} Mouse;

	inline std::ostream& operator<<(std::ostream& os, MouseCode mouseCode)
	{
		os << static_cast<int32_t>(mouseCode);
		return os;
	}
}

#define X_MOUSE_BUTTON_0      ::X::Mouse::Button0
#define X_MOUSE_BUTTON_1      ::X::Mouse::Button1
#define X_MOUSE_BUTTON_2      ::X::Mouse::Button2
#define X_MOUSE_BUTTON_3      ::X::Mouse::Button3
#define X_MOUSE_BUTTON_4      ::X::Mouse::Button4
#define X_MOUSE_BUTTON_5      ::X::Mouse::Button5
#define X_MOUSE_BUTTON_6      ::X::Mouse::Button6
#define X_MOUSE_BUTTON_7      ::X::Mouse::Button7
#define X_MOUSE_BUTTON_LAST   ::X::Mouse::ButtonLast
#define X_MOUSE_BUTTON_LEFT   ::X::Mouse::ButtonLeft
#define X_MOUSE_BUTTON_RIGHT  ::X::Mouse::ButtonRight
#define X_MOUSE_BUTTON_MIDDLE ::X::Mouse::ButtonMiddle