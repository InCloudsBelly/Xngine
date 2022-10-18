#pragma once

#include "Event.h"
#include "Runtime/Input/KeyCodes.h"

namespace X 
{
	class XNGINE_API KeyEvent : public Event
	{
	public:
		[[nodiscard]] KeyCode GetKeyCode() const { return mKeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(const KeyCode keycode)
			: mKeyCode(keycode) {}

		KeyCode mKeyCode;
	};

	class XNGINE_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(const KeyCode keycode, const uint16_t repeatCount)
			: KeyEvent(keycode), mRepeatCount(repeatCount) {}

		uint16_t GetRepeatCount() const { return mRepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		uint16_t mRepeatCount;
	};

	class XNGINE_API KeyReleasedEvent : public KeyEvent
	{
	public:
		KeyReleasedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};

	class XNGINE_API KeyTypedEvent : public KeyEvent
	{
	public:
		KeyTypedEvent(const KeyCode keycode)
			: KeyEvent(keycode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyTypedEvent: " << mKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyTyped)
	};
}