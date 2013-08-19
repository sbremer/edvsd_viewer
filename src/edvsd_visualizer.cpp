#include "edvsd_visualizer.h"

EDVSD_Visualizer::EDVSD_Visualizer(QWidget *p_parent, int p_size_x, int p_size_y) :
    QWidget(p_parent)
{
    m_parent = p_parent;
    m_size_x = p_size_x;
    m_size_y = p_size_y;
    m_scaler = 1.0;

    this->resize((int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler));

    m_image = new QImage(m_size_x, m_size_y, QImage::Format_RGB32);
}

EDVSD_Visualizer::~EDVSD_Visualizer()
{
    delete m_image;
}

void EDVSD_Visualizer::setScaler(double p_scaler)
{
    m_scaler = p_scaler;
    this->resize((int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler));
}

void EDVSD_Visualizer::paintEvent(QPaintEvent *p_paintevent)
{
    m_painter.begin(m_parent);
    QPixmap buffer = QPixmap::fromImage(*m_image, Qt::ThresholdDither);
    m_painter.drawPixmap(0, 0, (int)(m_size_x*m_scaler), (int)(m_size_y*m_scaler), buffer, 0, 0, m_size_x, m_size_y);
    m_painter.end();
    p_paintevent->accept();
}
