/***************************************************************
  ZzzInput.h
***************************************************************/
#ifndef ZZZ_INPUT_H
#define ZZZ_INPUT_H

//M5Stack PBHUB default address
#define ZZZ_M5STACKPBHUB_ADDRESS 0x61
//M5Stack PBHUB ports 1-6
#define ZZZ_M5STACKPBHUB0        0x40
#define ZZZ_M5STACKPBHUB1        0x50
#define ZZZ_M5STACKPBHUB2        0x60
#define ZZZ_M5STACKPBHUB3        0x70
#define ZZZ_M5STACKPBHUB4        0x80
#define ZZZ_M5STACKPBHUB5        0xA0

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
template <int PIN, typename DATA> class ZzzInputDriverAnalogPin : public ZzzInputDriver, public DATA {
	public:
		ZzzInputDriverAnalogPin() {
		}

		virtual bool read() override {
			this->add(analogRead(PIN));
			return true;
		}
};


/**
 * Driver to read analog sensor via M5Stack PbHub
 * @param ADDRESS I2C address to access PbHub
 * @param ANALOG_PORT Port on PbHub where sensor is connected
 * @param WIRE template parameter allow to define custom Wire library
 * @param DATA template param to specify class to use for data processing. It must be of type ZzzData<int>
 */
template <typename WIRE, typename DATA, uint8_t ANALOG_PORT=ZZZ_M5STACKPBHUB0, uint8_t ADDRESS=ZZZ_M5STACKPBHUB_ADDRESS> class ZzzInputDriverPbHubAnalog : public ZzzInputDriver, public DATA {
	protected:
		WIRE *_pWire;
		const int COMMAND_A_ANALOG=0x06; //hub_a_read_value: analog read command on A wire

	public:
		ZzzInputDriverPbHubAnalog(WIRE& pParams) {
			_pWire=&pParams;
			_pWire->begin();
		}

		virtual bool read() override {
			_pWire->beginTransmission(ADDRESS);
			_pWire->write(ANALOG_PORT | COMMAND_A_ANALOG);
			if (_pWire->endTransmission() != 0) { //communication error
				return false;
			}
			_pWire->requestFrom(ADDRESS, (uint8_t)2);
			uint8_t valueL=_pWire->read();
			uint8_t valueH=_pWire->read();
			uint16_t value=(valueH<<8) | valueL;
			this->add(value);
			return true;
		}
};

/**
 * Driver to read SHT3X sensor via I2C
 * @param ADDRESS I2C address to access SHT3X
 * @param WIRE template parameter allow to define custom Wire library
 * @param DATA_TEMPERATURE template param to specify class to use for temperature data processing. It must be of type ZzzData<int>
 * @param DATA_HUMIDITY template param to specify class to use for humidity data processing. It must be of type ZzzData<int>
 */
template <typename WIRE, typename DATA_TEMPERATURE, typename DATA_HUMIDITY, uint8_t ADDRESS=0x44> class ZzzInputDriverI2CSHT3X : public ZzzInputDriver {
	protected:
		WIRE *_pWire;

	public:
		DATA_TEMPERATURE temperature;
		DATA_HUMIDITY humidity;

		ZzzInputDriverI2CSHT3X(WIRE& pParams) {
			_pWire=&pParams;
			_pWire->begin();
		}

		virtual bool read() override {
			_pWire->beginTransmission(ADDRESS);
			//Send measurement command
			_pWire->write(0x2C);
			_pWire->write(0x06);
			if (_pWire->endTransmission() != 0) { //communication error
				return false;
			}

			// Data is 6 bytes: Temperature MSB,LSB,CRC, humidity MSB,LSB,CRC
			uint8_t data[6];
			_pWire->requestFrom(ADDRESS, (uint8_t)6);
			for (int i=0;i<6;i++) {
				data[i]=_pWire->read();
			}
			uint16_t valueT=(data[0]<<8) | data[1];
			uint16_t valueH=(data[3]<<8) | data[4];

			//Temperature range is -45 - 130°C
			long longTemperature=((valueT*175L)/65535)-45; //TODO *100 to keep 2 decimal digits ((valueT*17500)/65535)-4500);
			temperature.add(longTemperature);
			//Humidity range is 0 - 100 %
			long longHumidity=(valueH*100L)/65535; //TODO *100 to keep 2 decimal digits (valueT*10000)/65535);
			humidity.add(longHumidity);
			return true;
		}
};


/**
 * Driver for multiple drivers
 * NB_DRIVER is the number of drivers to allocate. It should match constructor call otherwise result is unpredictable. (Max 8 drivers)
 */
template <size_t NB_DRIVER> class ZzzInputDriverMulti : public ZzzInputDriver {
	protected:
		ZzzInputDriver *_pDriver[NB_DRIVER];

	public:
		/** At least 2 drivers (use directly the driver if only one driver) */
		ZzzInputDriverMulti(ZzzInputDriver &driver1, ZzzInputDriver &driver2) {
			_pDriver[0]=&driver1;
			_pDriver[1]=&driver2;
		}

		ZzzInputDriverMulti(ZzzInputDriver &driver1, ZzzInputDriver &driver2, ZzzInputDriver &driver3) {
			_pDriver[0]=&driver1;
			_pDriver[1]=&driver2;
			_pDriver[2]=&driver3;
		}

		ZzzInputDriverMulti(ZzzInputDriver &driver1, ZzzInputDriver &driver2, ZzzInputDriver &driver3, ZzzInputDriver &driver4) {
			_pDriver[0]=&driver1;
			_pDriver[1]=&driver2;
			_pDriver[2]=&driver3;
			_pDriver[3]=&driver4;
		}

		ZzzInputDriverMulti(ZzzInputDriver &driver1, ZzzInputDriver &driver2, ZzzInputDriver &driver3, ZzzInputDriver &driver4, ZzzInputDriver &driver5) {
			_pDriver[0]=&driver1;
			_pDriver[1]=&driver2;
			_pDriver[2]=&driver3;
			_pDriver[3]=&driver4;
			_pDriver[4]=&driver5;
		}

		ZzzInputDriverMulti(ZzzInputDriver &driver1, ZzzInputDriver &driver2, ZzzInputDriver &driver3, ZzzInputDriver &driver4, ZzzInputDriver &driver5, ZzzInputDriver &driver6) {
			_pDriver[0]=&driver1;
			_pDriver[1]=&driver2;
			_pDriver[2]=&driver3;
			_pDriver[3]=&driver4;
			_pDriver[4]=&driver5;
			_pDriver[5]=&driver6;
		}

		ZzzInputDriverMulti(ZzzInputDriver &driver1, ZzzInputDriver &driver2, ZzzInputDriver &driver3, ZzzInputDriver &driver4, ZzzInputDriver &driver5, ZzzInputDriver &driver6, ZzzInputDriver &driver7) {
			_pDriver[0]=&driver1;
			_pDriver[1]=&driver2;
			_pDriver[2]=&driver3;
			_pDriver[3]=&driver4;
			_pDriver[4]=&driver5;
			_pDriver[5]=&driver6;
			_pDriver[6]=&driver7;
		}

		ZzzInputDriverMulti(ZzzInputDriver &driver1, ZzzInputDriver &driver2, ZzzInputDriver &driver3, ZzzInputDriver &driver4, ZzzInputDriver &driver5, ZzzInputDriver &driver6, ZzzInputDriver &driver7, ZzzInputDriver &driver8) {
			_pDriver[0]=&driver1;
			_pDriver[1]=&driver2;
			_pDriver[2]=&driver3;
			_pDriver[3]=&driver4;
			_pDriver[4]=&driver5;
			_pDriver[5]=&driver6;
			_pDriver[6]=&driver7;
			_pDriver[7]=&driver8;
		}

		size_t size() {
			return NB_DRIVER;
		}

		virtual bool read() override {
			bool result=false;
			for (int i=0;i<NB_DRIVER;i++) {
				bool curResult=_pDriver[i]->read();
				result=result || curResult;
			}
			return result;
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


		/** Set the callback to call on each change. */
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
		 * @param driver Underlying instance to access input(s).
		 * @param intervalMs minimum time in milliseconds to wait before next driver requests.
		 */
		ZzzInput(ZzzInputDriver &driver, unsigned long intervalMs=ZzzInput::DEFAULT_INTERVAL_MS) {
			_pDriver=&driver;
			_intervalMs=intervalMs;
		}
};

#endif

