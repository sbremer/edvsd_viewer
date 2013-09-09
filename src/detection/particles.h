#ifndef PARTICLES_H
#define PARTICLES_H

#ifdef QT
#include <QPointF>
#endif

#include <vector>
#include <math.h>

using namespace std;

struct PointF{
	double x,y;

	PointF()
	{
		x = y = 0.0;
	}

	PointF(double p_x, double p_y)
	{
		x = p_x;
		y = p_y;
	}

#ifdef QT
	PointF(QPointF p_point)
	{
		x = p_point.x();
		y = p_point.y();
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

template <int N>
struct Particle{
	vector<PointF> points;
	int ts;

	Particle(PointF p_point = PointF(), int p_ts = -1)
	{
		points.resize(N);
		for(int a=0;a<N;a++){
			points[a] = p_point;
		}
		ts = p_ts;
	}
};

#endif // PARTICLES_H
