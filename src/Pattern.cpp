////////////////////////////////////////////////////////////////////
// File includes:
#include "Pattern.hpp"

void PatternTrackingInfo::computePose(const Pattern& pattern, const CameraCalibration& calibration)
{
  cv::Mat camMatrix, distCoeff;
  cv::Mat(3,3, CV_32F, const_cast<float*>(&calibration.getIntrinsic().data[0])).copyTo(camMatrix);
  cv::Mat(4,1, CV_32F, const_cast<float*>(&calibration.getDistorsion().data[0])).copyTo(distCoeff);

  cv::Mat Rvec;
  cv::Mat_<float> Tvec;
  cv::Mat raux,taux;

  cv::solvePnP(pattern.points3d, points2d, camMatrix, distCoeff,raux,taux);
  raux.convertTo(Rvec,CV_32F);
  taux.convertTo(Tvec ,CV_32F);

  cv::Mat_<float> rotMat(3,3); 
  cv::Rodrigues(Rvec, rotMat);

  // Copy to transformation matrix
  for (int col=0; col<3; col++)
  {
    for (int row=0; row<3; row++)
    {        
     pose3d.r().mat[row][col] = rotMat(row,col); // Copy rotation component
    }
    pose3d.t().data[col] = Tvec(col); // Copy translation component
  }

  // Since solvePnP finds camera location, w.r.t to marker pose, to get marker pose w.r.t to the camera we invert it.
  pose3d = pose3d.getInverted();
}

void PatternTrackingInfo::draw2dContour(cv::Mat& image, cv::Scalar color) const
{
  for (size_t i=0; i < points2d.size();i++)
  {
    cv::line(image, points2d[i], points2d[ (i+1) % points2d.size() ], color, 2, CV_AA);
  }
}

