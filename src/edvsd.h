#ifndef EDVSD_H
#define EDVSD_H

#include <QtGlobal>

struct EDVS_Event{
	quint8 x, y, p;
	quint8 fill;
	quint32 t;
};

#endif // EDVSD_H
