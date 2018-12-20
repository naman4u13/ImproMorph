#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
using namespace cv;
using namespace std;

struct coordinate
{
	int x;
	int y;

};
typedef struct coordinate point;
struct control_points
{
	point pts[3];
	

};
typedef struct control_points cp;
cp srcpts[8];
cp dstpts[8]; 
int rows;
int cols;
Mat arr[25];

float area(int x1, int y1, int x2, int y2, int x3, int y3)
{
	return abs((x1*(y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2)) / 2.0);
}
bool isInside(int x1, int y1, int x2, int y2, int x3, int y3, int x, int y)
{
	/* Calculate area of triangle ABC */
	float A = area(x1, y1, x2, y2, x3, y3);

	/* Calculate area of triangle PBC */
	float A1 = area(x, y, x2, y2, x3, y3);

	/* Calculate area of triangle PAC */
	float A2 = area(x1, y1, x, y, x3, y3);

	/* Calculate area of triangle PAB */
	float A3 = area(x1, y1, x2, y2, x, y);

	/* Check if sum of A1, A2 and A3 is same as A */
	return (A == A1 + A2 + A3);
}
float *calvect(cp imgctrl)
{
	float vector[4];
	vector[0] = imgctrl.pts[1].x - imgctrl.pts[0].x;
	vector[1] = imgctrl.pts[2].x - imgctrl.pts[0].x;
	vector[2] = -imgctrl.pts[1].y + imgctrl.pts[0].y;
	vector[3] = -imgctrl.pts[2].y + imgctrl.pts[0].y;
	return vector;
}
void asssign()
{
	srcpts[0] = { {{215,205},{0,0},{cols - 1,0}} };
	srcpts[1] = { {{215,205},{304,195},{cols - 1,0}} };
	srcpts[2] = { {{304,195},{cols - 1,0},{cols - 1,rows-1}} };
	srcpts[3] = { {{304,195},{269,310}, {cols - 1,rows-1}} };
	srcpts[4] = { {{269,310},{0,rows - 1}, {cols - 1,rows-1}} };
	srcpts[5] = { {{269,310}, {0,rows - 1},{215,205}} };
	srcpts[6] = { {{215,205},{0,0}, {0,rows - 1}} };
	srcpts[7] = { {{215,205},{304,195}, {269,310}} };
	dstpts[0] = { {{181,250},{0,0},{cols - 1,0}} };
	dstpts[1] = { {{181,250},{279,241},{cols - 1,0}} };
	dstpts[2] = { {{279,241},{cols - 1,0},{cols - 1,rows - 1}} };
	dstpts[3] = { {{279,241},{235,363}, {cols - 1,rows - 1}} };
	dstpts[4] = { {{235,363},{0,rows - 1}, {cols - 1,rows - 1}} };
	dstpts[5] = { {{235,363}, {0,rows - 1},{181,250}} };
	dstpts[6] = { {{181,250},{0,0}, {0,rows - 1}} };
	dstpts[7] = { {{181,250},{279,241}, {235,363}} };


}
void drawline(Mat img, cp* imgpts,String dest)
{
	for (int i = 0; i < 8; i++)
	{

		line(img, Point(imgpts[i].pts[0].x, imgpts[i].pts[0].y), Point(imgpts[i].pts[1].x, imgpts[i].pts[1].y), (255, 0, 0), 2);
		line(img, Point(imgpts[i].pts[1].x, imgpts[i].pts[1].y), Point(imgpts[i].pts[2].x, imgpts[i].pts[2].y), (255, 0, 0), 2);
		line(img, Point(imgpts[i].pts[0].x, imgpts[i].pts[0].y), Point(imgpts[i].pts[2].x, imgpts[i].pts[2].y), (255, 0, 0), 2);
	}
	
	String type = ".jpg";
	String path = dest + type;
	imwrite(path,img);

}

Vec3b interpolation(float row, float col, Mat image)
{
	float row1 = floor(row);
	float row2 = ceil(row);
	float col1 = floor(col);
	float col2 = ceil(col);

	//printf("\n\n %f,%f,%f,%f \n\n", row1, row2, col1, col2);
	Vec3b color[4] = { 0 };
	if (round(col1) < cols && round(row1) < rows&&round(row1) > 0 && round(col1) > 0)
	{
		color[0] = image.at<Vec3b>(row1, col1);
	}
	if (round(col2) < cols && round(row1) < rows&&round(row1) > 0 && round(col2) > 0)
	{
		color[1] = image.at<Vec3b>(row1, col2);
	}
	if (round(col1) < cols && round(row2) < rows&&round(row2) > 0 && round(col1) > 0)
	{
		color[2] = image.at<Vec3b>(row2, col1);
	}
	if (round(col2) < cols && round(row2) < rows&&round(row2) > 0 && round(col2) > 0)
	{
		color[3] = image.at<Vec3b>(row2, col2);
	}

	Vec3b result;
	if ((int(row1) == int(row2)) && (int(col1) == int(col2)))
	{
		for (int i = 0; i < 3; i++)
		{
			result[i] = (color[0][i]);
		}
	}
	else if (int(row1) == int(row2))
	{
		for (int i = 0; i < 3; i++)
		{
			result[i] = ((color[0][i])*(col - col1)) + ((color[1][i]) * (col2 - col));
		}

	}
	else if (int(col1) == int(col2))
	{
		for (int i = 0; i < 3; i++)
		{
			result[i] = ((color[0][i])*(row - row1)) + ((color[2][i])*(row2 - row));
		}

	}
	else
	{
		for (int i = 0; i < 3; i++)
		{
			result[i] = ((((color[0][i])*(col - col1)) + ((color[1][i]) * (col2 - col)))*(row - row1)) + ((((color[2][i])*(col - col1)) + ((color[3][i]) * (col2 - col)))*(row2 - row));
		}
	}
	return result;
}

void compute(Mat bush, Mat clinton)
{

	int length = sizeof(arr) / sizeof(arr[0]);
	printf("\n\n%d\n\n", length);
	for (int k = 0; k < length; k++)
	{
		Mat img(rows, cols, CV_8UC3, Scalar(0, 0, 0));
		point imgctrl[3];
		cp imgpts[8];
		for (int i = 0; i < 3; i++)
		{
			imgctrl[i].x = srcpts[7].pts[i].x + (((-srcpts[7].pts[i].x + dstpts[7].pts[i].x) / (length + 1))*(k + 1));
			imgctrl[i].y = srcpts[7].pts[i].y - (((srcpts[7].pts[i].y - dstpts[7].pts[i].y) / (length + 1))*(k + 1));
		}
		{
			imgpts[0] = { {imgctrl[0],{0,0},{cols - 1,0}} };
			imgpts[1] = { {imgctrl[0],imgctrl[1],{cols - 1,0}} };
			imgpts[2] = { {imgctrl[1],{cols - 1,0},{cols - 1,rows - 1}} };
			imgpts[3] = { {imgctrl[1],imgctrl[2], {cols - 1,rows - 1}} };
			imgpts[4] = { {imgctrl[2],{0,rows - 1}, {cols - 1,rows - 1}} };
			imgpts[5] = { {imgctrl[2], {0,rows - 1},imgctrl[0]} };
			imgpts[6] = { {imgctrl[0],{0,0}, {0,rows - 1}} };
			imgpts[7] = { {imgctrl[0],imgctrl[1], imgctrl[2]} };


		}
		
		for (int i = 0; i < rows; i++)
		{
			for (int j = 0; j < cols; j++)
			{
				int n =0;
				for (int l = 0; l < 8; l++)
				{
					
					if (isInside(imgpts[l].pts[0].x, imgpts[l].pts[0].y, imgpts[l].pts[1].x, imgpts[l].pts[1].y, imgpts[l].pts[2].x, imgpts[l].pts[2].y,j,i))
					{
						n = l;
						break;
					}
				}
				float *vector = calvect(imgpts[n]);
				float xs = j - imgpts[n].pts[0].x;
				float ys = imgpts[n].pts[0].y - i;
				float alpha = (((vector[3] * xs) - (vector[1] * ys)) / ((vector[0] * vector[3]) - (vector[1] * vector[2])));
				float beta = (((vector[0] * ys) - (vector[2] * xs)) / ((vector[0] * vector[3]) - (vector[1] * vector[2])));
				float *dstdis = calvect(dstpts[n]);
				float *srcdis = calvect(srcpts[n]);
				float xdst = (alpha * dstdis[0]) + (beta * dstdis[1]) + dstpts[n].pts[0].x;
				float ydst = -((alpha * dstdis[2]) + (beta * dstdis[3])) + dstpts[n].pts[0].y;
				float xsrc = (alpha * srcdis[0]) + (beta * srcdis[1]) + srcpts[n].pts[0].x;
				float ysrc = -((alpha * srcdis[2]) + (beta * srcdis[3])) + srcpts[n].pts[0].y;

				Vec3b srccolor = 0;
				Vec3b dstcolor = 0;
				/*if (round(xsrc) < cols && round(ysrc) < rows&&round(ysrc) >0 && round(xsrc) >0)
				{

					srccolor = bush.at<Vec3b>((int)round(ysrc), (int)round(xsrc));
				}
				if (round(xdst) < cols && round(ydst) < rows&&round(xdst)>0 && round(ydst)>0)
				{

					dstcolor = clinton.at<Vec3b>((int)round(ydst), (int)round(xdst));
				}*/
				srccolor = interpolation(ysrc, xsrc, bush);
				dstcolor = interpolation(ydst, xdst, clinton);

				for (int c = 0; c < 3; c++)
				{
					img.at<Vec3b>(i, j)[c] = ((((srccolor[c])* (length - k)) + (dstcolor[c] * (k + 1))) / (length + 1));

				}
			}

		}
		arr[k] = img;
		/*String window = "Intermediate " + to_string(k);
		namedWindow(window);
		imshow(window, img);*/

	}

}

int main()
{
	Mat bush;
	Mat clinton;
	// Read the image file
	bush = imread("Bush.jpg");

	clinton = imread("Clinton.jpg");
	// Check for failure
	String windowbush = "Bush";
	namedWindow(windowbush);
	String windowclinton = "Clinton";
	namedWindow(windowclinton);
	imshow(windowbush, bush);
	imshow(windowclinton, clinton);
	rows = bush.rows;
	cols = bush.cols;
	waitKey(0);
	asssign();
	drawline(bush, srcpts, "bushtri");
	drawline(clinton, dstpts, "clintontri");
	compute(bush, clinton);

	Size frameSize(cols, rows);
	VideoWriter video = VideoWriter("last.mp4", CV_FOURCC('8', 'B', 'P', 'S'), 30, frameSize);
	int length = sizeof(arr) / sizeof(arr[0]);
	video.write(bush);
	for (int i = 0; i < length; i++)
	{
		if ((i + 1) % 5 == 0)
		{
			String ppath = "img";
			String type = ".jpg";
			String dest = to_string((i + 1) / 5);
			String path = ppath + dest + type;
			imwrite(path, arr[i]);
		}
		video.write(arr[i]);
	}
	video.write(clinton);
	//waitKey(0);
	destroyAllWindows();
	video.release();
	
	return 0;
}