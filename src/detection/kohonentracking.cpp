#include "kohonentracking.h"

template <int N>
KohonenTracking<N>::KohonenTracking()
	:m_size(N), m_function(Function_Exponential), m_neighbors(1), m_start(PointF()), m_end(PointF()), m_spawn(-1), m_trackpolarity(false), m_duration_min(1000000)
{

}

template <int N>
void KohonenTracking<N>::initialize(PointF p_start, PointF p_end, bool p_trackpolarity)
{
	m_start = p_start;
	m_end = p_end;
	m_trackpolarity = p_trackpolarity;

	m_tracker.clear();

	m_tracker.push_back(m_start);
	m_iterator = m_tracker.begin();
	m_iterator_at = 0;
}

template <int N>
int KohonenTracking<N>::getDurationMin()
{
	return m_duration_min;
}

template <int N>
int KohonenTracking<N>::getListLength()
{
	return m_tracker.size();
}

template <int N>
const KohonenMap<N>* KohonenTracking<N>::getKohonenMap(int p_at)
{
	if(p_at < m_iterator_at){
		m_iterator = m_tracker.begin();
		m_iterator_at = 0;
	}

	if(m_iterator == m_tracker.end()){
		return 0;
	}

	while(m_iterator_at < p_at){
		m_iterator++;
		m_iterator_at++;
		if(m_iterator == m_tracker.end()){
			return 0;
		}
	}

	return &(*m_iterator);
}

template <int N>
const KohonenMap<N>	* KohonenTracking<N>::analyzeEvent(PointF p_event, bool p_polarity, unsigned int p_ts)
{
	if(p_polarity != m_trackpolarity)
		return 0;

	const double End_Dist = 4.0;
	const double Start_Dist = 4.0;

	bool atstart = false;

	typename list<KohonenMap<N> >::iterator pointmin;
	double distmin = 1000;
	int particlemin;

	for(typename list<KohonenMap<N> >::iterator i = m_tracker.begin(); i != m_tracker.end(); i++){
		for(int a = 0; a < N; a++){
			double dist = PointF::getDistance(p_event, i->points[a]);
			if(dist<distmin){
				distmin = dist;
				pointmin = i;
				particlemin = a;
			}
			if(PointF::getDistance(i->points[a], m_start) < 2.0){
				atstart = true;
			}
		}
	}

//	if(distmin = 1000)
//		return 0;

	PointF *p = &(pointmin->points[particlemin]);

	PointF delta = p_event-*p;
	double fact = 3.0 / pow(PointF().getDistance(delta), 3.0);
	fact = min(0.3, fact);

	*p += delta*fact;

	for(int a = 1; a <= m_neighbors; a++){
		if(particlemin + a < N){
			PointF *pn = &(pointmin->points[particlemin + a]);
			*pn += (delta * fact + (*p - *pn) * 0.01) * m_function(a);
		}
		if(particlemin - a >= 0){
			PointF *pn = &(pointmin->points[particlemin - a]);
			*pn += (delta * fact + (*p - *pn) * 0.01) * m_function(a);
		}
	}

	if((*p).getDistance(m_start) > Start_Dist && pointmin->ts == -1){
		pointmin->ts = p_ts;
	}

	if(!atstart && m_spawn == -1){
		m_spawn = p_ts + 2000;
		//tracker.append(Particle2(m_motions.at(0).start));
	}

	if(m_spawn != -1 && p_ts >= m_spawn){
		m_spawn = -1;
		m_tracker.push_back(KohonenMap<N>(m_start));
	}

	if((*p).getDistance(m_end) < End_Dist || (pointmin->ts != -1 && p_ts - pointmin->ts > 1.5 * m_duration_min)){
		if(p_ts - pointmin->ts < m_duration_min){
			m_duration_min = p_ts - pointmin->ts;
		}
		m_tracker.erase(pointmin);
	}

	return &(*pointmin);
}

template class KohonenTracking<2>;
