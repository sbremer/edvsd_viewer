#ifndef POINTF_H
#define POINTF_H

#ifdef QT
#include <QPointF>
#endif

#include <math.h>
#include <iostream>

struct PointF{
	double x, y;

	PointF()
	{
		x = y = 0.0;
	}

	PointF(double p_x, double p_y)
	{
		x = p_x;
		y = p_y;
	}

	PointF(int p_x, int p_y)
	{
		x = (double) p_x;
		y = (double) p_y;
	}

#ifdef QT
	PointF(const QPointF& p_point)
	{
		x = p_point.x();
		y = p_point.y();
	}

	QPointF toQPointF()
	{
		return QPointF(x, y);
	}

	static QPointF toQPointF(const PointF& p_point)
	{
		return QPointF(p_point.x, p_point.y);
	}
#endif

	PointF& operator += (PointF rhs)
	{
		this->x += rhs.x;
		this->y += rhs.y;
		return *this;
	}

	PointF operator + (PointF rhs)
	{
		PointF tmp = *this;
		tmp += rhs;
		return tmp;
	}

	PointF& operator -= (PointF rhs)
	{
		this->x -= rhs.x;
		this->y -= rhs.y;
		return *this;
	}

	PointF operator - (PointF rhs)
	{
		PointF tmp = *this;
		tmp -= rhs;
		return tmp;
	}

	PointF& operator *= (double rhs)
	{
		this->x *= rhs;
		this->y *= rhs;
		return *this;
	}

	PointF operator * (double rhs)
	{
		PointF tmp = *this;
		tmp *= rhs;
		return tmp;
	}

	PointF& operator /= (double rhs)
	{
		this->x /= rhs;
		this->y /= rhs;
		return *this;
	}

	PointF operator / (double rhs)
	{
		PointF tmp = *this;
		tmp /= rhs;
		return tmp;
	}

	double getDistance(PointF p_p1)
	{
		return sqrt((p_p1.x-this->x)*(p_p1.x-this->x)+(p_p1.y-this->y)*(p_p1.y-this->y));
	}

	static double getDistance(PointF p_p1, PointF p_p2)
	{
		return sqrt((p_p1.x-p_p2.x)*(p_p1.x-p_p2.x)+(p_p1.y-p_p2.y)*(p_p1.y-p_p2.y));
	}
};

inline std::ostream& operator << (std::ostream& os, const PointF& obj) {
	os << "PointF(" << obj.x << ", " << obj.y << ")";
	return os;
}

#endif // POINTF_H
