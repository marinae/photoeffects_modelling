#include "gui.h"

void GUI::onInit()
{
	QRect qrGeometry;
	QSize szWindow;

	//move window to screen center
	qrGeometry = frameGeometry();
	qrGeometry.moveCenter(QDesktopWidget().availableGeometry().center());
	move(qrGeometry.topLeft());

	//make window fixed-sized
	szWindow = size();
	setMaximumSize(szWindow);
	setMinimumSize(szWindow);

	// set default encoding to cp1251
	QTextCodec::setCodecForCStrings(QTextCodec::codecForName("Windows-1251"));

	m_pCurrentScene = NULL;
	m_pCurrentMask = NULL;
	m_Engine.setCallbacks((PCALLBACK)callback, this);
}

void GUI::updateImage()
{
	QPixmap pmNew = QPixmap::fromImage(m_Engine.getImage());
	QGraphicsScene *pNewScene = new QGraphicsScene;
	pNewScene->addPixmap(pmNew);
	ui.gvMain->setScene(pNewScene);
	if (m_pCurrentScene != NULL) delete m_pCurrentScene;
	m_pCurrentScene = pNewScene;
}

void GUI::updateImageMask()
{
	// ???
	QPixmap pmNew = QPixmap::fromImage(m_Engine.getImageMask());
	QGraphicsScene *pNewScene = new QGraphicsScene;
	pNewScene->addPixmap(pmNew);
	ui.gvMask->setScene(pNewScene);
	if (m_pCurrentMask != NULL) delete m_pCurrentMask;
	m_pCurrentMask = pNewScene;
}

void GUI::onOpen()
{
	string sPath;

	sPath = QFileDialog::getOpenFileName(this, "Загрузка изображения",
		QString("C:\\"), "Изображения (*.jpg;*.png;*.bmp)").toStdString();
	if (sPath.empty()) return;

	if (!m_Engine.load(sPath))
	{
		QMessageBox::critical(this, "Ошибка", "Не удаётся загрузить изображение");
		return;
	};

	updateImage();
	m_sPath = sPath;
}

void GUI::onLoadMask()
{
	string sPath;

	sPath = QFileDialog::getOpenFileName(this, "Загрузка маски",
		QString("C:\\"), "Изображения (*.jpg;*.png;*.bmp)").toStdString();
	if (sPath.empty()) return;

	if (!m_Engine.loadMask(sPath))
	{
		QMessageBox::critical(this, "Ошибка", "Не удаётся загрузить изображение");
		return;
	};

	updateImageMask();
	m_sPathMask = sPath;
}

void GUI::onSaveAs()
{
	string sPath;

	sPath = QFileDialog::getSaveFileName(this, "Загрузка изображения",
		QString("C:\\"), "Изображения (*.jpg;*.png;*.bmp)").toStdString();
	if (sPath.empty()) return;

	if (!m_Engine.save(sPath))
	{
		QMessageBox::critical(this, "Ошибка", "Не удаётся сохранить изображение");
		return;
	};

	m_sPath = sPath;
}

void GUI::onSave()
{
	if (!m_Engine.save(m_sPath))
	{
		QMessageBox::critical(this, "Ошибка", "Не удаётся сохранить изображение");
		return;
	}
}

void QBlurThread::run()
{
	m_lEngine.toBlur(Radius, Threshold, Boost);
}

void GUI::onBlur()
{
	enableControls(false);
	m_BlurThread.setRadius(ui.spinBox->value());
	m_BlurThread.setThreshold(ui.spinBox_2->value());
	m_BlurThread.setBoost(ui.spinBox_3->value());
	m_BlurThread.start();
}

void QMatrThread::run()
{
	m_lEngine.toMatr();
}

void GUI::onMatr()
{
	enableControls(false);
	m_MatrThread.start();
}

GUI::GUI(QWidget *parent, Qt::WFlags flags)
	: QMainWindow(parent, flags), m_BlurThread(m_Engine), m_MatrThread(m_Engine)
{
	ui.setupUi(this);
	onInit();

	connect(ui.btnOpen, SIGNAL(clicked()), this, SLOT(onOpen()));
	connect(ui.mnuOpen, SIGNAL(activated()), this, SLOT(onOpen()));
	connect(ui.btnSave, SIGNAL(clicked()), this, SLOT(onSave()));
	connect(ui.mnuSave, SIGNAL(activated()), this, SLOT(onSave()));
	connect(ui.btnSaveAs, SIGNAL(clicked()), this, SLOT(onSaveAs()));
	connect(ui.mnuSaveAs, SIGNAL(activated()), this, SLOT(onSaveAs()));
	connect(ui.btnLoadMask, SIGNAL(clicked()), this, SLOT(onLoadMask()));
	connect(ui.btnBlur, SIGNAL(clicked()), this, SLOT(onBlur()));	
	connect(ui.btnMatr, SIGNAL(clicked()), this, SLOT(onMatr()));
}

GUI::~GUI()
{}

void GUI::callback(uint percent, GUI* pThis)
{
	QProgressEvent *pEvent = new QProgressEvent;
	pEvent->percent = percent;
	QCoreApplication::postEvent(pThis, pEvent);
}

void GUI::enableControls(bool in_bEnable)
{
	ui.gbImage->setEnabled(in_bEnable);
	ui.gbFilters->setEnabled(in_bEnable);
	ui.menuBar->setEnabled(in_bEnable);
}

bool GUI::event(QEvent *event)
{
	if (event->type() == (QEvent::Type)(QEvent::User + 1))
	{
		// Progress event
		QProgressEvent *pEvent = (QProgressEvent*)event;
		ui.pbMain->setValue(pEvent->percent);
		if (pEvent->percent == 100)
		{
			enableControls(true);
			updateImage();
			updateImageMask();
		}
	}

	return QMainWindow::event(event);
}
