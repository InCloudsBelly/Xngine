#pragma once

namespace X
{
	typedef enum class KeyCode : uint16_t
	{
		// From glfw3.h
		Space = 32,
		Apostrophe = 39, /* ' */
		Comma = 44, /* , */
		Minus = 45, /* - */
		Period = 46, /* . */
		Slash = 47, /* / */

		D0 = 48, /* 0 */
		D1 = 49, /* 1 */
		D2 = 50, /* 2 */
		D3 = 51, /* 3 */
		D4 = 52, /* 4 */
		D5 = 53, /* 5 */
		D6 = 54, /* 6 */
		D7 = 55, /* 7 */
		D8 = 56, /* 8 */
		D9 = 57, /* 9 */

		Semicolon = 59, /* ; */
		Equal = 61, /* = */

		A = 65,
		B = 66,
		C = 67,
		D = 68,
		E = 69,
		F = 70,
		G = 71,
		H = 72,
		I = 73,
		J = 74,
		K = 75,
		L = 76,
		M = 77,
		N = 78,
		O = 79,
		P = 80,
		Q = 81,
		R = 82,
		S = 83,
		T = 84,
		U = 85,
		V = 86,
		W = 87,
		X = 88,
		Y = 89,
		Z = 90,

		LeftBracket = 91,  /* [ */
		Backslash = 92,  /* \ */
		RightBracket = 93,  /* ] */
		GraveAccent = 96,  /* ` */

		World1 = 161, /* non-US #1 */
		World2 = 162, /* non-US #2 */

		/* Function keys */
		Escape = 256,
		Enter = 257,
		Tab = 258,
		Backspace = 259,
		Insert = 260,
		Delete = 261,
		Right = 262,
		Left = 263,
		Down = 264,
		Up = 265,
		PageUp = 266,
		PageDown = 267,
		Home = 268,
		End = 269,
		CapsLock = 280,
		ScrollLock = 281,
		NumLock = 282,
		PrintScreen = 283,
		Pause = 284,
		F1 = 290,
		F2 = 291,
		F3 = 292,
		F4 = 293,
		F5 = 294,
		F6 = 295,
		F7 = 296,
		F8 = 297,
		F9 = 298,
		F10 = 299,
		F11 = 300,
		F12 = 301,
		F13 = 302,
		F14 = 303,
		F15 = 304,
		F16 = 305,
		F17 = 306,
		F18 = 307,
		F19 = 308,
		F20 = 309,
		F21 = 310,
		F22 = 311,
		F23 = 312,
		F24 = 313,
		F25 = 314,

		/* Keypad */
		KP0 = 320,
		KP1 = 321,
		KP2 = 322,
		KP3 = 323,
		KP4 = 324,
		KP5 = 325,
		KP6 = 326,
		KP7 = 327,
		KP8 = 328,
		KP9 = 329,
		KPDecimal = 330,
		KPDivide = 331,
		KPMultiply = 332,
		KPSubtract = 333,
		KPAdd = 334,
		KPEnter = 335,
		KPEqual = 336,

		LeftShift = 340,
		LeftControl = 341,
		LeftAlt = 342,
		LeftSuper = 343,
		RightShift = 344,
		RightControl = 345,
		RightAlt = 346,
		RightSuper = 347,
		Menu = 348
	} Key;

	inline std::ostream& operator<<(std::ostream& os, KeyCode keyCode)
	{
		os << static_cast<int32_t>(keyCode);
		return os;
	}
}

// From glfw3.h
#define X_KEY_SPACE           ::X::Key::Space
#define X_KEY_APOSTROPHE      ::X::Key::Apostrophe    /* ' */
#define X_KEY_COMMA           ::X::Key::Comma         /* , */
#define X_KEY_MINUS           ::X::Key::Minus         /* - */
#define X_KEY_PERIOD          ::X::Key::Period        /* . */
#define X_KEY_SLASH           ::X::Key::Slash         /* / */
#define X_KEY_0               ::X::Key::D0
#define X_KEY_1               ::X::Key::D1
#define X_KEY_2               ::X::Key::D2
#define X_KEY_3               ::X::Key::D3
#define X_KEY_4               ::X::Key::D4
#define X_KEY_5               ::X::Key::D5
#define X_KEY_6               ::X::Key::D6
#define X_KEY_7               ::X::Key::D7
#define X_KEY_8               ::X::Key::D8
#define X_KEY_9               ::X::Key::D9
#define X_KEY_SEMICOLON       ::X::Key::Semicolon     /* ; */
#define X_KEY_EQUAL           ::X::Key::Equal         /* = */
#define X_KEY_A               ::X::Key::A
#define X_KEY_B               ::X::Key::B
#define X_KEY_C               ::X::Key::C
#define X_KEY_D               ::X::Key::D
#define X_KEY_E               ::X::Key::E
#define X_KEY_F               ::X::Key::F
#define X_KEY_G               ::X::Key::G
#define X_KEY_H               ::X::Key::H
#define X_KEY_I               ::X::Key::I
#define X_KEY_J               ::X::Key::J
#define X_KEY_K               ::X::Key::K
#define X_KEY_L               ::X::Key::L
#define X_KEY_M               ::X::Key::M
#define X_KEY_N               ::X::Key::N
#define X_KEY_O               ::X::Key::O
#define X_KEY_P               ::X::Key::P
#define X_KEY_Q               ::X::Key::Q
#define X_KEY_R               ::X::Key::R
#define X_KEY_S               ::X::Key::S
#define X_KEY_T               ::X::Key::T
#define X_KEY_U               ::X::Key::U
#define X_KEY_V               ::X::Key::V
#define X_KEY_W               ::X::Key::W
#define X_KEY_X               ::X::Key::X
#define X_KEY_Y               ::X::Key::Y
#define X_KEY_Z               ::X::Key::Z
#define X_KEY_LEFT_BRACKET    ::X::Key::LeftBracket   /* [ */
#define X_KEY_BACKSLASH       ::X::Key::Backslash     /* \ */
#define X_KEY_RIGHT_BRACKET   ::X::Key::RightBracket  /* ] */
#define X_KEY_GRAVE_ACCENT    ::X::Key::GraveAccent   /* ` */
#define X_KEY_WORLD_1         ::X::Key::World1        /* non-US #1 */
#define X_KEY_WORLD_2         ::X::Key::World2        /* non-US #2 */

/* Function keys */
#define X_KEY_ESCAPE          ::X::Key::Escape
#define X_KEY_ENTER           ::X::Key::Enter
#define X_KEY_TAB             ::X::Key::Tab
#define X_KEY_BACKSPACE       ::X::Key::Backspace
#define X_KEY_INSERT          ::X::Key::Insert
#define X_KEY_DELETE          ::X::Key::Delete
#define X_KEY_RIGHT           ::X::Key::Right
#define X_KEY_LEFT            ::X::Key::Left
#define X_KEY_DOWN            ::X::Key::Down
#define X_KEY_UP              ::X::Key::Up
#define X_KEY_PAGE_UP         ::X::Key::PageUp
#define X_KEY_PAGE_DOWN       ::X::Key::PageDown
#define X_KEY_HOME            ::X::Key::Home
#define X_KEY_END             ::X::Key::End
#define X_KEY_CAPS_LOCK       ::X::Key::CapsLock
#define X_KEY_SCROLL_LOCK     ::X::Key::ScrollLock
#define X_KEY_NUM_LOCK        ::X::Key::NumLock
#define X_KEY_PRINT_SCREEN    ::X::Key::PrintScreen
#define X_KEY_PAUSE           ::X::Key::Pause
#define X_KEY_F1              ::X::Key::F1
#define X_KEY_F2              ::X::Key::F2
#define X_KEY_F3              ::X::Key::F3
#define X_KEY_F4              ::X::Key::F4
#define X_KEY_F5              ::X::Key::F5
#define X_KEY_F6              ::X::Key::F6
#define X_KEY_F7              ::X::Key::F7
#define X_KEY_F8              ::X::Key::F8
#define X_KEY_F9              ::X::Key::F9
#define X_KEY_F10             ::X::Key::F10
#define X_KEY_F11             ::X::Key::F11
#define X_KEY_F12             ::X::Key::F12
#define X_KEY_F13             ::X::Key::F13
#define X_KEY_F14             ::X::Key::F14
#define X_KEY_F15             ::X::Key::F15
#define X_KEY_F16             ::X::Key::F16
#define X_KEY_F17             ::X::Key::F17
#define X_KEY_F18             ::X::Key::F18
#define X_KEY_F19             ::X::Key::F19
#define X_KEY_F20             ::X::Key::F20
#define X_KEY_F21             ::X::Key::F21
#define X_KEY_F22             ::X::Key::F22
#define X_KEY_F23             ::X::Key::F23
#define X_KEY_F24             ::X::Key::F24
#define X_KEY_F25             ::X::Key::F25

/* Keypad */
#define X_KEY_KP_0            ::X::Key::KP0
#define X_KEY_KP_1            ::X::Key::KP1
#define X_KEY_KP_2            ::X::Key::KP2
#define X_KEY_KP_3            ::X::Key::KP3
#define X_KEY_KP_4            ::X::Key::KP4
#define X_KEY_KP_5            ::X::Key::KP5
#define X_KEY_KP_6            ::X::Key::KP6
#define X_KEY_KP_7            ::X::Key::KP7
#define X_KEY_KP_8            ::X::Key::KP8
#define X_KEY_KP_9            ::X::Key::KP9
#define X_KEY_KP_DECIMAL      ::X::Key::KPDecimal
#define X_KEY_KP_DIVIDE       ::X::Key::KPDivide
#define X_KEY_KP_MULTIPLY     ::X::Key::KPMultiply
#define X_KEY_KP_SUBTRACT     ::X::Key::KPSubtract
#define X_KEY_KP_ADD          ::X::Key::KPAdd
#define X_KEY_KP_ENTER        ::X::Key::KPEnter
#define X_KEY_KP_EQUAL        ::X::Key::KPEqual

#define X_KEY_LEFT_SHIFT      ::X::Key::LeftShift
#define X_KEY_LEFT_CONTROL    ::X::Key::LeftControl
#define X_KEY_LEFT_ALT        ::X::Key::LeftAlt
#define X_KEY_LEFT_SUPER      ::X::Key::LeftSuper
#define X_KEY_RIGHT_SHIFT     ::X::Key::RightShift
#define X_KEY_RIGHT_CONTROL   ::X::Key::RightControl
#define X_KEY_RIGHT_ALT       ::X::Key::RightAlt
#define X_KEY_RIGHT_SUPER     ::X::Key::RightSuper
#define X_KEY_MENU            ::X::Key::Menu