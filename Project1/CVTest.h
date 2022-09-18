#pragma once
#include <iostream>
#include <vector>
#include <sstream>
#include <fstream>
#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp> 

using namespace cv;
using namespace std;

uint8_t* MatToBytes(class Mat image);

void createAlphaMat(vector<vector<double>> inPoint_diffs, class Mat& mat);
void createAlphaMat(vector<vector<double>> invertexs, vector<vector<double>> invtxPos, vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, class Mat& mat, Mat& matN);

void CreateTextTo(vector<vector<double>> inPoint_diffs, int insize, string infilename);
void CreateTextTo(vector<vector<double>> inPoint_diffs, vector<vector<double>> invtxs, vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, int insize, string infilename, string infilenameN);

void LoadObj(string inpath, vector<vector<double>>& vectexs, vector<vector<double>>& normals, vector<vector<double>>& uvs, vector<vector<int>>& faces_vertexs, vector<vector<int>>& faces_uvs);

vector<double> CalculateInterpolation(vector<vector<double>> invertexs, vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, vector<double> inUV);//´«Ò»¸öuvÖµ,È»ºóÅÐ¶ÏÕâ¸öuvÊÇÂäÔÚÄÄ¸öÃæÉÏ,²éÕÒµ½Ö®ºóË³±ã°Ñ²åÖµ¼ÆËã³öÀ´

vector<double> CalculateAreaRatio(vector<double> uv0, vector<double> uv1, vector<double> uv2, vector<double> inUV);
vector<double> CalculateAreaRatio(vector<int> pixel0, vector<int> pixel1, vector<int> pixel2, vector<int> inpixel);

void LoadDiffData(string inpath, vector<vector<double>>& Point_diffs);

vector<int> GetPixelCoord(int inWidth, int inHeight, vector<double> inUV);

vector<int> GetTrianglePixelBoundingBox(vector<int>inpixel0, vector<int>inpixel1, vector<int>inpixel2);

int Round(double inx);		//四舍五入函数

vector<double> CrossProduct(vector<double>invector1, vector<double> invector2);
vector <double> Normalize(vector<double> inVector);

void WriteToFile(string outfilepath, vector<vector<double>> outcontent);