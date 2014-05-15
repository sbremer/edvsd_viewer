#ifndef FEATURE_EVENT_H
#define FEATURE_EVENT_H

#include <vector>
using namespace std;

enum FeatureEventType {FEATURE_EVENT_TYPE_INVALID, FEATURE_EVENT_TYPE_NEW_NODE, FEATURE_EVENT_TYPE_LEARN_NODE, FEATURE_EVENT_TYPE_KILL_NODE};

struct FeatureEvent{
	FeatureEvent(vector<double> p_features, int p_id, unsigned int p_time, FeatureEventType p_type)
	{
		features = p_features;
		id = p_id;
		time = p_time;
		type = p_type;
	}

	FeatureEvent()
	{
		type = FEATURE_EVENT_TYPE_INVALID;
	}

	vector<double> features;
	int id;
	unsigned int time;
	FeatureEventType type;
};

#endif // FEATURE_EVENT_H
