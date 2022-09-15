
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
	vector<vector<double>> Point_diffs;
	LoadDiffData("D:/UE5_Project/TTSAPro/AssetsFromOther/axPatDr50.txt", Point_diffs);
	
	CreateTextTo(Point_diffs, 1024, "writetestpng3.png");
	
	////提取驱动模型Dr的obj信息
	//vector<vector<double>> vertexs_Dr = {};
	//vector<vector<double>> normals_Dr = {};
	//vector<vector<double>> uvs_Dr = {};
	//vector<vector<int>> faces_vertexs_Dr = {};
	//vector<vector<int>> faces_uvs_Dr = {};
	//LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/Dr.obj", vertexs_Dr, normals_Dr, uvs_Dr, faces_vertexs_Dr, faces_uvs_Dr);
	////提取爱夏模型的obj信息
	//vector<vector<double>> vertexs_Ax = {};
	//vector<vector<double>> normals_Ax = {};
	//vector<vector<double>> uvs_Ax = {};
	//vector<vector<int>> faces_vertexs_Ax = {};
	//vector<vector<int>> faces_uvs_Ax = {};
	//LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/aixia.obj", vertexs_Ax, normals_Ax, uvs_Ax, faces_vertexs_Ax, faces_uvs_Ax);

	////计算所有的UV对应的空间坐标
	//vector<vector<double>> AxPointAtDr = {};
	//for (auto i= uvs_Ax.begin();i != uvs_Ax.end();i++)
	//{
	//	vector<double> tempResult = CalculateInterpolation(vertexs_Dr, uvs_Dr, faces_vertexs_Dr, faces_uvs_Dr, *i);
	//	AxPointAtDr.push_back(tempResult);
	//}
	//cout << AxPointAtDr.size() << endl;

	//vector<double> testUV = { 0.02024, 0.02105 };
	//vector<double>testResult = CalculateInterpolation(vertexs_Dr, uvs_Dr, faces_vertexs_Dr, faces_uvs_Dr, testUV);
	//cout << testResult[0] << " " << testResult[1] << " " << testResult[2] << " " << testResult[3] << endl;

	return 0;
}

uint8_t* MatToBytes(Mat image)
{
	int size = image.total() * image.elemSize();
	uint8_t* bytes = new uint8_t[size];
	std::memcpy(bytes, image.data, size * sizeof(uint8_t));//拷贝字节到内存中
	return bytes;
};

void createAlphaMat(vector<vector<double>> inPoint_diffs,Mat& mat)
{
	//设置底色为纯黑
	for (int i = 0; i < mat.rows; ++i)
	{
		for (int j = 0; j < mat.cols; ++j)
		{
			Vec4b& rgba = mat.at<Vec4b>(i, j);
			rgba[0] = 0;
			rgba[1] = 0;
			rgba[2] = 0;
			rgba[3] = 255;
		}
	}
	//上色
	for (int i = 0; i < inPoint_diffs.size(); i++)
	{
		int pixelX = (1 - inPoint_diffs[i][1]) * mat.rows;
		int pixelY = inPoint_diffs[i][0] * mat.cols;
		Vec4b& rgba = mat.at<Vec4b>(pixelX, pixelY);
		rgba[0] = inPoint_diffs[i][2];
		rgba[1] = inPoint_diffs[i][3];
		rgba[2] = inPoint_diffs[i][4];
	}
}

void CreateTextTo(vector<vector<double>> inPoint_diffs, int insize, string infilename)
{
	Mat mat(insize, insize, CV_8UC4);
	createAlphaMat(inPoint_diffs,mat);
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
				vector<int> tempFaceVertexIndex(4);
				vector<int> tempFaceUVs(4);
				//读取顶点和uv索引
				ins >> s0;//首先把f去掉
				string s1;
				for (int i = 0; i < 4; i++)	//有的模型是四点面,三点面的模型直接截断掉第四个值就行了
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

vector<double> CalculateInterpolation(vector<vector<double>> invertexs,vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, vector<double> inUV)
{
	vector<double> result = { 0,0,0 };
	for (int i = 0; i < inface_uvindex.size(); i++)
	{
		int uvindex0 = inface_uvindex[i][0];
		int uvindex1 = inface_uvindex[i][1];
		int uvindex2 = inface_uvindex[i][2];
		int vertindex0 = inface_vertindex[i][0];
		int vertindex1 = inface_vertindex[i][1];
		int vertindex2 = inface_vertindex[i][2];
		vector<double> uv0 = inuvs[uvindex0 - 1];	//注意f面提供的序号都是从1开始的
		vector<double> uv1 = inuvs[uvindex1 - 1];
		vector<double> uv2 = inuvs[uvindex2 - 1];
		vector<double> P0 = invertexs[vertindex0 - 1];
		vector<double> P1 = invertexs[vertindex1 - 1];
		vector<double> P2 = invertexs[vertindex2 - 1];

		vector<double> TriRatio = CalculateAreaRatio(uv0, uv1, uv2, inUV);
		if ((TriRatio[0] + TriRatio[1] + TriRatio[2]) <= 1)	//三个比值之和小于1才说明点在三角形内部,然后返回三个插值和面序号
		{
			//直接返回世界坐标(插值也可以顺带返回出去,以便后期计算其他的信息使用)
			result[0] = P0[0] * TriRatio[0] + P1[0] * TriRatio[1] + P2[0] * TriRatio[2];
			result[1] = P0[1] * TriRatio[0] + P1[1] * TriRatio[1] + P2[1] * TriRatio[2];
			result[2] = P0[2] * TriRatio[0] + P1[2] * TriRatio[1] + P2[2] * TriRatio[2];
			//result[3] = i;
			//result[4] = TriRatio[0];
			//result[5] = TriRatio[1];
			//result[6] = TriRatio[2];
			return result;
		}
	}
	return result;
}

vector<double> CalculateAreaRatio(vector<double> uv0, vector<double> uv1, vector<double> uv2, vector<double> inUV)
{
	vector<double> result = { 0,0,0 };
	double S = 0.5 * abs((uv1[0] - uv0[0]) * (uv2[1] - uv0[1]) - (uv1[1] - uv0[1]) * (uv2[0] - uv0[0]));//三角形总面积
	double s0 = 0.5*abs((uv1[0] - inUV[0]) * (uv2[1] - inUV[1]) - (uv1[1] - inUV[1]) * (uv2[0] - inUV[0]));//与uv0对应的三角形面积
	double s1 = 0.5 * abs((inUV[0] - uv0[0]) * (uv2[1] - uv0[1]) - (inUV[1] - uv0[1]) * (uv2[0] - uv0[0]));//与uv1对应的三角形面积
	double s2 = 0.5 * abs((uv1[0] - uv0[0]) * (inUV[1] - uv0[1]) - (uv1[1] - uv0[1]) * (inUV[0] - uv0[0]));//与uv2对应的三角形面积
	result[0] = s0 / S;
	result[1] = s1 / S;
	result[2] = s2 / S;
	return result;
}

void LoadDiffData(string inpath, vector<vector<double>>& Point_diffs)
{
	ifstream infile;
	infile.open(inpath);
	if (!infile.is_open())
	{
		cout << "读取文件失败!" << endl;
	}
	else
	{
		cout << "开始加载数据" << endl;
		string sline;
		while (getline(infile, sline))
		{
			vector<double> tempVector(5);
			istringstream ins(sline);
			ins >> tempVector[0] >> tempVector[1] >> tempVector[2] >> tempVector[3] >> tempVector[4];
			Point_diffs.push_back(tempVector);
		}
		infile.close();
		cout << "加载完毕" << endl;
	}
}




