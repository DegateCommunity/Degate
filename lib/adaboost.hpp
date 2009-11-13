/*

  This code is from http://terpconnect.umd.edu/~xliu10/research/adaboost.html

  Todo: Check licence type

 */

#ifndef ADABOOST_HPP
#define ADABOOST_HPP

#include <vector>
#include <string>
#include <math.h>

// that is bad
using namespace std;

template <class T>
class Classifier
{
public:
	// This function performs the actual recognition
	// MUST be implemented by the weak classifier, usually T is the feature vector
	virtual int recognize(T&) = 0;
	// MUST be implemented by the weak classifier, simply return the name of the weak classifier itself
	// It is recommended to use this function to keep track of the weak classifiers.
	// You will find this useful if more than 30 weak classifiers are trained
	virtual string get_name() const = 0;
	// the ada-boost algorithm that trains the strong classifier from weak classifiers
	// data and label defines the training set
	// clsfrs is a collection of weak classifiers
	// this ada-boost implementation will first run the weak classifiers against all the training samples
	// and therefore the acutal trainning will be very fast 
	static vector<float> adaboost(vector<Classifier<T>*> clsfrs, vector<T*> data, vector<int> label, const int maxround = 80)
	{
		vector<float> alpha;
		vector<float> d;
		
		if (data.size()!=label.size() || clsfrs.size()==0 || label.size()==0)
			return alpha;
		
		d.resize(label.size());
		alpha.resize(clsfrs.size());


		for (unsigned int i=0;i<label.size();i++)
			d[i]=float(1.0)/float(label.size());
		vector< vector<int> > rec;
		rec.resize(clsfrs.size());

		// run the weak classifiers on all the trainning data first
		for (unsigned int j=0;j<clsfrs.size();j++)
		{
			rec[j].resize(label.size());
			for (unsigned int i=0;i<label.size();i++)
				rec[j][i]=clsfrs[j]->recognize(*data[i]);
		}

		//run maxround times of iteration
		
		for (int round=0;round<maxround;round++)
		{
			float minerr=(float)label.size();
			int best = 0;
			for (unsigned int j=0;j<clsfrs.size();j++) 
			{
				float err=0;
				for (unsigned int i=0;i<label.size();i++)
				{
					if (rec[j][i]!=label[i])
					err += d[i];
				}
				if (err<minerr)
				{
					minerr = err;
					best = j;
				}
			}
			if (minerr >= 0.5) break;

			float a= log((1.0f-minerr)/minerr)/2;
			alpha[best]+=a;
			vector<float> d1=d;
			float z = 0;
			for (unsigned int i=0;i<label.size();i++)
			{
				d1[i]=d[i]*exp(-a*label[i]*rec[best][i]);
				z+=d1[i];
			}
			for (unsigned int i=0;i<label.size();i++)
			{
				d[i]=d1[i]/z;
			}
		}
		return alpha;
	}
};

//The linear combination of weak classifiers i.e. the strong classifier

template <class T>
class MultiClassifier :public Classifier<T>
{
private:
	vector<float> weights;
	vector<Classifier<T>*> clsfrs;
public:
	float score;
	MultiClassifier(vector<float> w, vector<Classifier<T>*> c)
	{	
		this->weights = w;
		this->clsfrs = c;
	}
  std::string get_name() const { return "MultiClassifier"; }
    int recognize(T& obj)
	{
		float res=0;
		for (unsigned int i=0;i<weights.size();i++)
			res+=weights[i]*clsfrs[i]->recognize(obj);
		score=res;
		if (res>=0) 
			return 1;
		else
			return -1;
	}
};

// the utility function that tests a (strong) classifier over all the test data

template <class T>
void testClassifier(Classifier<T>* cls, vector<T*> data, vector<int> label, float & fpos, float & fneg)
{
	int pos = 0, neg = 0;
	fpos=fneg=0;
	for (int i=0;i<label.size();i++)
	{
		int rec = cls->recognize(*data[i]);
		if (label[i]==1)
		{
			pos++;
			if (rec!=1)
				fneg=fneg+1;
		}
		if (label[i]==-1)
		{
			neg++;
			if (rec!=-1)
				fpos=fpos+1;
		}
	}
	fpos = fpos/neg;
	fneg = fneg/pos;
}

#endif
