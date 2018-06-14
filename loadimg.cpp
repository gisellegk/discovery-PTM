#include "opencv2/imgproc.hpp"
#include "opencv2/highgui.hpp"
#include <stdlib.h>
#include <stdio.h>
#include <iostream>

using namespace cv;

/// Global variables

int threshold_value = 0;
int threshold_type = 3;;
int const max_value = 255;
int const max_type = 4;
int const max_BINARY_value = 255;

Mat src, roi, src_gray, dst;
char* window_name = "Threshold Demo";
char* window_name2 = "circle demo";

char* trackbar_type = "Type: \n 0: Binary \n 1: Binary Inverted \n 2: Truncate \n 3: To Zero \n 4: To Zero Inverted";
char* trackbar_value = "Value";

/// Function headers
void Threshold_Demo( int, void* );
void findCircles();
void findBrightSpot(Mat);
/**
 * @function main
 */
int main( int argc, char** argv )
{
  /// Load an image
  src = imread( argv[1], 1 );

  Rect RectangleToSelect(0,0,400,400);
  roi = src(RectangleToSelect);
  std::cout << "initial crop.\n";

  /// Convert the image to Gray
  cvtColor( roi, src_gray, COLOR_RGB2GRAY );
  std::cout << "grayscale\n";
  GaussianBlur( src_gray, src_gray, Size(9, 9), 2, 2 );
  std::cout << "gaussian\n";
  /// Create a window to display results
  namedWindow( window_name, WINDOW_AUTOSIZE );

  /// Call the function to initialize
  findCircles();

  /// Wait until user finishes program
  while(true)
  {
    int c;
    c = waitKey( 20 );
    if( (char)c == 27 )
  { break; }
   }

}


void findCircles(){
  std::cout << "finding circles\n";
  Mat cm = roi.clone();
  Mat ledmat = src_gray.clone();
  std::vector<Vec3f> circles;
  std::cout << "hough Circles \n";
  /// Apply the Hough Transform to find the circles
  HoughCircles( src_gray, circles, HOUGH_GRADIENT, 1, src_gray.rows/8, 50, 50, 0, 0 );
  std::cout << "drawing\n";
  /// Draw the circles detected
  for( size_t i = 0; i < circles.size(); i++ )
  {
    Point center(cvRound(circles[i][0]), cvRound(circles[i][1]));
    int radius = cvRound(circles[i][2]);
    int topRightx = cvRound(circles[i][0]) - radius;
    int topRighty = cvRound(circles[i][1]) - radius;
    // circle center
    circle( cm, center, 3, Scalar(0,255,0), -1, 8, 0 );
    // circle outline
    circle( cm, center, radius, Scalar(0,0,255), 3, 8, 0 );
    if(circles.size() == 1) {
      Rect RectangleToSelect(topRightx,topRighty,radius*2,radius*2);
      cm = cm(RectangleToSelect);
      ledmat = ledmat(RectangleToSelect);
    }
  }
  std::cout << "show image\n";
  /// Show your results
  imshow( window_name2, cm );

  threshold( ledmat, dst, 245, max_BINARY_value, 1 );
  findBrightSpot(dst);
  //imshow( window_name, dst );
}

void findBrightSpot(Mat m) {
  RNG rng(12345);
  /* Search for flame and draw a rectangle around it */
  std::vector<std::vector<Point> > contours;
  std::vector<Vec4i> hierarchy;
  findContours( m, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, Point(0, 0) );
  /// Approximate contours to polygons + get bounding rects and circles
  std::vector<std::vector<Point> > contours_poly( contours.size() );
  //std::vector<Rect> boundRect( contours.size() );
  std::vector<Point2f>center( contours.size() );
  std::vector<float>radius( contours.size() );

  for( int i = 0; i < contours.size(); i++ )
  { approxPolyDP( Mat(contours[i]), contours_poly[i], 3, true );
    //boundRect[i] = boundingRect( Mat(contours_poly[i]) );
    minEnclosingCircle( (Mat)contours_poly[i], center[i], radius[i] );
  }
  /// Draw polygonal contour + bonding rects + circles
  Mat drawing = Mat::zeros( m.size(), CV_8UC3 );
  for( int i = 0; i< contours.size(); i++ )
  {
    //Rect r = boundRect[i];
    //std::cout << "(" << (r.x+(r.width/2.0)) << ", " << (r.y+(r.height/2.0)) << ")" << std::endl;
    Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
    drawContours( drawing, contours_poly, i, color, 1, 8, std::vector<Vec4i>(), 0, Point() );
    //rectangle( drawing, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0 );
    //std::cout << i << ": (" << center[i].x << ", " << center[i].y << ")\n";
    circle( drawing, center[i], (int)radius[i], color, 2, 8, 0 );
  }

  double cp = center[0].x;
  double x = (center[1].x - cp) / (cp);
  double y = (cp - center[1].y ) / (cp);
  double z = std::sqrt(1.0 - std::sqrt(x*x+y*y));

  std::cout << "(x,y,z): (" << x << ", " << y << ", " << z << ") \n";


  imshow(window_name, drawing ); // show frame on computer output window

}
