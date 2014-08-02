#pragma once
#ifndef _CENGINE_H
#define _CENGINE_H

#include <string>
using namespace std;
#include <QImage>
#include <QVector>
#include <Windows.h>

typedef void (*PCALLBACK)(uint percent, void *extra);

class CEngine
{
	QImage m_Image;
	QImage m_ImageMask;
	PCALLBACK m_fnCallback;
	void *m_pCallbackExtra;

public:
	CEngine();

	void setCallbacks(PCALLBACK in_fnCallback, void *in_pCallbackExtra);

	bool load(const string& in_sPath);
	bool save(const string& in_sPath);
	bool loadMask(const string& in_sPath);

	const QImage& getImage() const;
	const QImage& getImageMask() const;

	void toGrayscale(bool in_bDelay = true);
	void toMatr(bool in_bDelay = true);
	void toBlur(int Radius, int Threshold, int Boost, bool in_bDelay = true);
	// ... other filters
};

#endif //_CENGINE_H
