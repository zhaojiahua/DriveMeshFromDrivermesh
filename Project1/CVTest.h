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

void createAlphaMat(class Mat& mat);

void CreateTextTo(int insize, string infilename);

void LoadObj(string inpath, vector<vector<double>>& vectexs, vector<vector<double>>& normals, vector<vector<double>>& uvs, vector<vector<int>>& faces_vertexs, vector<vector<int>>& faces_uvs);

vector<double> CalculateInterpolation(vector<vector<double>> inuvs,vector<vector<int>> inface_uvindex, vector<double> inUV);//��һ��uvֵ,Ȼ���ж����uv�������ĸ�����,���ҵ�֮��˳��Ѳ�ֵ�������