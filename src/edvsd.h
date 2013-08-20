#ifndef EDVSD_H
#define EDVSD_H

#include <QtGlobal>

struct EDVS_Event{
	quint8 x, y, p;
	quint8 fill;
	quint32 t;
};

enum EDVS_Timestamp_Resolution{
	EDVS_Timestamp_Resolution_None = 0,
	EDVS_Timestamp_Resolution_16bit = 1,
	EDVS_Timestamp_Resolution_24bit = 2,
	EDVS_Timestamp_Resolution_32bit = 3
};

#endif // EDVSD_H
