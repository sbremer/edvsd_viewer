#include "kohonentracking.h"

template <int N>
KohonenTracking<N>::KohonenTracking()
	:m_size(N), m_start(PointF()), m_end(PointF()), m_spawn(-1), m_trackpolarity(false)
{

}

template <int N>
void KohonenTracking<N>::initialize(PointF p_start, PointF p_end, bool p_trackpolarity)
{
	m_start = p_start;
	m_end = p_end;
	m_trackpolarity = p_trackpolarity;
}

template <int N>
void KohonenTracking<N>::analyzeEvent(PointF p_event, bool p_polarity, unsigned int p_ts)
{
	if(p_polarity != m_trackpolarity)
		return;

	const double End_Dist = 6.0;
	const double Start_Dist = 6.0;

	bool atstart = false;

	typename list<Particle<N> >::iterator pointmin;
	double distmin = 1000;
	int particlemin;

	for(typename list<Particle<N> >::iterator i = m_tracker.begin(); i != m_tracker.end(); i++){
		for(int a = 0; a < N; a++){
			double dist = PointF::getDistance(p_event, i->points[a]);
			if(dist<distmin){
				distmin = dist;
				pointmin = i;
				particlemin = a;
			}
			if(getDistance(i->point[a], m_start) < 2.0){
				atstart = true;
			}
		}
	}

	PointF *p = &(pointmin->points.a[particlemin]);
	PointF *p2 = pointmin->points+(particlemin+1)%2;

	PointF delta = p_event-*p;
	double fact = 3.0/pow(PointF().getDistance(delta),3.0);
	fact = min(0.3,fact);

	*p += delta*fact;
	// *p2 += delta*fact*0.5 + (*p1 - *p2)*0.01;
	//*p2 += delta*fact*0.6 + (*p1 - *p2)*0.005;

	if((*p).getDistance(m_start) > Start_Dist && pointmin->ts == -1){
		pointmin->ts = p_ts;
	}

	if(!atstart && m_spawn == -1){
		m_spawn = p_ts + 2000;
		//tracker.append(Particle2(m_motions.at(0).start));
	}
	if(m_spawn != -1 && p_ts >= m_spawn){
		m_spawn = -1;
		m_tracker.push_back(Particle<N>(m_start));
	}
}
