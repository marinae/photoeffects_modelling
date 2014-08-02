#ifndef GUI_H
#define GUI_H

#include <string>
#include <vector>
#include <fstream>
using namespace std;
#include <QtGui/QMainWindow>
#include "ui_gui.h"
#include <QDesktopWidget>
#include <QTextCodec>
#include <QMessageBox>
#include <QFileDialog>
#include <QClipboard>
#include <QTimer>
#include <QThread>
#include <QDesktopServices>

#include "../Engine/CEngine.h"

#ifndef uint
typedef unsigned int uint;
#endif //uint

class QBlurThread;
class QMatrThread;

class QBlurThread : public QThread
{
	Q_OBJECT

	CEngine& m_lEngine;

	int Radius;
	int Threshold;
	int Boost;

protected:
	void run();

public:
	QBlurThread(CEngine& in_lEngine) : m_lEngine(in_lEngine) 
	{
		//Radius = Rad;
	};

public:
	void setRadius(int Rad)
	{
		Radius = Rad;
	}

	void setThreshold(int Thres)
	{
		Threshold = Thres;
	}

	void setBoost(int Boos)
	{
		Boost = Boos;
	}
};

class QMatrThread : public QThread
{
	Q_OBJECT

	CEngine& m_lEngine;

protected:
	void run();

public:
	QMatrThread(CEngine& in_lEngine) : m_lEngine(in_lEngine) {};
};

class  QProgressEvent : public QEvent
{
public:
	uint percent;
	QProgressEvent() : QEvent((QEvent::Type)(QEvent::User + 1)) {};
};

class GUI : public QMainWindow
{
	Q_OBJECT

public:
	GUI(QWidget *parent = 0, Qt::WFlags flags = 0);
	~GUI();

private slots:
	void onOpen();
	void onSave();
	void onSaveAs();
	void onLoadMask();
	void onBlur();
	void onMatr();

protected:
	bool event(QEvent *event);

private:
	Ui::GUIClass ui;
	CEngine m_Engine;
	string m_sPath;
	string m_sPathMask;
	QGraphicsScene *m_pCurrentScene;
	QGraphicsScene *m_pCurrentMask;
	QBlurThread m_BlurThread;
	QMatrThread m_MatrThread;

	void onInit();
	void updateImage();
	void updateImageMask();
	void enableControls(bool in_bEnable);

	static void callback(uint percent, GUI* pThis);
};

#endif // GUI_H
