#include "CEngine.h"
#include <math.h>

CEngine::CEngine()
{
	m_fnCallback = NULL;
	m_pCallbackExtra = NULL;
}

bool CEngine::load(const string& in_sPath)
{
	QString path(in_sPath.data());
	return m_Image.load(path);
}

bool CEngine::loadMask(const string& in_sPath)
{
	QString path(in_sPath.data());
	return m_ImageMask.load(path);
}

bool CEngine::save(const string& in_sPath)
{
	QString path(in_sPath.data()); 
	return m_Image.save(path);
}

const QImage& CEngine::getImage() const
{
	return m_Image;
}

const QImage& CEngine::getImageMask() const
{
	return m_ImageMask;
}

double PI = asin(1.0)*2;

double Erf_1(double x)
{
	double Temp = sqrt(PI) * x / 2;
	return Temp + pow(Temp,3)/3 + pow(Temp,5)*7/30 + pow(Temp,7)*127/630;
}

double CriticalValue1(double SD, double Al, double Sigma1)
{
	double Chisl = SD * sqrt(-2*log(Al));
	double Znam = 2*sqrt(2*PI)*pow(Sigma1,2);
	return Chisl / Znam;  // 1.1 * SD / pow(Sigma1,2); //SD * sqrt(-2*log(Al)) / (2*sqrt(2*PI)*sqrt(Sigma1));*/
}

double CriticalValue2(double SD, double Al, double Sigma2)
{
	return /*1.8 * SD / pow(Sigma2,3); */  SD * sqrt(2.0) /*  * Erf_AlphaP  */ / (4 * sqrt(PI/3) * pow(Sigma2,3));
}

int round(double d)
{
  return floor(d + 0.5);
}

void CEngine::toMatr(bool in_bDelay)
{
	QVector<QRgb> vGrayscale;
	QImage imgNew(m_Image.width(), m_Image.height(), QImage::Format_RGB32);
	uint uiPercent, uiQuantum = m_Image.width() * m_Image.height() / 100;

	vGrayscale.reserve(256);
	for (uint i = 0; i < 256; ++i) {
		vGrayscale.append(qRgb(i, i, i));
	}
	imgNew.setColorTable(vGrayscale);

	int Width = m_Image.width();
	int Height = m_Image.height();
	double AlphaI = 0.0001;										// overall significance level
	double AlphaP = 1 - pow(1 - AlphaI, 1.0 / (Height * Width)); // pointwise significance level
	double Erf_AlphaP = Erf_1(AlphaP);							// efr^(-1) (AlphaP)
	double StDeviation = 2.5;								// Sn
	int MaxSigma = 16;
	int MaxSize = 6*MaxSigma - 1;							

	int** Data = new int* [Width];
	for (int i = 0; i < Width; i++)
		Data[i] = new int [Height];
	
	double** Filter_g1x = new double* [MaxSize];
	for(int i = 0; i < MaxSize; i++)
		Filter_g1x[i] = new double [MaxSize];

	double** Filter_g1y = new double* [MaxSize];
	for(int i = 0; i < MaxSize; i++)
		Filter_g1y[i] = new double [MaxSize];	

	double** Filter_g2x = new double* [MaxSize];
	for(int i = 0; i < MaxSize; i++)
		Filter_g2x[i] = new double [MaxSize];

	double** Filter_g2y = new double* [MaxSize];
	for(int i = 0; i < MaxSize; i++)
		Filter_g2y[i] = new double [MaxSize];

	double** Filter_g2xy = new double* [MaxSize];
	for(int i = 0; i < MaxSize; i++)
		Filter_g2xy[i] = new double [MaxSize];
	
	double** ResultX = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		ResultX[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			ResultX[i][j] = 0;
	}

	double** ResultY = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		ResultY[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			ResultY[i][j] = 0;
	}

	double** ResultXX = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		ResultXX[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			ResultXX[i][j] = 0;
	}

	double** ResultYY = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		ResultYY[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			ResultYY[i][j] = 0;
	}

	double** ResultXY = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		ResultXY[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			ResultXY[i][j] = 0;
	}

	double** Result = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		Result[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			Result[i][j] = 0;
	}

	double** Result2 = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		Result2[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			Result2[i][j] = 0;
	}

	double** TettaMap = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		TettaMap[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			TettaMap[i][j] = 0;
	}

	double** SigmaMap = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		SigmaMap[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			SigmaMap[i][j] = 0;
	}

	double** GradTetta = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		GradTetta[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			GradTetta[i][j] = 0;
	}

	double** SecondDerivative = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		SecondDerivative[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			SecondDerivative[i][j] = 0;
	}

	double** DefocusMap = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		DefocusMap[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			DefocusMap[i][j] = 255;
	}
	
	for (int y = 0; y < Height; y++)		
	{
		for (int x = 0; x < Width; x++)
		{
			QRgb rgb = m_Image.pixel(x, y);
			Data[x][y] = (int)(0.3*qRed(rgb)+0.59*qGreen(rgb)+0.11*qBlue(rgb));
		}
	}

	double Sum1X = 0, Sum1Y = 0, Sum2X = 0, Sum2Y = 0, Sum2XY = 0;

	// действия с сигмой
	double Sigma1 = 0.5;
	double Sigma2 = 0.5;
	double CV1, CV2;

	for (int m = 0; m < 3; m++)
	{
		int Size = ceil(6*Sigma1) - 1;
		int Med = Size/2;
		CV1 = CriticalValue1(StDeviation, AlphaP, Sigma1);
		CV2 = CriticalValue2(StDeviation, AlphaP, Sigma2);

		for (int i = -Med; i <= Med; i++)
			for (int j = -Med; j <= Med; j++)
			{
				Filter_g1x[Med+i][Med+j] = -j / (2*PI*pow(Sigma1,4)) * exp(-(i*i+j*j)/(2*pow(Sigma1,2)));
				Filter_g1y[Med+i][Med+j] = -i / (2*PI*pow(Sigma1,4)) * exp(-(i*i+j*j)/(2*pow(Sigma1,2)));

				Filter_g2x[Med+i][Med+j] = (pow((j/Sigma2),2) - 1) / (2*PI*pow(Sigma2,4)) * exp(-(i*i+j*j)/(2*pow(Sigma2,2)));
				Filter_g2y[Med+i][Med+j] = (pow((i/Sigma2),2) - 1) / (2*PI*pow(Sigma2,4)) * exp(-(i*i+j*j)/(2*pow(Sigma2,2)));
				Filter_g2xy[Med+i][Med+j] = i * j / (2*PI*pow(Sigma2,6)) * exp(-(i*i+j*j)/(2*pow(Sigma2,2)));
			}

			for (int i = Med; i <= (Width - Med) - 1; i++)
		    {
				for (int j = Med; j <= (Height  - Med) - 1; j++)
				{   
					for(int k = -Med; k <= Med; k++)
					{
						for(int l = -Med; l <= Med; l++)
						{
							Sum1X = Sum1X + Data[i+k][j+l] * Filter_g1x[Med+l][Med+k];
							Sum1Y = Sum1Y + Data[i+k][j+l] * Filter_g1y[Med+l][Med+k];
							Sum2X = Sum2X + Data[i+k][j+l] * Filter_g2x[Med+l][Med+k];
							Sum2Y = Sum2Y + Data[i+k][j+l] * Filter_g2y[Med+l][Med+k];
							Sum2XY = Sum2XY + Data[i+k][j+l] * Filter_g2xy[Med+l][Med+k];
						}
					}
					ResultX[i][j] = Sum1X;
					ResultY[i][j] = Sum1Y;
					ResultXX[i][j] = Sum2X;
					ResultYY[i][j] = Sum2Y;
					ResultXY[i][j] = Sum2XY;
		
					Sum1X = 0;
					Sum1Y = 0;
					Sum2X = 0;
					Sum2Y = 0;
					Sum2XY = 0;
				}
			}
 
			double GradientTetta;
			double GradientTetta2;
			double Tetta;
			for (int y = 0; y < Height; y++)
			{
				for (int x = 0; x < Width; x++)
				{
					GradientTetta = sqrt(ResultX[x][y]*ResultX[x][y] + ResultY[x][y]*ResultY[x][y]);
					if (ResultX[x][y] != 0)
						Tetta = atan(ResultY[x][y] / ResultX[x][y]);
					else
						Tetta = PI / 2;
					GradientTetta2 = pow(cos(Tetta),2) * ResultXX[x][y] - 
						2 * cos(Tetta) * sin(Tetta) * ResultXY[x][y] +
						pow(sin(Tetta),2) * ResultYY[x][y];

					if (GradientTetta > CV1 && Result[x][y] == 0)
					{
						Result[x][y] = Sigma1;
					}
					if (abs(GradientTetta2) > CV2 && Result2[x][y] == 0)
					{
						Result2[x][y] = Sigma2;
					}
					if (GradTetta[x][y] == 0)
					{
						GradTetta[x][y] = GradientTetta;
						TettaMap[x][y] = Tetta;
					}
					if (SecondDerivative[x][y] == 0)
					{
						SecondDerivative[x][y] = GradientTetta2;
					}
				}
			}
			Sigma1 = Sigma1 * 2;
			Sigma2 = Sigma2 * 2;

			//uiPercent = m * Width * 1.0/7;
			if (uiPercent % uiQuantum == 0 && m_fnCallback)
			m_fnCallback(uiPercent / uiQuantum, m_pCallbackExtra);
	}
	
	double R1_Approx, R2_Approx, G1_Approx, G2_Approx;
	double Alpha, Beta;
	//double Distance;
	//int dx, dy;
	//int MaxBlur = 4;

	for (int y = 0; y < Height; y++)
		{
			for (int x = 0; x < Width; x++)
			{	
					int h = 1;
					if (GradTetta[x][y] > CriticalValue1(StDeviation, AlphaP, Result[x][y])
							&& SecondDerivative[x][y] > CriticalValue2(StDeviation, AlphaP, Result2[x][y]))
					{					
						// по напр. градиента * в противоп.напр < 0 ?
						if (abs(TettaMap[x][y]) == PI/2)
						{		
							G1_Approx = GradTetta[x][y+1];
							G2_Approx = GradTetta[x][y-1];

							R1_Approx = SecondDerivative[x][y+1];
							R2_Approx = SecondDerivative[x][y-1];
						}
						else
							if (abs(tan(TettaMap[x][y])) > 1)
							{
								Alpha = 1.0 / tan(TettaMap[x][y]);
									
								if (Alpha > 0)
								{	
									G1_Approx = Alpha*GradTetta[x-1][y+1] + (1-Alpha)*GradTetta[x][y+1];
									G2_Approx = Alpha*GradTetta[x+1][y-1] + (1-Alpha)*GradTetta[x][y-1];

									R1_Approx = Alpha*SecondDerivative[x-1][y+1] + (1-Alpha)*SecondDerivative[x][y+1];
									R2_Approx = Alpha*SecondDerivative[x+1][y-1] + (1-Alpha)*SecondDerivative[x][y-1];
								}
								else
								{
									G1_Approx = -Alpha*GradTetta[x+1][y+1] + (1+Alpha)*GradTetta[x][y+1];
									G2_Approx = -Alpha*GradTetta[x-1][y-1] + (1+Alpha)*GradTetta[x][y-1];

									R1_Approx = -Alpha*SecondDerivative[x+1][y+1] + (1+Alpha)*SecondDerivative[x][y+1];
									R2_Approx = -Alpha*SecondDerivative[x-1][y-1] + (1+Alpha)*SecondDerivative[x][y-1];
								}
							}							
							else
							{	
								if (abs(tan(TettaMap[x][y])) < 1)
								{
									Beta = tan(TettaMap[x][y]);
	
									if (Beta > 0)
									{	
										G1_Approx = Beta*GradTetta[x-1][y+1] + (1-Beta)*GradTetta[x-1][y];
										G2_Approx = Beta*GradTetta[x+1][y-1] + (1-Beta)*GradTetta[x+1][y];

										R1_Approx = Beta*SecondDerivative[x-1][y+1] + (1-Beta)*SecondDerivative[x-1][y];
										R2_Approx = Beta*SecondDerivative[x+1][y-1] + (1-Beta)*SecondDerivative[x+1][y];
									}
									else
									{
										G1_Approx = -Beta*GradTetta[x-1][y-1] + (1+Beta)*GradTetta[x-1][y];
										G2_Approx = -Beta*GradTetta[x+1][y+1] + (1+Beta)*GradTetta[x+1][y];

										R1_Approx = -Beta*SecondDerivative[x-1][y-1] + (1+Beta)*SecondDerivative[x-1][y];
										R2_Approx = -Beta*SecondDerivative[x+1][y+1] + (1+Beta)*SecondDerivative[x+1][y];
									}
								}
								else
								{
									if (tan(TettaMap[x][y]) == 1)
									{
										G1_Approx = GradTetta[x-1][y+1];
										G2_Approx = GradTetta[x+1][y-1];

										R1_Approx = SecondDerivative[x-1][y+1];
										R2_Approx = SecondDerivative[x+1][y-1];										
									}
									else
										if (tan(TettaMap[x][y]) == -1)
										{
											G1_Approx = GradTetta[x-1][y-1];
											G2_Approx = GradTetta[x+1][y+1];

											R1_Approx = SecondDerivative[x-1][y-1];
											R2_Approx = SecondDerivative[x+1][y+1];
										}
										else
										{
											G1_Approx = 0;
											G2_Approx = 0;

											R1_Approx = 0;
											R2_Approx = 0;
										}
								}
							}
					}
					else
					{
						G1_Approx = 0;
						G2_Approx = 0;

						R1_Approx = 0;	
						R2_Approx = 0;
					}

					if (G1_Approx > 0 && R1_Approx < 0 ||
							G2_Approx > 0 && R2_Approx < 0)
					{
						imgNew.setPixel(x, y, qRgb(255, 255, 255));
					}
					else
						imgNew.setPixel(x, y, qRgb(0, 0, 0));
		}
	}

	m_Image = imgNew;
	if (m_fnCallback) m_fnCallback(100, m_pCallbackExtra);
}

int CountPixelsInCOF(int Radius)
{
	int N = 1;

	if (Radius > 0)
	{
		for (int i = -Radius; i <= Radius; i++)
		for (int j = -Radius; j <= Radius; j++)
			if (i*i + j*j <= Radius*Radius)
				N++;
	}
	//else
		//N = 1;

	return N;
}

int **OldRed, **OldGreen, **OldBlue, **DepthMap, *Pixels;

void CEngine::toBlur(int BigRadius, int Threshold, int Boost, bool in_bDelay)
{
	QVector<QRgb> vGrayscale;
	QImage imgNew(m_Image.width(), m_Image.height(), QImage::Format_RGB32);
	uint uiPercent, uiQuantum = m_Image.width() * m_Image.height() / 100;

	vGrayscale.reserve(256);
	for (uint i = 0; i < 256; ++i) {
		vGrayscale.append(qRgb(i, i, i));
	}
	imgNew.setColorTable(vGrayscale);

	int Width = m_Image.width();
	int Height = m_Image.height();

	int WidthMask = m_ImageMask.width();
	int HeightMask = m_ImageMask.height();

	int MaxRadius = 30;

	Pixels = new int [BigRadius + 1];

	for (int i = 0; i <= BigRadius; i++)
	{
		Pixels[i] = CountPixelsInCOF(i);
		int N = Pixels[i];	
	}

	DepthMap = new int* [Width + 2*MaxRadius];
	for(int i = 0; i < Width + 2*MaxRadius; i++)
	{
		DepthMap[i] = new int [Height + 2*MaxRadius];
		for(int j = 0; j < Height + 2*MaxRadius; j++)
		{
			/*
			//if (j < 9 * Height / 10)
				DepthMap[i][j] = 100;
			if (j < 8 * Height / 10)
				DepthMap[i][j] = 90;
			if (j < 7 * Height / 10)
				DepthMap[i][j] = 80;
			if (j < 6 * Height / 10)
				DepthMap[i][j] = 70;
			if (j < 5 * Height / 10)
				DepthMap[i][j] = 50;
			if (j < 4 * Height / 10)
				DepthMap[i][j] = 40;
			if (j < 3 * Height / 10)
				DepthMap[i][j] = 30;
			if (j < 2 * Height / 10)
				DepthMap[i][j] = 20;
			if (j < 1 * Height / 10)
				DepthMap[i][j] = 15;*/

			if (i >= MaxRadius && j >= MaxRadius && i < Width+MaxRadius && j < Height+MaxRadius)
			{
				QRgb rgb = m_ImageMask.pixel(i - MaxRadius, j - MaxRadius);	
				DepthMap[i][j] = qRed(rgb) * 100 / 255;
			}
			else
				DepthMap[i][j] = 100;
		}
	}

	OldRed = new int* [Width + 2*MaxRadius];
	for(int i = 0; i < Width + 2*MaxRadius; i++)
	{
		OldRed[i] = new int [Height + 2*MaxRadius];
		for(int j = 0; j < Height + 2*MaxRadius; j++)
			OldRed[i][j] = 0;
	}

	OldGreen = new int* [Width + 2*MaxRadius];
	for(int i = 0; i < Width + 2*MaxRadius; i++)
	{
		OldGreen[i] = new int [Height + 2*MaxRadius];
		for(int j = 0; j < Height + 2*MaxRadius; j++)
			OldGreen[i][j] = 0;
	}

	OldBlue = new int* [Width + 2*MaxRadius];
	for(int i = 0; i < Width + 2*MaxRadius; i++)
	{
		OldBlue[i] = new int [Height + 2*MaxRadius];
		for(int j = 0; j < Height + 2*MaxRadius; j++)
			OldBlue[i][j] = 0;
	}

	double** NewRed = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		NewRed[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			NewRed[i][j] = 0;
	}

	double** NewGreen = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		NewGreen[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			NewGreen[i][j] = 0;
	}

	double** NewBlue = new double* [Width];
	for(int i = 0; i < Width; i++)
	{
		NewBlue[i] = new double [Height];
		for(int j = 0; j < Height; j++)
			NewBlue[i][j] = 0;
	}
	
	//int Radius = 20;
	int Radius = BigRadius;
	int N = CountPixelsInCOF(Radius);
	double DeltaRed, DeltaBlue, DeltaGreen;
	
	// дублировать края картинки, чтобы крайние пиксели не были темными
	QRgb rgb1 = m_Image.pixel(0, 0);
	QRgb rgb2 = m_Image.pixel(0, Height - 1);
	QRgb rgb4 = m_Image.pixel(Width - 1, 0);
	QRgb rgb5 = m_Image.pixel(Width - 1, Height - 1);

	for (int y = -MaxRadius; y < Height + MaxRadius; y++)
	{
		for (int x = -MaxRadius; x < Width + MaxRadius; x++)
		{			
			if (x >= 0 && y >= 0 && x < Width && y < Height)
			{
				QRgb rgb = m_Image.pixel(x, y);	
					
				OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb);
				OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb);
				OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb);
					
				NewRed[x][y] = qRed(rgb);
				NewGreen[x][y] = qGreen(rgb);
				NewBlue[x][y] = qBlue(rgb);
			}
			else
			{
				if (x < 0)
				{
					if (y < 0)
					{
						OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb1);
						OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb1);
						OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb1);

						DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[MaxRadius][MaxRadius];
					}
					else
					{
						if (y >= Height)
						{
							OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb2);
							OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb2);
							OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb2);

							DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[MaxRadius][Height+MaxRadius-1];
						}
						else
						{
							QRgb rgb3 = m_Image.pixel(0, y);

							OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb3);
							OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb3);
							OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb3);

							DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[MaxRadius][y+MaxRadius];
						}
					}
				}
				else
				{
					if (x >= Width)
					{
						if (y < 0)
						{	
							OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb4);
							OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb4);
							OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb4);

							DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[Width+MaxRadius-1][MaxRadius];
						}
						else
						{
							if (y >= Height)
							{
								OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb5);
								OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb5);
								OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb5);

								DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[Width+MaxRadius-1][Height+MaxRadius-1];
							}
							else
							{
								QRgb rgb6 = m_Image.pixel(Width - 1, y);
									
								OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb6);
								OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb6);
								OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb6);

								DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[Width+MaxRadius-1][y+MaxRadius];
							}
						}
					}
					else
					{
						if (y < 0)
						{
							QRgb rgb7 = m_Image.pixel(x, 0);
	
							OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb7);
							OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb7);
							OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb7);

							DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[x+MaxRadius][MaxRadius];
						}
						else
						{
							QRgb rgb8 = m_Image.pixel(x, Height - 1);
	
							OldRed[x+MaxRadius][y+MaxRadius] = qRed(rgb8);
							OldGreen[x+MaxRadius][y+MaxRadius] = qGreen(rgb8);
							OldBlue[x+MaxRadius][y+MaxRadius] = qBlue(rgb8);

							DepthMap[x+MaxRadius][y+MaxRadius] = DepthMap[x+MaxRadius][Height+MaxRadius-1];
						}
					}
				}
			}
		}
	}

	double Brighten = (double)(Boost + 100) / 100;

	// собственно blur
	for (int y = -MaxRadius; y < Height + MaxRadius; y++)
	{
		for (int x = -MaxRadius; x < Width + MaxRadius; x++)
		{	
			//Radius = DepthMap[x+MaxRadius][y+MaxRadius] / BigRadius;
			//N = CountPixelsInCOF(Radius);
			Radius = DepthMap[x+MaxRadius][y+MaxRadius] * BigRadius / 100;
			N = Pixels[Radius];

			//if (DepthMap[x+MaxRadius][y+MaxRadius] == 10)
				//N = N;			

			if (N > 1)
			{
				/*if (Boost > 0
					&& 0.3 * OldRed[x+MaxRadius][y+MaxRadius] + 0.59 * OldGreen[x+MaxRadius][y+MaxRadius]
					+ 0.11 * OldBlue[x+MaxRadius][y+MaxRadius] > Threshold)
				{
					DeltaRed = (double)(OldRed[x+MaxRadius][y+MaxRadius] * Brighten) / N;
					DeltaGreen = (double)(OldGreen[x+MaxRadius][y+MaxRadius] * Brighten) / N;
					DeltaBlue = (double)(OldBlue[x+MaxRadius][y+MaxRadius] * Brighten) / N;
				}
				else
				{*/
					DeltaRed = (double)OldRed[x+MaxRadius][y+MaxRadius] / N;
					DeltaGreen = (double)OldGreen[x+MaxRadius][y+MaxRadius] / N;
					DeltaBlue = (double)OldBlue[x+MaxRadius][y+MaxRadius] / N;
				//}

				for (int i = -Radius; i <= Radius; i++)
				{
					for (int j = -Radius; j <= Radius; j++)
					{						
						if (i*i + j*j <= Radius*Radius
							&& x+i >= 0 && y+j >= 0 && x+i < Width && y+j < Height)
						{
							if (DepthMap[x+MaxRadius][y+MaxRadius] == DepthMap[x+i+MaxRadius][y+j+MaxRadius])
							{
								NewRed[x+i][y+j] += DeltaRed;
								NewGreen[x+i][y+j] += DeltaGreen;
								NewBlue[x+i][y+j] += DeltaBlue;
							}
							else
							{
								if (x >= 0 && y >= 0 && x < Width && y < Height)
								{
									if (abs(DepthMap[x+MaxRadius][y+MaxRadius] - DepthMap[x+i+MaxRadius][y+j+MaxRadius]) < 70)
									{
										NewRed[x][y] += (double)OldRed[x+i+MaxRadius][y+j+MaxRadius] / N; //DeltaRed;
										NewGreen[x][y] += (double)OldGreen[x+i+MaxRadius][y+j+MaxRadius] / N; //DeltaGreen;
										NewBlue[x][y] += (double)OldBlue[x+i+MaxRadius][y+j+MaxRadius] / N; //DeltaBlue;
									}
									else
									{
										NewRed[x][y] += ((double)OldRed[x+i+MaxRadius][y+j+MaxRadius] / N + DeltaRed) / 2;
										NewGreen[x][y] += ((double)OldGreen[x+i+MaxRadius][y+j+MaxRadius] / N + DeltaGreen) / 2;
										NewBlue[x][y] += ((double)OldBlue[x+i+MaxRadius][y+j+MaxRadius] / N + DeltaBlue) / 2;
									}

									//NewRed[x+i][y+j] += DeltaRed / N;
									//NewGreen[x+i][y+j] += DeltaGreen / N;
									//NewBlue[x+i][y+j] += DeltaBlue / N;
								}
							}
						}
					}
				}
			}

			if (x >= 0 && y >= 0 && x < Width && y < Height)
			{
				NewRed[x][y] -= (N - 1) * (DeltaRed / Brighten);
				NewGreen[x][y] -= (N - 1) * (DeltaGreen / Brighten);
				NewBlue[x][y] -= (N - 1) * (DeltaBlue / Brighten);
										
				uiPercent = y * m_Image.width() + x;
				if (uiPercent % uiQuantum == 0 && m_fnCallback)
					m_fnCallback(uiPercent / uiQuantum, m_pCallbackExtra);
			}
		}
	}

	// записать в картинку то что получилось
	for (int y = 0; y < Height; y++)
	{
		for (int x = 0; x < Width; x++)
		{
			if (NewRed[x][y] < 0)
				NewRed[x][y] = 0;
			if (NewGreen[x][y] < 0)
				NewGreen[x][y] = 0;
			if (NewBlue[x][y] < 0)
				NewBlue[x][y] = 0;
			if (NewRed[x][y] > 255)
				NewRed[x][y] = 255;
			if (NewGreen[x][y] > 255)
				NewGreen[x][y] = 255;
			if (NewBlue[x][y] > 255)
				NewBlue[x][y] = 255;

			imgNew.setPixel(x, y, qRgb((int)NewRed[x][y], (int)NewGreen[x][y], (int)NewBlue[x][y]));
		}
	}

	m_Image = imgNew;
	if (m_fnCallback) m_fnCallback(100, m_pCallbackExtra);
}

void CEngine::setCallbacks(PCALLBACK in_fnCallback, void *in_pCallbackExtra)
{
	m_fnCallback = in_fnCallback;
	m_pCallbackExtra = in_pCallbackExtra;
}