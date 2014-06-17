#include "startendtracker.h"

StartEndTracker::StartEndTracker()
	:m_attraction_fact_start(2.5), m_attraction_pow_start(3.2), m_dist(1.0), m_attraction_fact_end(2.0), m_attraction_pow_end(3.3)
{

}

StartEndTracker::StartEndTracker(double p_attraction_fact_start, double p_attraction_pow_start, double p_dist, double p_attraction_fact_end, double p_attraction_pow_end)
	:m_attraction_fact_start(p_attraction_fact_start), m_attraction_pow_start(p_attraction_pow_start), m_dist(p_dist), m_attraction_fact_end(p_attraction_fact_end), m_attraction_pow_end(p_attraction_pow_end)
{

}

/*list<MotionF> StartEndTracker::trackPoints(EDVS_Event *p_buffer, int p_n)
{
	list<MotionF> motions;
	motions = analyzeMotionStartpoints(p_buffer, p_n);
	motions = analyzeMotionEndpoints(p_buffer, p_n, motions);
	return motions;
}*/

list<MotionF> StartEndTracker::trackPoints(EventF *p_buffer, int p_n)
{
	list<MotionF> motions;
	motions = analyzeMotionStartpoints(p_buffer, p_n);
	motions = analyzeMotionEndpoints(p_buffer, p_n, motions);
	return motions;
}

/*list<MotionF> StartEndTracker::analyzeMotionStartpoints(EDVS_Event *p_buffer, int p_n)
{
	PointF point_cloud_revers[13*13];

	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			point_cloud_revers[y*13+x] = PointF(3.0+x*10, 3.0+y*10);
		}
	}

	//Track Motion (revers to find motion startpoint)
	for(int a=p_n-1;a>=0;a--){
		if(!p_buffer[a].p)continue;
		for(int x=0;x<=12;x++){
			for(int y=0;y<=12;y++){
				EDVS_Event event = p_buffer[a];
				PointF tmp;
				PointF point = point_cloud_revers[y*13+x];
				double fact = m_attraction_fact_start/pow(sqrt((event.x-point.x)*(event.x-point.x)+(event.y-point.y)*(event.y-point.y)), m_attraction_pow_start);
				fact = qMin(1.0, fact);
				tmp.x = (point.x*(1.0-fact)+event.x*fact);
				tmp.y =(point.y*(1.0-fact)+event.y*fact);
				point_cloud_revers[y*13+x] = tmp;
			}
		}
	}

	//Filter moved points
	list<PointF> points;
	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			if(PointF::getDistance(PointF(3.0+x*10, 3.0+y*10), point_cloud_revers[y*13+x])>2){
				PointF tmp;
				tmp = point_cloud_revers[y*13+x];
				points.push_back(tmp);
			}
		}
	}

	list<MotionF> startpoints;

	//Find startpoints (
	MotionF tmp;
	tmp.start = points.front();
	points.pop_front();
	tmp.num = 1;
	startpoints.push_back(tmp);

	while(points.size() != 0){
		MotionF tmp;
		tmp.start = points.front();
		points.pop_front();
		bool found = false;
		for(list<MotionF>::iterator i = startpoints.begin(); i!= startpoints.end(); i++){
			if(PointF::getDistance(tmp.start, i->start)<m_dist){
				found = true;
				i->num++;
				break;
			}
		}
		if(!found){
			tmp.num = 1;
			startpoints.push_back(tmp);
		}
	}

	//Filter single start points
	for(list<MotionF>::iterator i = startpoints.begin(); i!= startpoints.end();){
		if(i->num<2){
			i = startpoints.erase(i);
		}
		else{
			i++;
		}
	}
	return startpoints;
}*/

list<MotionF> StartEndTracker::analyzeMotionStartpoints(EventF *p_buffer, int p_n)
{
	PointF point_cloud_revers[13*13];

	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			point_cloud_revers[y*13+x] = PointF(3.0+x*10, 3.0+y*10);
		}
	}

	//Track Motion (revers to find motion startpoint)
	for(int a=p_n-1;a>=0;a--){
		if(!p_buffer[a].polarity)continue;
		for(int x=0;x<=12;x++){
			for(int y=0;y<=12;y++){
				PointF event = p_buffer[a].position;
				PointF tmp;
				PointF point = point_cloud_revers[y*13+x];
				double fact = m_attraction_fact_start/pow(sqrt((event.x-point.x)*(event.x-point.x)+(event.y-point.y)*(event.y-point.y)), m_attraction_pow_start);
				fact = min(1.0, fact);
				tmp.x = (point.x*(1.0-fact)+event.x*fact);
				tmp.y =(point.y*(1.0-fact)+event.y*fact);
				point_cloud_revers[y*13+x] = tmp;
			}
		}
	}

	//Filter moved points
	list<PointF> points;
	for(int x=0;x<=12;x++){
		for(int y=0;y<=12;y++){
			if(PointF::getDistance(PointF(3.0+x*10, 3.0+y*10), point_cloud_revers[y*13+x])>2){
				PointF tmp;
				tmp = point_cloud_revers[y*13+x];
				points.push_back(tmp);
			}
		}
	}

	list<MotionF> startpoints;

	//Find startpoints (
	MotionF tmp;
	tmp.start = points.front();
	points.pop_front();
	tmp.num = 1;
	startpoints.push_back(tmp);

	while(points.size() != 0){
		MotionF tmp;
		tmp.start = points.front();
		points.pop_front();
		bool found = false;
		for(list<MotionF>::iterator i = startpoints.begin(); i!= startpoints.end(); i++){
			if(PointF::getDistance(tmp.start, i->start)<m_dist){
				found = true;
				i->num++;
				break;
			}
		}
		if(!found){
			tmp.num = 1;
			startpoints.push_back(tmp);
		}
	}

	//Filter single start points
	for(list<MotionF>::iterator i = startpoints.begin(); i!= startpoints.end();){
		if(i->num<30){
			i = startpoints.erase(i);
		}
		else{
			i++;
		}
	}
	return startpoints;
}

/*list<MotionF> StartEndTracker::analyzeMotionEndpoints(EDVS_Event *p_buffer, int p_n, list<MotionF> p_motions)
{
	//Track motion (forward, find endpoints)
	for(list<MotionF>::iterator i = p_motions.begin(); i!= p_motions.end(); i++){
		PointF tracker = i->start;

		for(int a=0;a<p_n;a++){
			if(!p_buffer[a].p)continue;
			EDVS_Event event = p_buffer[a];
			PointF tmp;
			PointF point = tracker;
			double fact = m_attraction_fact_end/pow(sqrt((event.x-point.x)*(event.x-point.x)+(event.y-point.y)*(event.y-point.y)), m_attraction_pow_end);
			fact = min(1.0, fact);
			tmp.x = (point.x*(1.0-fact)+event.x*fact);
			tmp.y = (point.y*(1.0-fact)+event.y*fact);
			tracker = tmp;
		}

		i->end = tracker;
	}
	return p_motions;
}*/

list<MotionF> StartEndTracker::analyzeMotionEndpoints(EventF *p_buffer, int p_n, list<MotionF> p_motions)
{
	//Track motion (forward, find endpoints)
	for(list<MotionF>::iterator i = p_motions.begin(); i!= p_motions.end(); i++){
		PointF tracker = i->start;

		for(int a=0;a<p_n;a++){
			if(!p_buffer[a].polarity)continue;
			PointF event = p_buffer[a].position;
			PointF tmp;
			PointF point = tracker;
			double fact = m_attraction_fact_end/pow(sqrt((event.x-point.x)*(event.x-point.x)+(event.y-point.y)*(event.y-point.y)), m_attraction_pow_end);
			fact = min(1.0, fact);
			tmp.x = (point.x*(1.0-fact)+event.x*fact);
			tmp.y = (point.y*(1.0-fact)+event.y*fact);
			tracker = tmp;
		}

		i->end = tracker;
	}
	return p_motions;
}
