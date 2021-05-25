/***************************************************************
  ZzzInput.h
***************************************************************/
#ifndef ZZZ_INPUT_H
#define ZZZ_INPUT_H

#ifndef ZZZ_DATA_H
	/* To replace ZzzData.h if not available */
	template <typename T=int> class ZzzData {
		public:
			void reset();
			T add(T input);
			T get();

	};
#endif

/** Callback to receive input update notifications */
typedef void(*ZzzInputCallback)();


/** Abstract input driver. Class to override to implement a new input driver */
class ZzzInputDriver {
	protected:
		ZzzInputDriver() {
		}
	public:
		/** Read input data return true if a new value has been read */
		virtual bool read();
};


/**
 * Simple driver template class to read input from analog pin
 * @param PIN template param to specify which pin to read from.
 * @param DATA template param to specify class to use for data processing. It must be of type ZzzData<int>
 */
template <int PIN, typename DATA> class ZzzInputDriverAnalogPin : public ZzzInputDriver {
	public:
		DATA data;

		ZzzInputDriverAnalogPin() {
		}

		virtual bool read() override {
			data.add(analogRead(PIN));
			return true;
		}
};


/**
 * Class to manage an input or a set of inputs. The template need a Driver parameter to check the inputs states.
 * The driver class must implement get(), size() and getIntervalUs().
 */
class ZzzInput {
	protected:
		/** Interval between two driver states requests */
		unsigned long _intervalMs;
		unsigned long _lastRequestMs=0;

		ZzzInputDriver *_pDriver;

		/** Callback called on status change */
		ZzzInputCallback _callback=nullptr;

	public:
		/** Default interval in milliseconds */
		static const unsigned long DEFAULT_INTERVAL_MS=1000;


		/** Set the callback to call on each button state change. */
		void setCallback(ZzzInputCallback callback) {
			_callback=callback;
		}
	
		/** To call frequently (ie: in Arduino loop) */
		void update() {
			//check elapsed time (overflow proof)
			if (millis() - _lastRequestMs > _intervalMs) {
				bool readIsOk=_pDriver->read();
				_lastRequestMs=millis();

				if (readIsOk && _callback!=nullptr) {
					_callback();
				}
			}
		}

		/** Constructor 
		 * @param driver Underlying instance to access button(s).
		 * @param intervalMs minimum time in milliseconds to wait before next driver requests.
		 */
		ZzzInput(ZzzInputDriver &driver, unsigned long intervalMs=ZzzInput::DEFAULT_INTERVAL_MS) {
			_pDriver=&driver;
			_intervalMs=intervalMs;
		}
};

#endif

