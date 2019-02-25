#include "Pins.h"

std::bitset<20> ino::CheckedPin::pinmodes_ = std::bitset<20>{};
std::bitset<20> ino::CheckedPin::is_pullup_ = std::bitset<20>{};

