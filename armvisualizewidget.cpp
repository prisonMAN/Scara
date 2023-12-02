#include "armvisualizewidget.h"
#include "ui_armvisualizewidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QtMath>

static constexpr double Rad2Deg = (180.0 / M_PI);

ArmVisualizeWidget::ArmVisualizeWidget(QWidget *parent, double l1, double l2, double l3)
    : QFrame(parent)
    , ui(new Ui::ArmVisualizeWidget),
    handcoor(1),
    m_l1(l1), m_l2(l2), m_l3(l3)
{
    ui->setupUi(this);

    m_theta1 = 0;
    m_theta2 = 0;
    m_theta3 = 0;
    m_lasttheta1 = 0;
    m_lasttheta2 = 0;
}

ArmVisualizeWidget::~ArmVisualizeWidget()
{
    delete ui;
}

void ArmVisualizeWidget::paintEvent(QPaintEvent *event)
{
    QFrame::paintEvent(event);

    auto w = width(), h = height(), minEdge = std::min(w, h);
    double l1Screen = m_l1 * m_coord2ScreenRatio,
           l2Screen = m_l2 * m_coord2ScreenRatio,
           l3Screen = m_l3 * m_coord2ScreenRatio;

    QPainter p(this);

    p.translate(w / 2, h / 2); // Move to center

    // Range circle
    p.setPen(QPen(Qt::black, 0, Qt::DashLine));
    p.drawEllipse(QPointF(0, 0), minEdge / 2, minEdge / 2);

    // Segment 1
    p.setPen(QPen(Qt::darkRed, 5));
    p.setBrush(Qt::darkRed);
    p.drawText(0, 0, QString("%1").arg(m_theta1));
    p.rotate(m_theta1 * Rad2Deg);
    p.drawLine(QLineF(0, 0, 0, -l1Screen));

    p.translate(0, -l1Screen); // Move to segment 2 axis

    // Segment 2
    p.setPen(QPen(Qt::darkGreen, 5));
    p.setBrush(Qt::darkGreen);
    p.rotate(-m_theta1 * Rad2Deg);
    p.drawText(0, 0, QString("%2").arg(m_theta2));
    p.rotate((m_theta1 + m_theta2) * Rad2Deg);
    p.drawLine(QLineF(0, 0, 0, -l2Screen));

    p.translate(0, -l2Screen); // Move to segment 3 axis

    // Segment 3
    p.setPen(QPen(Qt::darkBlue, 5));
    p.setBrush(Qt::darkBlue);
    p.rotate(-(m_theta1 + m_theta2) * Rad2Deg);
    p.drawText(0, 0, QString("%3").arg(m_theta3));
    p.rotate((m_theta1 + m_theta2 + m_theta3) * Rad2Deg);
    p.drawLine(QLineF(0, 0, 0, -l3Screen));

    // Target point
    p.resetTransform();
    p.translate(w / 2, h / 2);
    p.setPen(QPen(Qt::red, 0));
    p.translate(m_targetCoord.x() * m_coord2ScreenRatio,
                m_targetCoord.y() * -m_coord2ScreenRatio);
    p.drawLine(-5, -5, 5, 5);
    p.drawLine(-5, 5, 5, -5);

    p.end();
}

void ArmVisualizeWidget::resizeEvent(QResizeEvent *event)
{
    auto minEdge = std::min(event->size().width(), event->size().height());
    m_coord2ScreenRatio = minEdge / 2.0 / (m_l1 + m_l2);
}

void ArmVisualizeWidget::mouseMoveEvent(QMouseEvent *event)
{
    auto mousePos = event->localPos();

    if (m_mousePress) {
        m_targetCoord = QPointF(mousePos.x() - width() / 2, height() / 2 - mousePos.y());
        m_targetCoord /= m_coord2ScreenRatio;

        // Clamp to circle
        double norm = std::sqrt(std::pow(m_targetCoord.x(), 2) +
                                std::pow(m_targetCoord.y(), 2));
        if (norm > m_l1 + m_l2 + m_l3) {
            m_targetCoord /= (norm / (m_l1 + m_l2 + m_l3));
        }

        qDebug() << "Target coord: " << m_targetCoord;
    }

    if (m_mousePress) {
        solve();
    }
}

void ArmVisualizeWidget::judgeHandcoor()
{
    if ((m_theta2 > 0.0 && m_theta2 < M_PI) || (m_theta2 > -2.0 * M_PI && m_theta2 < -M_PI)){
        handcoor = true;
    }
    else{
        handcoor = false;
    }
}

void ArmVisualizeWidget::solve()
{
    judgeHandcoor();

    auto x = m_targetCoord.x(), y = m_targetCoord.y();
    double D = ((x * x) + (y * y) - (m_l1 * m_l1) - (m_l2 * m_l2)) / (2 * m_l1 * m_l2);

    if(handcoor == true){
        m_theta2 = std::atan2(std::sqrt(1 - (D * D)), D);
    }
    else{
        m_theta2 = -std::atan2(std::sqrt(1 - (D * D)), D);
    }

    m_theta1 = std::atan2(x, y) - std::atan2(m_l2 * std::sin(m_theta2), m_l1 + m_l2 * std::cos(m_theta2));

    auto temp1 = abs((m_theta1 + 2.0 * M_PI) - m_lasttheta1);
    auto temp2 = abs((m_theta1 - 2.0 * M_PI) - m_lasttheta1);
    auto temp3 = abs(m_theta1 - m_lasttheta1);

    if(temp1 < temp2 && temp1 < temp3){
        m_theta1 = m_theta1 + 2.0 * M_PI;
    }
    else if(temp2 < temp1 && temp2 < temp3){
        m_theta1 = m_theta1 - 2.0 * M_PI;
    }

    temp1 = abs((m_theta2 + 2.0 * M_PI) - m_lasttheta2);
    temp2 = abs((m_theta2 - 2.0 * M_PI) - m_lasttheta2);
    temp3 = abs(m_theta2 - m_lasttheta2);

    if(temp1 < temp2 && temp1 < temp3){
        m_theta2 = m_theta2 + 2.0 * M_PI;
    }
    else if(temp2 < temp1 && temp2 < temp3){
        m_theta2 = m_theta2 - 2.0 * M_PI;
    }

    m_theta3 = m_yaw - m_theta1 - m_theta2;

    m_lasttheta1 = m_theta1;
    m_lasttheta2 = m_theta2;

    update();
}
