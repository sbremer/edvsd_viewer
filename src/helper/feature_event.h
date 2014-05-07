#ifndef FEATURE_EVENT_H
#define FEATURE_EVENT_H

#include <vector>
using namespace std;

enum FeatureEventType {FEATURE_EVENT_TYPE_NEW_NODE, FEATURE_EVENT_TYPE_LEARN_NODE, FEATURE_EVENT_TYPE_KILL_NODE};

struct FeatureEvent{
	FeatureEvent(vector<double> p_features, FeatureEventType p_type)
	{
		features = p_features;
		type = p_type;
	}

	vector<double> features;
	FeatureEventType type;
};

#endif // FEATURE_EVENT_H
