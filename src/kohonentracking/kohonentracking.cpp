#include "kohonentracking.h"

template <int N>
KohonenTracking<N>::KohonenTracking()
	:m_size(N), m_function(Function_Exponential), m_neighbors(1), m_start(PointF()), m_end(PointF()), m_spawn(-1), m_trackpolarity(false), m_duration_min(1000000),
	  m_start_dist(4.0), m_end_dist(4.0), m_attraction_fact(3.0), m_attraction_pow(3.0), m_attraction_max(0.3), m_neighbor_attraction(0.01)
{

}

template <int N>
KohonenTracking<N>::KohonenTracking(double p_start_dist, double p_end_dist, double p_attraction_fact, double p_attraction_pow, double p_attraction_max, double p_neighbor_attraction)
	:m_size(N), m_function(Function_Exponential), m_neighbors(1), m_start(PointF()), m_end(PointF()), m_spawn(-1), m_trackpolarity(false), m_duration_min(1000000),
	  m_start_dist(p_start_dist), m_end_dist(p_end_dist), m_attraction_fact(p_attraction_fact), m_attraction_pow(p_attraction_pow), m_attraction_max(p_attraction_max), m_neighbor_attraction(p_neighbor_attraction)
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
			if(PointF::getDistance(i->points[a], m_start) < m_start_dist){
				atstart = true;
			}
		}
	}

//	if(distmin = 1000)
//		return 0;

	PointF *p = &(pointmin->points[particlemin]);

	PointF delta = p_event-*p;
	double fact = m_attraction_fact / pow(PointF().getDistance(delta), m_attraction_pow);
	fact = min(m_attraction_max, fact);

	*p += delta*fact;

	for(int a = 1; a <= m_neighbors; a++){
		if(particlemin + a < N){
			PointF *pn = &(pointmin->points[particlemin + a]);
			*pn += (delta * fact + (p_event - *pn) * m_neighbor_attraction) * m_function(a);
		}
		if(particlemin - a >= 0){
			PointF *pn = &(pointmin->points[particlemin - a]);
			*pn += (delta * fact + (p_event - *pn) * m_neighbor_attraction) * m_function(a);
		}
	}

	if((*p).getDistance(m_start) > m_start_dist && pointmin->ts == -1){
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

	if((*p).getDistance(m_end) < m_end_dist || (pointmin->ts != -1 && p_ts - pointmin->ts > 1.5 * m_duration_min)){
		if(p_ts - pointmin->ts < m_duration_min){
			m_duration_min = p_ts - pointmin->ts;
		}
		m_tracker.erase(pointmin);
		return 0;
	}

	return &(*pointmin);
}

template <int N>
const KohonenMap<N>	* KohonenTracking<N>::analyzeEvent(EventF p_event)
{
	if(p_event.polarity != m_trackpolarity)
		return 0;

	bool atstart = false;

	typename list<KohonenMap<N> >::iterator pointmin;
	double distmin = 1000;
	int particlemin;

	for(typename list<KohonenMap<N> >::iterator i = m_tracker.begin(); i != m_tracker.end(); i++){
		for(int a = 0; a < N; a++){
			double dist = PointF::getDistance(p_event.position, i->points[a]);
			if(dist<distmin){
				distmin = dist;
				pointmin = i;
				particlemin = a;
			}
			if(PointF::getDistance(i->points[a], m_start) < m_start_dist){
				atstart = true;
			}
		}
	}

	if(distmin == 1000)
		return 0;

	PointF *p = &(pointmin->points[particlemin]);

	PointF delta = p_event.position-*p;
	double fact = m_attraction_fact / pow(PointF().getDistance(delta), m_attraction_pow);
	fact = min(m_attraction_max, fact);

	*p += delta*fact;

	for(int a = 1; a <= m_neighbors; a++){
		if(particlemin + a < N){
			PointF *pn = &(pointmin->points[particlemin + a]);
			*pn += (delta * fact + (p_event.position - *pn) * m_neighbor_attraction) * m_function(a);
		}
		if(particlemin - a >= 0){
			PointF *pn = &(pointmin->points[particlemin - a]);
			*pn += (delta * fact + (p_event.position - *pn) * m_neighbor_attraction) * m_function(a);
		}
	}

	if((*p).getDistance(m_start) > m_start_dist && pointmin->ts == -1){
		pointmin->ts = p_event.ts;
	}

	if(!atstart && m_spawn == -1){
		m_spawn = p_event.ts + 2000;
		//tracker.append(Particle2(m_motions.at(0).start));
	}

	if(m_spawn != -1 && p_event.ts >= m_spawn){
		m_spawn = -1;
		m_tracker.push_back(KohonenMap<N>(m_start));
	}

	if((*p).getDistance(m_end) < m_end_dist || (pointmin->ts != -1 && p_event.ts - pointmin->ts > 1.5 * m_duration_min)){
		if(p_event.ts - pointmin->ts < m_duration_min){
			m_duration_min = p_event.ts - pointmin->ts;
		}
		m_tracker.erase(pointmin);
		return 0;
	}

	return &(*pointmin);
}

template class KohonenTracking<2>;
