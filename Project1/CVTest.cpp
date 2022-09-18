
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

	//for (int i = 0; i < 1048576; i++)
	//{
	//	if (img_bytes[i] < 0)
	//	{
	//		cout << (int)img_bytes[i] << " ";
	//	}
	//}
	
	//load Dr.obj
	vector<vector<double>> vertexs_Dr = {};
	vector<vector<double>> normals_Dr = {};
	vector<vector<double>> uvs_Dr = {};
	vector<vector<int>> faces_vertexs_Dr = {};
	vector<vector<int>> faces_uvs_Dr = {};		//×¢ÒâÕâÀïµÃµ½µÄÊÇUVË÷Òý
	LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/Dr.obj", vertexs_Dr, normals_Dr, uvs_Dr, faces_vertexs_Dr, faces_uvs_Dr);
	//load Ax.obj
	vector<vector<double>> vertexs_Ax = {};
	vector<vector<double>> normals_Ax = {};
	vector<vector<double>> uvs_Ax = {};
	vector<vector<int>> faces_vertexs_Ax = {};
	vector<vector<int>> faces_uvs_Ax = {};
	LoadObj("D:/UE5_Project/TTSAPro/AssetsFromOther/aixia.obj", vertexs_Ax, normals_Ax, uvs_Ax, faces_vertexs_Ax, faces_uvs_Ax);
	vector<vector<double>> Point_diffs;
	LoadDiffData("D:/UE5_Project/TTSAPro/AssetsFromOther/axPatDr50.txt", Point_diffs);
	CreateTextTo(Point_diffs, vertexs_Ax, uvs_Ax, faces_vertexs_Ax, faces_uvs_Ax, 1024, "writetestpng.png", "writetestpngN.png");

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
			Vec4f& rgba = mat.at<Vec4f>(i, j);
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
		Vec4f& rgba = mat.at<Vec4f>(pixelX, pixelY);
		rgba[0] = inPoint_diffs[i][1];
		rgba[1] = inPoint_diffs[i][2];
		rgba[2] = inPoint_diffs[i][0];
	}
}

void createAlphaMat(vector<vector<double>> inPoint_diffs, vector<vector<double>> invtxPos, vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, Mat& mat, Mat& matN)
{
	//给背景上纯灰色
	for (int i = 0; i < mat.rows; ++i)
	{
		for (int j = 0; j < mat.cols; ++j)
		{
			Vec4f& rgba = mat.at<Vec4f>(i, j);
			rgba[0] = 127.0;
			rgba[1] = 127.0;
			rgba[2] = 127.0;
			rgba[3] = 255.0;
		}
	}
	//ÉÏÉ«(Öð¸öÃæÉÏÉ«¶ø²»ÊÇÖðµãÉÏÉ«)
	//先声明一个存储法线的数组,和顶点一一对应
	vector<vector<double>> PntNormals(invtxPos.size(), vector<double>(3, 0));
	for (int i = 0; i < inface_vertindex.size(); i++)
	{
		//计算面法线
		int vtxIndex0 = inface_vertindex[i][0];
		int vtxIndex1 = inface_vertindex[i][1];
		int vtxIndex2 = inface_vertindex[i][2];		//取出前三个点来计算面法线
		vector<double> vtx0_differ = inPoint_diffs[vtxIndex0 - 1];
		vector<double> vtx1_differ = inPoint_diffs[vtxIndex1 - 1];		//三角形三个顶点的三维空间坐标(注意读取的文件中索引是从1开始的)
		vector<double> vtx2_differ = inPoint_diffs[vtxIndex2 - 1];
		vector<double> vtx0 = invtxPos[vtxIndex0 - 1];
		vector<double> vtx1 = invtxPos[vtxIndex1 - 1];
		vector<double> vtx2 = invtxPos[vtxIndex2 - 1];
		vector<double> newvtx0 = { vtx0[0] + vtx0_differ[0],vtx0[1] + vtx0_differ[1], vtx0[2] + vtx0_differ[2] };
		vector<double> newvtx1 = { vtx1[0] + vtx1_differ[0],vtx1[1] + vtx1_differ[1], vtx1[2] + vtx1_differ[2] };
		vector<double> newvtx2 = { vtx2[0] + vtx2_differ[0],vtx2[1] + vtx2_differ[1], vtx2[2] + vtx2_differ[2] };
		vector<double> side1 = { newvtx1[0] - newvtx0[0],newvtx1[1] - newvtx0[1],newvtx1[2] - newvtx0[2] };
		vector<double> side2 = { newvtx2[0] - newvtx0[0],newvtx2[1] - newvtx0[1],newvtx2[2] - newvtx0[2] };
		vector <double> faceN = Normalize(CrossProduct(side1, side2));		//向量叉乘算出面法线
		for (int index = 0; index < inface_vertindex[i].size(); index++)		//由面提供的法线给此面的每一个顶点法线累加最后就得到法线列表
		{
			int tempVtxIndex = inface_vertindex[i][index] - 1;
			PntNormals[tempVtxIndex][0] += faceN[0];
			PntNormals[tempVtxIndex][1] += faceN[1];
			PntNormals[tempVtxIndex][2] += faceN[2];
		}
	}
	//WriteToFile("vtxNormal.txt", PntNormals);
	for (int i = 0; i < inface_vertindex.size(); i++)
	{
		//多边形都转换成三角形计算
		for (int j = 0; j < inface_vertindex[i].size() - 2; j++)	//顶点数-2=三角形面数
		{
			//ÏÈËã³öÈý¸ö¶¥µãµÄÆÁÄ»×ø±êºÍ²îÖµÐÅÏ¢
			//Èý½ÇÃæµÄ¶¥µãË÷Òý
			int vtxIndex0 = inface_vertindex[i][0];
			int vtxIndex1 = inface_vertindex[i][j + 1];
			int vtxIndex2 = inface_vertindex[i][j + 2];
			//注意这里面的diffs是按照点序排列的
			vector<double> vtx0_differ = inPoint_diffs[vtxIndex0 - 1];
			vector<double> vtx1_differ = inPoint_diffs[vtxIndex1 - 1];
			vector<double> vtx2_differ = inPoint_diffs[vtxIndex2 - 1];
			//顶点的法线
			vector<double> vtx0_N = Normalize(PntNormals[vtxIndex0 - 1]);
			vector<double> vtx1_N = Normalize(PntNormals[vtxIndex1 - 1]);
			vector<double> vtx2_N = Normalize(PntNormals[vtxIndex2 - 1]);
			//查找UV索引
			int uvIndex0 = inface_uvindex[i][0];
			int uvIndex1 = inface_uvindex[i][j + 1];
			int uvIndex2 = inface_uvindex[i][j + 2];
			//三角形三个顶点的UV空间坐标
			vector<double> uv0 = inuvs[uvIndex0-1];
			vector<double> uv1 = inuvs[uvIndex1-1];
			vector<double> uv2 = inuvs[uvIndex2-1];
			//三角形三个顶点的像素空间坐标
			vector<int> pixel0 = GetPixelCoord(mat.rows, mat.cols, uv0);
			vector<int> pixel1 = GetPixelCoord(mat.rows, mat.cols, uv1);
			vector<int> pixel2 = GetPixelCoord(mat.rows, mat.cols, uv2);
			
			vector<int> bbox = GetTrianglePixelBoundingBox(pixel0, pixel1, pixel2);
			
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
						Vec4f& bgra = mat.at<Vec4f>(x, y);
						bgra[2] = vtx0_differ[0] * pixelRatio[0] + vtx1_differ[0] * pixelRatio[1] + vtx2_differ[0] * pixelRatio[2];
						bgra[0] = vtx0_differ[1] * pixelRatio[0] + vtx1_differ[1] * pixelRatio[1] + vtx2_differ[1] * pixelRatio[2];
						bgra[1] = vtx0_differ[2] * pixelRatio[0] + vtx1_differ[2] * pixelRatio[1] + vtx2_differ[2] * pixelRatio[2];
						Vec3f& N = matN.at<Vec3f>(x, y);
						N[2] = 100*(vtx0_N[0] * pixelRatio[0] + vtx1_N[0] * pixelRatio[1] + vtx2_N[0] * pixelRatio[2] + 1);//法线取值范围+1映射到[0-2]
						N[0] = 100*(vtx0_N[1] * pixelRatio[0] + vtx1_N[1] * pixelRatio[1] + vtx2_N[1] * pixelRatio[2] + 1);
						N[1] = 100*(vtx0_N[2] * pixelRatio[0] + vtx1_N[2] * pixelRatio[1] + vtx2_N[2] * pixelRatio[2] + 1);
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

void CreateTextTo(vector<vector<double>> inPoint_diffs, vector<vector<double>> invtxs, vector<vector<double>> inuvs, vector<vector<int>> inface_vertindex, vector<vector<int>> inface_uvindex, int insize, string infilename, string infilenameN)
{
	Mat mat(insize, insize, CV_32FC4);
	Mat matN(insize, insize, CV_32FC3);
	createAlphaMat(inPoint_diffs, invtxs, inuvs, inface_vertindex, inface_uvindex, mat, matN);
	//createAlphaMat(inPoint_diffs, mat);
	vector<int>compression_params;
	compression_params.push_back(IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);
	try
	{
		imwrite(infilename, mat, compression_params);
		imwrite(infilenameN, matN, compression_params);
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

vector<double> CrossProduct(vector<double>invector1, vector<double> invector2)
{
	return { invector1[1] * invector2[2] - invector2[1] * invector1[2], invector2[0] * invector1[2] - invector1[0] * invector2[2], invector1[0] * invector2[1] - invector2[0] * invector1[1] };
}

vector <double> Normalize(vector<double> inVector)
{
	double vectorLen = sqrt(pow(inVector[0], 2) + pow(inVector[1], 2) + pow(inVector[2], 2));
	return { inVector[0] / vectorLen, inVector[1] / vectorLen, inVector[2] / vectorLen };
}

void WriteToFile(string outfilepath, vector<vector<double>> outcontent)
{
	ofstream destFile(outfilepath, ios::out);
	if (!destFile) { cout << "error opening destination file." << endl; }
	for (int it = 0; it < outcontent.size(); it++)
	{
		destFile << outcontent[it][0] << " " << outcontent[it][1] << " " << outcontent[it][2] << endl;
	}
	destFile.close();
}

