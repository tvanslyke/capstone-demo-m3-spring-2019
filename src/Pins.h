#ifndef INO_PINS_H
#define INO_PINS_H

#include <Arduino.h>
#include "ino_assert.h"
#include "Array.h"
#include <utility>
#include <bitset>

namespace ino {

enum class PinKind {
	Digital,
	DigitalPWM,
	Analog
};

enum class LogicLevel: int {
	High = HIGH,
	Low = LOW
};

enum class PinMode: int {
	Input = INPUT,
	Output = OUTPUT,
	InputPullup = INPUT_PULLUP
};

enum class PinStatus {
	Good,
	BadPinMode,
	BadPinKind,
	BadAnalogWriteValue,
};

/**
 * Encapsulates the 
 */
struct CheckedPin {

	template <int Number>
	[[nodiscard]]
	static constexpr CheckedPin from_pin_number() noexcept {
		static_assert(CheckedPin(Number).is_valid(), "Invalid pin number.");
		return CheckedPin(Number);
	}


	static constexpr int analog_write_minm = 0;
	static constexpr int analog_write_maxm = 255;

	constexpr CheckedPin() = delete;

	[[nodiscard]]
	constexpr int number() const {
		return number_;
	}

	[[nodiscard]]
	constexpr const char* name() const {
		switch(number()) {
		default:
			UNREACHABLE();
			return nullptr;
		case  0:  return "0";
		case  1:  return "1";
		case  2:  return "2";
		case  3:  return "3";
		case  4:  return "4";
		case  5:  return "5";
		case  6:  return "6";
		case  7:  return "7";
		case  8:  return "8";
		case  9:  return "9";
		case 10: return "10";
		case 11: return "11";
		case 12: return "12";
		case 13: return "13";
		case A0: return "A0";
		case A1: return "A1";
		case A2: return "A2";
		case A3: return "A3";
		case A4: return "A4";
		case A5: return "A5";
		}
	}

	[[nodiscard]]
	constexpr PinKind kind() const {
		switch(number()) {
		default:  UNREACHABLE();
		case 0:  return PinKind::Digital;
		case 1:  return PinKind::Digital;
		case 2:  return PinKind::Digital;
		case 3:  return PinKind::DigitalPWM;
		case 4:  return PinKind::Digital;
		case 5:  return PinKind::DigitalPWM;
		case 6:  return PinKind::DigitalPWM;
		case 7:  return PinKind::Digital;
		case 8:  return PinKind::Digital;
		case 9:  return PinKind::Digital;
		case 10: return PinKind::DigitalPWM;
		case 11: return PinKind::DigitalPWM;
		case 12: return PinKind::Digital;
		case 13: return PinKind::Digital;
		case A0: return PinKind::Analog;
		case A1: return PinKind::Analog;
		case A2: return PinKind::Analog;
		case A3: return PinKind::Analog;
		case A4: return PinKind::Analog;
		case A5: return PinKind::Analog;
		}
	}

	[[nodiscard]]
	constexpr std::ptrdiff_t index() const {
		switch(number()) {
		default:
			UNREACHABLE();
			return -1;
		case 0:  return 0;
		case 1:  return 1;
		case 2:  return 2;
		case 3:  return 3;
		case 4:  return 4;
		case 5:  return 5;
		case 6:  return 6;
		case 7:  return 7;
		case 8:  return 8;
		case 9:  return 9;
		case 10: return 10;
		case 11: return 11;
		case 12: return 12;
		case 13: return 13;
		case A0: return 14;
		case A1: return 15;
		case A2: return 16;
		case A3: return 17;
		case A4: return 18;
		case A5: return 19;
		case A6: return 19;
		case A7: return 19;
		}
	}

	[[nodiscard]]
	constexpr bool is_valid() const {
		return index() >= 0 and index() <= A7;
	}

	void set_mode(PinMode mode) const {
		switch(mode) {
		default:
			/* shouldn't be reachable */
			UNREACHABLE();
		case PinMode::InputPullup:
			is_pullup_[index()] = true;
		case PinMode::Input:
			pinmodes_[index()] = true;
			break;
		case PinMode::Output:
			is_pullup_[index()] = false;
			pinmodes_[index()] = false;
			break;
		}
		pinMode(number(), static_cast<int>(mode));
	}

	[[nodiscard]]
	PinMode mode() const {
		if(pinmodes_[index()]) {
			if(is_pullup_[index()]) {
				return PinMode::InputPullup;
			}
			return PinMode::Input;
		}
		return PinMode::Output;
	}


	[[nodiscard]]
	LogicLevel digital_read() const {
		int level = digitalRead(number());
		if(level == HIGH) {
			return LogicLevel::High;
		}
		return LogicLevel::Low;
	}

	[[nodiscard]]
	PinStatus digital_write(LogicLevel level) const {
		if(mode() != PinMode::Output) {
			return PinStatus::BadPinMode;
		}
		digitalWrite(number(), static_cast<int>(level));
		return PinStatus::Good;
	}

	[[nodiscard]]
	std::pair<int, PinStatus> analog_read() const {
		if(kind() != PinKind::Analog) {
			return {-1, PinStatus::BadPinKind};
		}
		if(mode() == PinMode::Output) {
			return {-1, PinStatus::BadPinMode};
		}
		int val = analogRead(number());
		return {val, PinStatus::Good};
	}

	[[nodiscard]]
	PinStatus analog_write(int value) const {
		if(kind() != PinKind::DigitalPWM) {
			return PinStatus::BadPinKind;
		}
		if(mode() != PinMode::Output) {
			return PinStatus::BadPinMode;
		}
		if(analog_write_minm > value or analog_write_maxm < value) {
			return PinStatus::BadAnalogWriteValue;
		}
		analogWrite(number(), value);
		return PinStatus::Good;
	}

private:

	constexpr CheckedPin(int p):
		number_(p)
	{
		
	}

	static std::bitset<20> pinmodes_;
	static std::bitset<20> is_pullup_;
	int8_t number_;
};
	
inline constexpr auto all_pins = ino::Array{
	CheckedPin::from_pin_number<0>(),
	CheckedPin::from_pin_number<1>(),
	CheckedPin::from_pin_number<2>(),
	CheckedPin::from_pin_number<3>(),
	CheckedPin::from_pin_number<4>(),
	CheckedPin::from_pin_number<5>(),
	CheckedPin::from_pin_number<6>(),
	CheckedPin::from_pin_number<7>(),
	CheckedPin::from_pin_number<8>(),
	CheckedPin::from_pin_number<9>(),
	CheckedPin::from_pin_number<10>(),
	CheckedPin::from_pin_number<11>(),
	CheckedPin::from_pin_number<12>(),
	CheckedPin::from_pin_number<13>(),
	CheckedPin::from_pin_number<A0>(),
	CheckedPin::from_pin_number<A1>(),
	CheckedPin::from_pin_number<A2>(),
	CheckedPin::from_pin_number<A3>(),
	CheckedPin::from_pin_number<A4>(),
	CheckedPin::from_pin_number<A5>()
};

template <int PinNumber>
inline constexpr CheckedPin pin = CheckedPin::from_pin_number<PinNumber>();

} /* namespace ino */


#endif /* INO_PINS_H */
