#ifndef INO_STEPPER_H
#define INO_STEPPER_H


namespace ino {

template <uint8_t N, int E1, int E2, int M1, int M2>
struct Stepper {

	Stepper():
		state_(0),
		position_(0)
	{
	
	}

	void begin() {
		pinMode(E1, OUTPUT);
		pinMode(E2, OUTPUT);
		pinMode(M1, OUTPUT);
		pinMode(M2, OUTPUT);
		digitalWrite(E1, LOW);
		digitalWrite(E2, LOW);
		digitalWrite(M1, HIGH);
		digitalWrite(M2, HIGH);
	}

	void forward_step() {
		++position_;
		position_ %= N;
		++state_;
		publish();
	}

	void backward_step() {
		if(position() == 0u) {
			position_ = 100u;
		}
		--position_;
		--state_;
		publish();
	}

	uint8_t position() const {
		return position_;
	}

	void set_position(uint8_t pos) {
		pos %= 100;
		if(pos > position()) {
			auto count = pos - position();
			if(count < 50) {
				while(position() != pos) {
					forward_step();
					delay(4);
				}
			} else {
				while(position() != pos) {
					backward_step();
					delay(4);
				}
			}
		} else {
			auto count = position() - pos;
			if(count < 50) {
				while(position() != pos) {
					backward_step();
					delay(4);
				}
			} else {
				while(position() != pos) {
					forward_step();
					delay(4);
				}
			}
		}
	}

private:

	void publish() const {
		switch(state()) {
		case 0u:
			digitalWrite(E1, LOW);
			digitalWrite(E2, LOW);
			break;
		case 1u:
			digitalWrite(E1, HIGH);
			digitalWrite(E2, LOW);
			break;
		case 2u:
			digitalWrite(E1, HIGH);
			digitalWrite(E2, HIGH);
			break;
		case 3u:
			digitalWrite(E1, LOW);
			digitalWrite(E2, HIGH);
			break;
		}
	}

	uint8_t state() const {
		return state_ % 4u;
	}

	uint8_t state_ = 0u;
	uint8_t position_ = 0u;
};

} /* namespace ino */


#endif /* INO_STEPPER_H */
