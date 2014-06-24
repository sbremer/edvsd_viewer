#include "edvsd_visualizer.h"

#include "iostream"
using namespace std;

EDVSD_Visualizer::EDVSD_Visualizer(QWidget *p_parent, int p_size_x, int p_size_y) :
	QWidget(p_parent)
{
	m_parent = p_parent;
	m_size_x = p_size_x;
	m_size_y = p_size_y;
	m_scaler = 1.0;
	m_mode = EDVS_Visualization_Mode_Green_Red;
	m_fade = 30;

	m_paused = false;

	this->resize((int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler));

	m_image = new QImage(m_size_x, m_size_y, QImage::Format_RGB32);

	m_debug_pixmap = new QPixmap(m_size_x, m_size_y);
	m_debug_pixmap->fill(Qt::transparent);
	m_debug_painter = new QPainter(m_debug_pixmap);
	m_debug_painter->scale(m_scaler, m_scaler);
	QPen pen;
	QColor color(Qt::blue);
	pen.setColor(color);
	pen.setWidthF(1.0/m_scaler);
	m_debug_painter->setPen(pen);
	m_debug_painter->setCompositionMode(QPainter::CompositionMode_Source);

	m_image->fill(m_colors[m_mode][2]);

	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(displayFrame()));
}

EDVSD_Visualizer::~EDVSD_Visualizer()
{
	delete m_image;
	delete m_timer;
	m_debug_painter->end();
	delete m_debug_painter;
	delete m_debug_pixmap;
}

// 0xffRRGGBB
const quint32 EDVSD_Visualizer::m_colors[2][3] = {{0xFF00FF00, 0xFFFF0000, 0xFF000000}, {0xFFFFFFFF, 0xFF000000, 0xFF808080}};

void EDVSD_Visualizer::start()
{
	m_timer->start(33);
}

void EDVSD_Visualizer::setPaused(bool p_paused)
{
	m_paused = p_paused;

	if(m_paused)
		m_timer->stop();
	else
		m_timer->start(33);

	update();
}

void EDVSD_Visualizer::setScaler(double p_scaler)
{
	m_scaler = p_scaler;
	this->resize((int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler));

	m_debug_painter->end();
	delete m_debug_pixmap;
	m_debug_pixmap = new QPixmap((int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler));
	m_debug_pixmap->fill(Qt::transparent);
	m_debug_painter->begin(m_debug_pixmap);
	m_debug_painter->scale(m_scaler, m_scaler);
	QPen pen;
	QColor color(Qt::blue);
	pen.setColor(color);
	pen.setWidthF(1.0/m_scaler);
	m_debug_painter->setPen(pen);
	m_debug_painter->setCompositionMode(QPainter::CompositionMode_Source);
}

void EDVSD_Visualizer::setMode(EDVS_Visualization_Mode p_mode)
{
	m_mode = p_mode;
	m_image->fill(m_colors[m_mode][2]);
}

QPainter* EDVSD_Visualizer::getDebugPainter()
{
	return m_debug_painter;
}

void EDVSD_Visualizer::drawEvents(EDVS_Event *p_buffer, int p_n)
{
	quint32 *data = (quint32*)(m_image->bits());
	EDVS_Event event;
	for(int a=0; a<p_n; a++){

		event = p_buffer[a];
		//cout << "TS: " << event.t << endl;
		//cout.flush();

		*(data+event.y*m_size_x+event.x) = m_colors[(int)m_mode][event.p];
	}
}

void EDVSD_Visualizer::displayFrame()
{
	update();
}

void EDVSD_Visualizer::paintEvent(QPaintEvent *p_paintevent)
{
	QPainter painter (this);
	QPixmap buffer = QPixmap::fromImage(*m_image, Qt::ThresholdDither);
	painter.drawPixmap(0, 0, (int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler), buffer, 0, 0, m_size_x, m_size_y);
	painter.drawPixmap(0, 0, *m_debug_pixmap);

	if(!m_paused)
		fadeImage();

//	m_debug_painter->end();
//	m_debug_pixmap->fill(Qt::transparent);
//	m_debug_painter->begin(m_debug_pixmap);
//	QPen pen;
//	QColor color(Qt::blue);
//	pen.setColor(color);
//	pen.setWidth(1);
//	m_debug_painter->setPen(pen);
//	m_debug_painter->scale(m_scaler, m_scaler);

	p_paintevent->accept();
	if(!m_paused)
		emit loadEventData();
}

void EDVSD_Visualizer::fadeImage()
{
	//Pixel Access:
	//R: channel[2];
	//G: channel[1];
	//B: channel[0];
	quint32 *data = (quint32*)(m_image->bits());
	quint8 *channel;
	int x;
	if(m_mode == EDVS_Visualization_Mode_Green_Red){
		for(int a=0;a<m_size_x*m_size_y;a++){
			channel = (quint8*)data;
			x = (int)(channel[1] - m_fade);
			channel[1] = (x>=0 ? x : 0);
			x = (int)(channel[2] - m_fade);
			channel[2] = (x>=0 ? x : 0);
			//			x = (int)(channel[3] - m_fade);
			//			channel[3] = (x>=0 ? x : 0);
			data++;
		}
	}
	else if(m_mode == EDVS_Visualization_Mode_White_Black){
		int fade = m_fade/2;
		for(int a=0;a<m_size_x*m_size_y;a++){
			channel = (quint8*)data;
			if(channel[0]<0x80){
				x = channel[0] + fade;
				channel[0] = channel[1] = channel[2] = (x<=0x80 ? x : 0x80);
			}
			else if(channel[0]>0x80){
				x = channel[0] - fade;
				channel[0] = channel[1] = channel[2] = (x>=0x80 ? x : 0x80);
			}
			data++;
		}
	}
}
