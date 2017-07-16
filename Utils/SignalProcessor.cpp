/**
 * Signal processor is a generic supporting class utilizing many standard
 * transformation of the data
 */
template<class T> class SignalProcessor {

	typedef vector<T> Signal;

	public:

	inline double sqr(double x) {
		return x*x;
	}

	// enumerates the value of normal probability distribution function with given parameters
	// in given point
	inline double normalPDF(double x, double sigma, double u = 0.0) {
		return exp(-sqr(x-u) / (2*sqr(sigma))) / (sqrt(2*M_PI) * sigma);
	}

	// cuts the part of the vector
	static Signal interval(const Signal &signal, int begin, int end) {
		Signal segment;
		do {
			segment.push_back(signal[begin]);
			begin = (begin+1)%signal.size();
		} while (begin != end);
		segment.push_back(signal[end]);
		return segment;
	}

	static bool isLocalMinimum(int pos, const vector<T> &signal, int radius) {
		int length = signal.size();
		for (int i = 1; i <= radius; i++) {
			if (signal[(pos+i+length)%length] <= signal[pos])
				return false;
			if (signal[(pos-i+length)%length] <= signal[pos])
				return false;
		}
		return true;
	}

	static bool isLocalMaximum(int pos, const vector<T> &signal, int radius) {
		int length = signal.size();
		for (int i = 1; i <= radius; i++) {
			if (signal[(pos+i+length)%length] >= signal[pos])
				return false;
			if (signal[(pos-i+length)%length] >= signal[pos])
				return false;
		}
		return true;
	}

	static vector<T> circularConvolution(vector<T> signal, vector<double> kernel) {
		int signalLength = signal.size();
		int kernelLength = kernel.size();
	
		vector<T> convolution(signalLength);
	
		for (int i = 0; i < signalLength; i++) {
			for (int j = 0; j < kernelLength; j++) {
				convolution[i] += signal[(i-j+signalLength)%signalLength] * kernel[j];
			}
		}
	
		return convolution;
	}

	static vector<T> difference(const vector<T> &signal) {
		int length = signal.size();
		vector<T> diff(length);
		adjacent_difference(signal.begin(),signal.end(),diff.begin());
		diff[0] -= diff.back();
		return diff;
	}

	static vector<T> partialSum(const vector<T> &signal) {
		int length = signal.size();
		vector<int> result(length);
		partial_sum(signal.begin(),signal.end(),result.begin());
		return result;
	}

	static int findMinimum(const vector<T> &signal) {
		return min_element(signal.begin(),signal.end())-signal.begin();
	}

	static int findMaximum(const vector<T> &signal) {
		return max_element(signal.begin(),signal.end())-signal.begin();
	}

	static Signal weightBy(const Signal &signal, const Weight &weight) {
		assert(signal.size() == weight.size());
		int length = signal.size();
		Signal result(length);
		for (int i = 0; i < length; i++) {
			result[i] = signal[i] * weight[i];
		}
		return result;
	}

	static T sum(const Signal &signal) {
		int length = signal.size();
		T sum = 0;
		for (int i = 0; i < length; i++) {
			sum += signal[i];
		}
		return sum;
	}

	static T sum(const Signal &signal, const Weight &weight) {
		int length = signal.size();
		T sum = 0;
		for (int i = 0; i < length; i++) {
			sum += signal[i] * weight[i];
		}
		return sum;
	}

	static T mean(const Signal &signal) {
		return sum(signal) / (signal.size());
	}

	static T mean(const Signal &signal, const Weight &weight) {
		return sum(signal,weight) / SignalProcessor<double>::sum(weight);
	}

	// computes the difference of the signal
	static Signal difference(const Signal &signal1, const Signal &signal2) {
		assert(signal1.size() == signal2.size());
		Signal difference;
		for (unsigned int i = 0; i < signal1.size(); i++) {
			difference.push_back(signal1[i]-signal2[i]);
		}
		return difference;
	}

	// resample the given signal to given length
	static Signal resample(const Signal &signal, int sampleLength) {
		int length = signal.size();
		Signal sample(sampleLength);
		for (int i = 0; i < sampleLength; i++) {
			int j = Utils::Convert(double(i) * (length-1) / (sampleLength-1));
			sample[i] = signal[j];
		}
		return sample;
	}

	// resample the given signal using certain scale
	static Signal resample(const Signal &signal, double scale) {
		int length = signal.size();
		int sampleLength = Utils::Convert(length * scale);
		Signal sample(sampleLength);
		for (int i = 0; i < sampleLength; i++) {
			int j = Utils::Convert(double(i) * (length-1) / (sampleLength-1));
			sample[i] = signal[j];
		}
		return sample;
	}

	// convovle the given signal with gauss kernel
	static vector<T> averageFilter(vector<T> signal, int kernelLength) {
		int length = signal.size();
		int kernelHalf = kernelLength/2;
	
		vector<T> result(length);
		for (int i = 0; i < length; i++) {
			for (int j = -kernelHalf; j <= +kernelHalf; j++) {
				result[i] += signal[(i+j+length)%length];
			}
			result[i] /= kernelLength;
		}
	
		return result;
	}

	static Signal multiply(Signal signal, T val) {
		int length = signal.size();
		for (int i = 0; i < length; i++) {
			signal[i] *= val;
		}
		return signal;
	}

	static Signal normalize(const Signal &signal) {
		return multiply(signal,1.0/sum(signal));
	}

	// reduce noise, low pass filter
	// gaussian curve is from -3o to +3o
	static vector<T> gaussianBlur(vector<T> signal, double sigma) {
		int halfLength = 3*sigma;
		vector<double> kernel;
		for (int i = -halfLength; i <= +halfLength; i++) {
			kernel.push_back( exp(- i*i / (2 * sigma * sigma)) / (sqrt( 2 * M_PI ) * sigma) );
		}
		kernel = normalize(kernel);
		signal = circularConvolution(signal,kernel);
		rotate(signal.begin(),signal.begin()+halfLength,signal.end());
		return signal;
	}
	
	// returns all local maximas which are maximas in at least given radius of neighbourhood
	static vector<int> findLocalMaximas(const Signal &signal, int radius) {
		int length = signal.size();
	
		typedef pair<T,int> PackedExtrem;
		vector<PackedExtrem> packedExtrems;
	
		for (int i = 0; i < length; i++) {
			if (isLocalMaximum(i,signal,radius)) {
				packedExtrems.push_back( PackedExtrem(signal[i],i) );
			}
		}
	
		sort(packedExtrems.begin(),packedExtrems.end(),greater<PackedExtrem> ());
	
		int numExtrems = packedExtrems.size();
		vector<int> extrems(numExtrems);
	
		for (int i = 0; i < numExtrems; i++) {
			extrems[i] = packedExtrems[i].second;
		}
		return extrems;
	}
	
	// returns all local minimas which are minimas in at least given radius of neighbourhood
	static vector<int> findLocalMinimas(Signal signal, int radius) {
		for (unsigned int i = 0; i < signal.size(); i++) {
			signal[i] = -signal[i];
		}
		return findLocalMaximas(signal,radius);
	}

};
