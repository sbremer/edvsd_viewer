#ifndef EDVSD_H
#define EDVSD_H

//#include <QtGlobal>
#include <stdint.h>

struct EDVS_Event{
	uint8_t x;		// (0, EDVS_Header.x_resolution - 1)
	uint8_t y;		// (0, EDVS_Header.y_resolution - 1)
	uint8_t p;		// {0,1}, 0/false = "on", 1../true = "off"
	uint8_t fill;	// no function, only to ensure 8 byte total size and alignment
	uint32_t t;		// Timestamp

	inline bool operator< (const EDVS_Event& other) const
	{
		return t < other.t;
	}
};

struct EDVS_Header{
	uint8_t version;
	uint8_t x_resolution;
	uint8_t y_resolution;
	uint8_t timestamp_resolution;
};

enum EDVS_Polarity{
	EDVS_Polarity_On = 0,
	EDVS_Polarity_Off = 1
};

enum EDVS_Timestamp_Resolution{
	EDVS_Timestamp_Resolution_None = 0,
	EDVS_Timestamp_Resolution_16bit = 1,
	EDVS_Timestamp_Resolution_24bit = 2,
	EDVS_Timestamp_Resolution_32bit = 3
};

#endif // EDVSD_H
