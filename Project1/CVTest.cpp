
#include "CVTest.h"
#include "cmath"

using namespace cv;
using namespace std;

int main()
{
	//Mat img = imread("testTEX8.png");//读取图片
	//uint8_t* img_bytes = NULL;
	////cout << img.total() * img.elemSize();
	//img_bytes = img.data;

	////注意OpenCV读取图片的方式是从上往下按行读取像素,每次按顺序读取3个通道

	//for (int i = 0; i < 3072; i++)
	//{
	//	cout << (int)img_bytes[i] << " ";
	//}
	
	
	//CreateTextTo(1024, "writetestpng3.png");
	//提取驱动模型Dr的obj信息
	vector<vector<double>> vertexs_Dr;
	vector<vector<double>> normals_Dr;
	vector<vector<double>> uvs_Dr;
	vector<vector<int>> faces_vertexs_Dr;
	vector<vector<int>> faces_uvs_Dr;
	LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/Dr.obj", vertexs_Dr, normals_Dr, uvs_Dr, faces_vertexs_Dr, faces_uvs_Dr);
	//提取爱夏模型的obj信息
	vector<vector<double>> vertexs_Ax;
	vector<vector<double>> normals_Ax;
	vector<vector<double>> uvs_Ax;
	vector<vector<int>> faces_vertexs_Ax;
	vector<vector<int>> faces_uvs_Ax;
	LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/aixia.obj", vertexs_Ax, normals_Ax, uvs_Ax, faces_vertexs_Ax, faces_uvs_Ax);

	
	return 0;
}

uint8_t* MatToBytes(Mat image)
{
	int size = image.total() * image.elemSize();
	uint8_t* bytes = new uint8_t[size];
	std::memcpy(bytes, image.data, size * sizeof(uint8_t));//拷贝字节到内存中
	return bytes;
};

void createAlphaMat(Mat& mat)
{
	for (int i = 0; i < mat.rows; ++i)
	{
		for (int j = 0; j < mat.cols; ++j)
		{
			Vec4b& rgba = mat.at<Vec4b>(i, j);
			//rgba[0] = UCHAR_MAX;
			//rgba[1] = saturate_cast<uchar>((float(mat.cols - j)) / ((float)mat.cols) * UCHAR_MAX);
			//rgba[2] = saturate_cast<uchar>((float(mat.cols - i)) / ((float)mat.rows) * UCHAR_MAX);
			//rgba[3] = rgba[1] = saturate_cast<uchar>(rgba[1] + rgba[2]);
			rgba[0] = 0;
			rgba[1] = 0;
			rgba[2] = (i / 1024.0) * 255;
			rgba[3] = 255;
		}
	}
}

void CreateTextTo(int insize, string infilename)
{
	Mat mat(insize, insize, CV_8UC4);
	createAlphaMat(mat);
	vector<int>compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	try
	{
		imwrite(infilename, mat, compression_params);
		//imshow("生成的PNG图", mat);
		//waitKey(0);
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "转换错误", ex.what());
	}
}

void LoadObj(string inpath, vector<vector<double>>& vectexs, vector<vector<double>>& normals, vector<vector<double>>& uvs, vector<vector<int>>& faces_vertexs, vector<vector<int>>& faces_uvs)
{
	ifstream infile;
	infile.open(inpath);
	if (!infile.is_open())
	{
		cout << "读取文件失败!" << endl;
	}
	else
	{
		cout << "成功打开文件!" << endl;
		string sline, s0;
		while (getline(infile, sline))
		{
			if (sline[0] == 'v')
			{
				if (sline[1] == 't')
				{
					vector<double> tempUV(3);
					istringstream ins(sline);
					ins >> s0 >> tempUV[0] >> tempUV[1] >> tempUV[2];
					uvs.push_back(tempUV);
				}
				else if (sline[1] == 'n')
				{
					vector<double> tempnormal(3);
					istringstream ins(sline);
					ins >> s0 >> tempnormal[0] >> tempnormal[1] >> tempnormal[2];
					normals.push_back(tempnormal);
				}
				else
				{
					vector<double> tempvertex(3);
					istringstream ins(sline);
					ins >> s0 >> tempvertex[0] >> tempvertex[1] >> tempvertex[2];
					vectexs.push_back(tempvertex);
				}
			}
			else if (sline[0] == 'f')
			{
				istringstream ins(sline);
				vector<int> tempFaceVertexIndex(3);
				vector<int> tempFaceUVs(3);
				//读取顶点和uv索引
				ins >> s0;//首先把f去掉
				string s1;
				for (int i = 0; i < 3; i++)
				{
					ins >> s1;
					int a = 0, k;
					for (k = 0; s1[k] != '/'; k++)
					{
						a = a * 10 + (s1[k] - 48);
					}
					tempFaceVertexIndex[i] = a;
					a = 0;
					for (k = k + 1; s1[k]; k++)
					{
						a = a * 10 + (s1[k] - 48);
					}
					tempFaceUVs[i] = a;
				}
				faces_vertexs.push_back(tempFaceVertexIndex);
				faces_uvs.push_back(tempFaceUVs);
			}
		}
		
		infile.close();
	}
}

vector<double> CalculateInterpolation(vector<vector<double>> inuvs, vector<vector<int>> inface_uvindex, vector<double> inUV)
{
	for (int i = 0; i < inface_uvindex.size(); i++)
	{
		int uvindex0 = inface_uvindex[i][0];
		int uvindex1 = inface_uvindex[i][1];
		int uvindex2 = inface_uvindex[i][2];
		vector<double> uv0 = inuvs[uvindex0];
		vector<double> uv1 = inuvs[uvindex1];
		vector<double> uv2 = inuvs[uvindex2];


	}
	return vector<double>();
}




