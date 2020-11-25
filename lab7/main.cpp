#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <iostream>
#include <Windows.h>

bool comparator_Vec3b(const COORD a, const COORD b)
{
	return a.X < b.X || a.X == b.X && a.Y < b.Y;
}

bool cw(COORD a, COORD b, COORD c) {
	return a.X * (b.Y - c.Y) + b.X * (c.Y - a.Y) + c.X * (a.Y - b.Y) < 0;
}

bool ccw(COORD a, COORD b, COORD c) {
	return a.X * (b.Y - c.Y) + b.X * (c.Y - a.Y) + c.X * (a.Y - b.Y) > 0;
}

void convex(std::vector<COORD>& points)
{
	std::sort(points.begin(), points.end(), &comparator_Vec3b);

	auto left = points[0];
	auto right = points.back();
	std::vector<COORD> up_plane, down_plane;
	up_plane.push_back(left);
	down_plane.push_back(left);

	for (int i = 1; i < points.size(); ++i)
	{
		if (i == points.size() - 1 || cw(left, points[i], right))
		{
			while (up_plane.size() >= 2 && !cw(up_plane[up_plane.size() - 2], up_plane[up_plane.size() - 1], points[i]))
				up_plane.pop_back();
			up_plane.push_back(points[i]);
		}
		if (i == points.size() - 1 || ccw(left, points[i], right)) {
			while (down_plane.size() >= 2 && !ccw(down_plane[down_plane.size() - 2], down_plane[down_plane.size() - 1], points[i]))
				down_plane.pop_back();
			down_plane.push_back(points[i]);
		}
	}

	points.clear();

	for (auto i : up_plane)
		points.push_back(i);

	for (auto i : down_plane)
		points.push_back(i);
}


int main(int argc, char* argv[])
{
	std::string file_name;
	if (argc > 1)
	{
		file_name  = argv[1];
	}		
	else
	{
		return -1;
	}
	cv::Mat picture = cv::imread(file_name,cv::IMREAD_COLOR);
	cv::Mat gray_picture ;
	cv::cvtColor(picture, gray_picture, cv::COLOR_BGR2GRAY);
	cv::medianBlur(gray_picture, gray_picture, 5);
	std::vector<COORD> points;
	
	
	for (int i = 0; i < picture.rows; i++)
	{
		for (int j = 0; j < picture.cols; j++)
		{
			cv::Vec3b bgrPixel = picture.at<cv::Vec3b>(i, j);
			if (bgrPixel[0] != 255 || bgrPixel[1] != 255 || bgrPixel[2] != 255)
			{
				COORD point;
				point.X = i;
				point.Y = j;
				points.push_back(point);				
			}
		}
	}	

	convex(points);

	for (int i = 1; i < points.size(); ++i)
	{
		if (points[i].X == points[0].X && points[i].Y == points[0].Y)
			continue;
		cv::line(picture, cv::Point(points[i - 1].Y, points[i - 1].X), cv::Point(points[i].Y, points[i].X), cv::Scalar(255, 0, 0), 2);
	}	

	cv::imshow("test", picture);
	cv::waitKey();
	return 0;
}