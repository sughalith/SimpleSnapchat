#include <cv.hpp>
#include <highgui.h>
#include <vector>

cv::Point findLargestFieldWithColor(const cv::Mat & input, const cv::Scalar low, const cv::Scalar high);
