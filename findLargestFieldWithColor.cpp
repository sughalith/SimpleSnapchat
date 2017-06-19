#include "findLargestFieldWithColor.h"

using namespace cv;
using namespace std;

const Mat dilationErosionKernel = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*3 + 1, 2*3+1 ),
                                       Point( 3, 3 ) );
const Mat erosionDilationKernel = getStructuringElement( MORPH_ELLIPSE,
                                       Size( 2*3 + 1, 2*3+1 ),
                                       Point( 3, 3 ) );

cv::Point findLargestFieldWithColor(const cv::Mat & input, const cv::Scalar low, const cv::Scalar high) {
		
		
		Mat frame = input.clone();
		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		
		int maxSizeIndex = 0;
		
		Point center(0,0);
		
		cvtColor( frame, frame, CV_RGB2HSV);
		
		inRange(frame, low, high, frame);
				
		dilate(frame, frame, dilationErosionKernel);
		erode(frame, frame, dilationErosionKernel);
				
		erode(frame, frame, erosionDilationKernel);
		dilate(frame, frame, erosionDilationKernel);
		
		findContours(frame, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE);
				
		{
			double maxSize = -1;
			double tmpSizeStor = 0;
			
			for(int i = 0; i<contours.size(); ++i)
			{
				tmpSizeStor = contourArea(contours[i]);
				if(tmpSizeStor > maxSize) {
					maxSize = tmpSizeStor;
					maxSizeIndex = i;
				}
			}
				
		}
				
		if(!contours.empty()){
			for(int i = 0; i<contours[maxSizeIndex].size(); ++i)
				center += contours[maxSizeIndex][i];
						
			center *= 1.0/(double)contours[maxSizeIndex].size();
		}
		
		return center;
}
