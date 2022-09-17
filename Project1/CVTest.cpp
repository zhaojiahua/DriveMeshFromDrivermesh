﻿
#include "CVTest.h"
#include "cmath"


using namespace cv;
using namespace std;

int main()
{
	//Mat img = imread("writetestpng3.png");//¶ÁÈ¡Í¼Æ¬
	//uint8_t* img_bytes = NULL;
	////cout << img.total() ;
	//img_bytes = img.data;

	////×¢ÒâOpenCV¶ÁÈ¡Í¼Æ¬µÄ·½Ê½ÊÇ´ÓÉÏÍùÏÂ°´ÐÐ¶ÁÈ¡ÏñËØ,Ã¿´Î°´Ë³Ðò¶ÁÈ¡3¸öÍ¨µÀ

	//for (int i = 524288; i < 1048576; i++)
	//{
	//	if (img_bytes[i] != 0)
	//	{
	//		cout << (int)img_bytes[i] << " ";
	//	}
	//}
	
	//ÌáÈ¡Çý¶¯Ä£ÐÍDrµÄobjÐÅÏ¢
	vector<vector<double>> vertexs_Dr = {};
	vector<vector<double>> normals_Dr = {};
	vector<vector<double>> uvs_Dr = {};
	vector<vector<int>> faces_vertexs_Dr = {};
	vector<vector<int>> faces_uvs_Dr = {};		//×¢ÒâÕâÀïµÃµ½µÄÊÇUVË÷Òý
	LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/Dr.obj", vertexs_Dr, normals_Dr, uvs_Dr, faces_vertexs_Dr, faces_uvs_Dr);
	//ÌáÈ¡°®ÏÄÄ£ÐÍµÄobjÐÅÏ¢
	vector<vector<double>> vertexs_Ax = {};
	vector<vector<double>> normals_Ax = {};
	vector<vector<double>> uvs_Ax = {};
	vector<vector<int>> faces_vertexs_Ax = {};
	vector<vector<int>> faces_uvs_Ax = {};
	LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/aixia.obj", vertexs_Ax, normals_Ax, uvs_Ax, faces_vertexs_Ax, faces_uvs_Ax);

	vector<vector<double>> Point_diffs;
	LoadDiffData("D:/UE5_Project/TTSAPro/AssetsFromOther/axPatDr50.txt", Point_diffs);

	CreateTextTo(Point_diffs, uvs_Ax, faces_vertexs_Ax, faces_uvs_Ax, 1024, "writetestpng3.png");

	////¼ÆËãËùÓÐµÄUV¶ÔÓ¦µÄ¿Õ¼ä×ø±ê
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
	std::memcpy(bytes, image.data, size * sizeof(uint8_t));//¿½±´×Ö½Úµ½ÄÚ´æÖÐ
	return bytes;
};

void createAlphaMat(vector<vector<double>> inPoint_diffs,Mat& mat)
{
	//ÉèÖÃµ×É«Îª´¿»Ò
	for (int i = 0; i < mat.rows; ++i)
	{
		for (int j = 0; j < mat.cols; ++j)
		{
			Vec4b& rgba = mat.at<Vec4b>(i, j);
			rgba[0] = 127;
			rgba[1] = 127;
			rgba[2] = 127;
			rgba[3] = 255;
		}
	}
	//ÉÏÉ«
	for (int i = 0; i < inPoint_diffs.size(); i++)
	{
		int pixelX = (1 - inPoint_diffs[i][4]) * mat.rows;
		int pixelY = inPoint_diffs[i][3] * mat.cols;
		Vec4b& rgba = mat.at<Vec4b>(pixelX, pixelY);
		rgba[0] = inPoint_diffs[i][1];
		rgba[1] = inPoint_diffs[i][2];
		rgba[2] = inPoint_diffs[i][0];
	}
}

void createAlphaMat(vector<vector<double>> inPoint_diffs, vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, Mat& mat)
{
	//给背景上纯灰色
	for (int i = 0; i < mat.rows; ++i)
	{
		for (int j = 0; j < mat.cols; ++j)
		{
			Vec4b& rgba = mat.at<Vec4b>(i, j);
			rgba[0] = 127;
			rgba[1] = 127;
			rgba[2] = 127;
			rgba[3] = 255;
		}
	}
	//ÉÏÉ«(Öð¸öÃæÉÏÉ«¶ø²»ÊÇÖðµãÉÏÉ«)
	for (int i = 0; i < inface_vertindex.size(); i++)
	{
		//ËÄ±ßÐÎÃæ·Ö¸î³ÉÁ½¸öÈý½ÇÐÎ
		for (int j = 0; j < inface_vertindex[i].size() - 2; j++)	//¶¥µãÊý-2=ÃæÊý
		{
			//ÏÈËã³öÈý¸ö¶¥µãµÄÆÁÄ»×ø±êºÍ²îÖµÐÅÏ¢
			//Èý½ÇÃæµÄ¶¥µãË÷Òý
			int vtxIndex0=inface_vertindex[i][0];
			int vtxIndex1 = inface_vertindex[i][j+1];
			int vtxIndex2 = inface_vertindex[i][j+2];
			//Èý½ÇÃæµÄUVË÷Òý
			int uvIndex0 = inface_uvindex[i][0];
			int uvIndex1 = inface_uvindex[i][j + 1];
			int uvIndex2 = inface_uvindex[i][j + 2];
			//三角形三个顶点的三维空间坐标(注意读取的文件中索引是从1开始的)
			vector<double> vtx0 = inPoint_diffs[vtxIndex0-1];		//注意这里面的diffs是按照点序排列的
			vector<double> vtx1 = inPoint_diffs[vtxIndex1-1];
			vector<double> vtx2 = inPoint_diffs[vtxIndex2-1];
			//三角形三个顶点的UV空间坐标
			vector<double> uv0 = inuvs[uvIndex0-1];
			vector<double> uv1 = inuvs[uvIndex1-1];
			vector<double> uv2 = inuvs[uvIndex2-1];
			//三角形三个顶点的像素空间坐标
			vector<int> pixel0 = GetPixelCoord(mat.rows, mat.cols, uv0);
			vector<int> pixel1 = GetPixelCoord(mat.rows, mat.cols, uv1);
			vector<int> pixel2 = GetPixelCoord(mat.rows, mat.cols, uv2);
			
			vector<int> bbox = GetTrianglePixelBoundingBox(pixel0, pixel1, pixel2);
			//cout << bbox[0] << " " << bbox[1] << " " << bbox[2] << " " << bbox[3] << endl;
			/*Vec4b& bgra0 = mat.at<Vec4b>(pixel0[0], pixel0[1]);
			Vec4b& bgra1 = mat.at<Vec4b>(pixel1[0], pixel1[1]);
			Vec4b& bgra2 = mat.at<Vec4b>(pixel2[0], pixel2[1]);
			bgra0[2] = 255;
			bgra1[2] = 255;
			bgra2[2] = 255;*/
			
			for (int y = bbox[1]; y < bbox[3]; y++)
			{
				for (int x = bbox[0]; x < bbox[2]; x++)
				{
					vector<int> inpixel = { x,y };
					vector<double> pixelRatio = CalculateAreaRatio(pixel0, pixel1, pixel2, inpixel);
					//cout << pixelRatio[0] << " " << pixelRatio[1] << pixelRatio[2] << endl;
					//vector<double> pixelRatio = { 0.2,0.3,0.5 };
					if ((pixelRatio[0] + pixelRatio[1] + pixelRatio[2]) <= 1)
					{
						Vec4b& bgra = mat.at<Vec4b>(x, y);
						bgra[2] = vtx0[0] * pixelRatio[0] + vtx1[0] * pixelRatio[1] + vtx2[0] * pixelRatio[2];
						bgra[0] = vtx0[1] * pixelRatio[0] + vtx1[1] * pixelRatio[1] + vtx2[1] * pixelRatio[2];
						bgra[1] = vtx0[2] * pixelRatio[0] + vtx1[2] * pixelRatio[1] + vtx2[2] * pixelRatio[2];
					}
				}
			}
		}
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
		//imshow("Éú³ÉµÄPNGÍ¼", mat);
		//waitKey(0);
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "×ª»»´íÎó", ex.what());
	}
}

void CreateTextTo(vector<vector<double>> inPoint_diffs, vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, int insize, string infilename)
{
	Mat mat(insize, insize, CV_8UC4);
	createAlphaMat(inPoint_diffs, inuvs, inface_vertindex, inface_uvindex, mat);
	//createAlphaMat(inPoint_diffs, mat);
	//mat.at<Vec4b>(0, 0) = {10,20,100,100};
	//mat.at<Vec4b>(1, 0) ={200,250,10,200};
	vector<int>compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	try
	{
		imwrite(infilename, mat, compression_params);
		//imshow("Éú³ÉµÄPNGÍ¼", mat);
		//waitKey(0);
	}
	catch (const std::exception& ex)
	{
		fprintf(stderr, "×ª»»´íÎó", ex.what());
	}
}

void LoadObj(string inpath, vector<vector<double>>& vectexs, vector<vector<double>>& normals, vector<vector<double>>& uvs, vector<vector<int>>& faces_vertexs, vector<vector<int>>& faces_uvs)
{
	ifstream infile;
	infile.open(inpath);
	if (!infile.is_open())
	{
		cout << "open failed!" << endl;
	}
	else
	{
		cout << "data loading..." << endl;
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
				//¶ÁÈ¡¶¥µãºÍuvË÷Òý
				ins >> s0;//Ê×ÏÈ°ÑfÈ¥µô
				string s1;
				for (int i = 0; i < 4; i++)	//ÓÐµÄÄ£ÐÍÊÇËÄµãÃæ,ÈýµãÃæµÄÄ£ÐÍÖ±½Ó½Ø¶ÏµôµÚËÄ¸öÖµ¾ÍÐÐÁË
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
		vector<double> uv0 = inuvs[uvindex0 - 1];	//×¢ÒâfÃæÌá¹©µÄÐòºÅ¶¼ÊÇ´Ó1¿ªÊ¼µÄ
		vector<double> uv1 = inuvs[uvindex1 - 1];
		vector<double> uv2 = inuvs[uvindex2 - 1];
		vector<double> P0 = invertexs[vertindex0 - 1];
		vector<double> P1 = invertexs[vertindex1 - 1];
		vector<double> P2 = invertexs[vertindex2 - 1];

		vector<double> TriRatio = CalculateAreaRatio(uv0, uv1, uv2, inUV);
		if ((TriRatio[0] + TriRatio[1] + TriRatio[2]) <= 1)	//Èý¸ö±ÈÖµÖ®ºÍÐ¡ÓÚ1²ÅËµÃ÷µãÔÚÈý½ÇÐÎÄÚ²¿,È»ºó·µ»ØÈý¸ö²åÖµºÍÃæÐòºÅ
		{
			//Ö±½Ó·µ»ØÊÀ½ç×ø±ê(²åÖµÒ²¿ÉÒÔË³´ø·µ»Ø³öÈ¥,ÒÔ±ãºóÆÚ¼ÆËãÆäËûµÄÐÅÏ¢Ê¹ÓÃ)
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
	double S = 0.5 * abs((uv1[0] - uv0[0]) * (uv2[1] - uv0[1]) - (uv1[1] - uv0[1]) * (uv2[0] - uv0[0]));//Èý½ÇÐÎ×ÜÃæ»ý
	double s0 = 0.5*abs((uv1[0] - inUV[0]) * (uv2[1] - inUV[1]) - (uv1[1] - inUV[1]) * (uv2[0] - inUV[0]));//Óëuv0¶ÔÓ¦µÄÈý½ÇÐÎÃæ»ý
	double s1 = 0.5 * abs((inUV[0] - uv0[0]) * (uv2[1] - uv0[1]) - (inUV[1] - uv0[1]) * (uv2[0] - uv0[0]));//Óëuv1¶ÔÓ¦µÄÈý½ÇÐÎÃæ»ý
	double s2 = 0.5 * abs((uv1[0] - uv0[0]) * (inUV[1] - uv0[1]) - (uv1[1] - uv0[1]) * (inUV[0] - uv0[0]));//Óëuv2¶ÔÓ¦µÄÈý½ÇÐÎÃæ»ý
	result[0] = s0 / S;
	result[1] = s1 / S;
	result[2] = s2 / S;
	return result;
}

vector<double> CalculateAreaRatio(vector<int> pixel0, vector<int> pixel1, vector<int> pixel2, vector<int> inpixel)
{
	vector<double> result = { 0,0,0 };
	double S = abs((pixel1[0] - pixel0[0]) * (pixel2[1] - pixel0[1]) - (pixel1[1] - pixel0[1]) * (pixel2[0] - pixel0[0]));//Èý½ÇÐÎ×ÜÃæ»ý
	double s0 = abs((pixel1[0] - inpixel[0]) * (pixel2[1] - inpixel[1]) - (pixel1[1] - inpixel[1]) * (pixel2[0] - inpixel[0]));//Óëuv0¶ÔÓ¦µÄÈý½ÇÐÎÃæ»ý
	double s1 = abs((inpixel[0] - pixel0[0]) * (pixel2[1] - pixel0[1]) - (inpixel[1] - pixel0[1]) * (pixel2[0] - pixel0[0]));//Óëuv1¶ÔÓ¦µÄÈý½ÇÐÎÃæ»ý
	double s2 = abs((pixel1[0] - pixel0[0]) * (inpixel[1] - pixel0[1]) - (pixel1[1] - pixel0[1]) * (inpixel[0] - pixel0[0]));//Óëuv2¶ÔÓ¦µÄÈý½ÇÐÎÃæ»ý
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
		cout << "open failed!" << endl;
	}
	else
	{
		cout << "data loading..." << endl;
		string sline;
		while (getline(infile, sline))
		{
			vector<double> tempVector(5);
			istringstream ins(sline);
			ins >> tempVector[0] >> tempVector[1] >> tempVector[2] >> tempVector[3] >> tempVector[4];		//把点的UV值放在最后面2位
			Point_diffs.push_back(tempVector);
		}
		infile.close();
		cout << "loaded over!" << endl;
	}
}

vector<int> GetPixelCoord(int inWidth, int inHeight, vector<double> inUV)
{
	vector<int> outResult = { 0,0 };
	int outResult0 = Round((1 - inUV[1]) * inWidth);	//四舍五入
	int outResult1 = Round(inUV[0] * inHeight);
	outResult0 >= inWidth ? outResult[0] = inWidth-1 : outResult[0] = outResult0;		//对输入的值钳制
	outResult1 >= inHeight ? outResult[1] = inHeight-1 : outResult[1] = outResult1;
	return outResult;
}

vector<int> GetTrianglePixelBoundingBox(vector<int>inpixel0, vector<int>inpixel1, vector<int>inpixel2)
{
	vector<int> outBox;
	int minx = inpixel0[0];
	int miny = inpixel0[1];
	int maxx = inpixel0[0];
	int maxy = inpixel0[1];
	if (minx > inpixel1[0])minx = inpixel1[0];
	if (minx > inpixel2[0])minx = inpixel2[0];
	if (miny > inpixel1[1])miny = inpixel1[1];
	if (miny > inpixel2[1])miny = inpixel2[1];
	if (maxx < inpixel1[0])maxx = inpixel1[0];
	if (maxx < inpixel2[0])maxx = inpixel2[0];
	if (maxy < inpixel1[1])maxy = inpixel1[1];
	if (maxy < inpixel2[1])maxy = inpixel2[1];
	outBox.push_back(minx);
	outBox.push_back(miny);
	outBox.push_back(maxx);
	outBox.push_back(maxy);
	return outBox;
}

int Round(double inx)
{
	return (inx > 0.0) ? floor(inx + 0.5) : ceil(inx - 0.5);
}


