#include "edvsd_visualizer.h"

EDVSD_Visualizer::EDVSD_Visualizer(QWidget *p_parent, int p_size_x, int p_size_y) :
	QWidget(p_parent)
{
	m_parent = p_parent;
	m_size_x = p_size_x;
	m_size_y = p_size_y;
	m_scaler = 1.0;
	m_mode = EDVS_Visualization_Mode_Green_Red;
	m_fade = 60;

	this->resize((int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler));

	m_image = new QImage(m_size_x, m_size_y, QImage::Format_RGB32);

	m_image->fill(m_colors[m_mode][2]);

	m_timer = new QTimer();
	connect(m_timer, SIGNAL(timeout()), this, SLOT(displayFrame()));
	m_timer->start(33);
}

EDVSD_Visualizer::~EDVSD_Visualizer()
{
	delete m_image;
	delete m_timer;
}

const quint32 EDVSD_Visualizer::m_colors[2][3] = {{0xFFFF0000, 0xFF00FF00, 0xFFFFFFFF}, {0xFFFFFFFF, 0xFF000000, 0xFF808080}};

void EDVSD_Visualizer::setScaler(double p_scaler)
{
	m_scaler = p_scaler;
	this->resize((int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler));
}

void EDVSD_Visualizer::setMode(EDVS_Visualization_Mode p_mode)
{
	m_mode = p_mode;
	m_image->fill(m_colors[m_mode][2]);
}

void EDVSD_Visualizer::drawEvents(EDVS_Event *p_buffer, int p_n)
{
	quint32 *data = (quint32*)(m_image->bits());
	EDVS_Event event;
	for(int a=0; a<p_n;a++){
		event = p_buffer[a];
		*(data+event.y*m_size_x+event.x) = m_colors[(int)m_mode][event.p];
	}
}

void EDVSD_Visualizer::displayFrame()
{
	this->update();
}

void EDVSD_Visualizer::paintEvent(QPaintEvent *p_paintevent)
{
	fadeImage();
	m_painter.begin(m_parent);
	QPixmap buffer = QPixmap::fromImage(*m_image, Qt::ThresholdDither);
	m_painter.drawPixmap(0, 0, (int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler), buffer, 0, 0, m_size_x, m_size_y);
	m_painter.end();
	emit loadEventData();
	p_paintevent->accept();
}

void EDVSD_Visualizer::fadeImage()
{
	quint32 *data = (quint32*)(m_image->bits());
	quint8 *channel;
	int x;
	if(m_mode == EDVS_Visualization_Mode_Green_Red){
		for(int a=0;a<m_size_x*m_size_y;a++){
			channel = (quint8*)data;
			x = channel[1] - m_fade;
			channel[1] = (x>=0 ? x : 0);
			x = channel[2] - m_fade;
			channel[2] = (x>=0 ? x : 0);
//			x = channel[3] - m_fade;
//			channel[3] = (x>=0 ? x : 0);
			data++;
		}
	}
	else if(m_mode == EDVS_Visualization_Mode_White_Black){
		for(int a=0;a<m_size_x*m_size_y;a++){
			channel = (quint8*)data;
			if(channel[1]<0x80){
				x = channel[1] + m_fade;
				channel[1] = (x<=0x80 ? x : 0x80);
			}
			else{
				x = channel[1] - m_fade;
				channel[1] = (x>=0x80 ? x : 0x80);
			}
			if(channel[2]<0x80){
				x = channel[2] + m_fade;
				channel[2] = (x<=0x80 ? x : 0x80);
			}
			else{
				x = channel[2] - m_fade;
				channel[2] = (x>=0x80 ? x : 0x80);
			}
			if(channel[3]<0x80){
				x = channel[3] + m_fade;
				channel[3] = (x<=0x80 ? x : 0x80);
			}
			else{
				x = channel[3] - m_fade;
				channel[3] = (x>=0x80 ? x : 0x80);
			}

			data++;
		}
	}
}
