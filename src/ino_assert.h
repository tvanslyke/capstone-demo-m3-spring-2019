#ifndef INO_ASSERT_H
#define INO_ASSERT_H

#include <Arduino.h>

namespace ino {

[[noreturn]]
inline void ino_assert_fail(const char* assertion, const char* file, long linenumber) {
	Serial.print(file);
	Serial.print(" line ");
	Serial.println(linenumber);
	Serial.print("Assertion: \"");
	Serial.print(assertion);
	Serial.println("\" failed. Going into infinite loop.");
	volatile unsigned n = 0;
	for(;;) {
		++n;
	}
}

} /* namespace ino */

#define ASSERT(x) \
	(x) ? (void)0 : ino_assert_fail(#x, __FILE__, __LINE__)
	
#define UNREACHABLE() \
	ASSERT(!"Unreachable code path.")

#endif /* INO_ASSERT_H */
