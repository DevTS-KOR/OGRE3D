#if !defined(__DTW_H__)
#define __DTW_H__

#include <vector>
#include <limits>
#include <functional>

using namespace std;

template<typename T>
class SimpleDTW
{
	vector<vector<float>> mDistance;
	vector<T> mX;
	vector<T> mY;

public:

    SimpleDTW(vector<T> &x, vector<T> &y, pointer_to_unary_function<T, float> length)
	{
		mX = x;
		mY = y;

		mDistance = std::vector<std::vector<float>>(x.size(), std::vector<float>(y.size()));

		for (int i = 0; i < x.size(); ++i)
		{
			for (int j = 0; j < y.size(); ++j)
			{
				mDistance[i][j] = length(x[i] - y[j]);
			}
		}

		for (int i = 0; i < x.size() - 1; ++i)
			mDistance[i][y.size() - 1] = numeric_limits<float>::max();

		for (int j = 0; j < y.size() - 1; ++j)
            mDistance[x.size() - 1][j] = numeric_limits<float>::max();
	}

    // fabs function이 반드시 double --> double 이기 때문에 어쩔 수 없이 추가함.
    SimpleDTW(vector<T> &x, vector<T> &y, pointer_to_unary_function<double, double> length)
	{
		mX = x;
		mY = y;

		mDistance = std::vector<std::vector<float>>(x.size(), std::vector<float>(y.size()));

		for (int i = 0; i < x.size(); ++i)
		{
			for (int j = 0; j < y.size(); ++j)
			{
				mDistance[i][j] = length(x[i] - y[j]);
			}
		}

		for (int i = 0; i < x.size() - 1; ++i)
			mDistance[i][y.size() - 1] = numeric_limits<float>::max();

		for (int j = 0; j < y.size() - 1; ++j)
			mDistance[x.size() - 1][j] = numeric_limits<float>::max();
	}


	float calculateDistance(void) 
	{

		int xcount = 0;
		int ycount = 0;
	
		float sum = mDistance[0][0];

		while (xcount < mX.size() - 1 && ycount < mY.size() - 1) {
			if (mDistance[xcount][ycount + 1] <= mDistance[xcount + 1][ycount] && mDistance[xcount][ycount + 1] <= mDistance[xcount + 1][ycount + 1]) {
				ycount++;
			} 
			else if (mDistance[xcount + 1][ycount + 1] <= mDistance[xcount][ycount + 1] && mDistance[xcount + 1][ycount + 1] <= mDistance[xcount + 1][ycount]) {
				xcount++;
				ycount++;
			}
			else if (mDistance[xcount + 1][ycount] <= mDistance[xcount + 1][ycount + 1] && mDistance[xcount + 1][ycount] <= mDistance[xcount][ycount + 1]) {
				xcount++;
			}
			sum += mDistance[xcount][ycount];
		}

		return sum;
	}
};


#endif
