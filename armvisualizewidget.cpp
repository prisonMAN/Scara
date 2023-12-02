#include "armvisualizewidget.h"
#include "ui_armvisualizewidget.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPainter>
#include <QResizeEvent>
#include <QtMath>

static constexpr double Rad2Deg = (180.0 / M_PI);

ArmVisualizeWidget::ArmVisualizeWidget(QWidget *parent, double l1, double l2)
    : QFrame(parent)
    , ui(new Ui::ArmVisualizeWidget),
    m_l1(l1), m_l2(l2)
{
    ui->setupUi(this);

    m_theta1 = 0;
    m_theta2 = 0;
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
           l2Screen = m_l2 * m_coord2ScreenRatio;

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
        if (norm > m_l1 + m_l2) {
            m_targetCoord /= (norm / (m_l1 + m_l2));
        }

        qDebug() << "Target coord: " << m_targetCoord;
    }

    if (m_mousePress) {
        solve();
    }
}

void ArmVisualizeWidget::solve()
{
    /*
     * # 计算第二个关节角度
        D = (x**2 + y**2 - self.L1**2 - self.L2**2) / (2 * self.L1 * self.L2)
        theta2 = math.atan2(-math.sqrt(1 - D**2), D)

        # 计算第一个关节角度
        theta1 = math.atan2(y, x) - math.atan2(self.L2 * math.sin(theta2), self.L1 + self.L2 * math.cos(theta2))
     */
    auto x = m_targetCoord.x(), y = m_targetCoord.y();
    double D = ((x * x) + (y * y) - (m_l1 * m_l1) - (m_l2 * m_l2)) / (2 * m_l1 * m_l2);

    m_theta2 = std::atan2(-std::sqrt(1 - (D * D)), D);

    m_theta1 = std::atan2(y, x) - std::atan2(m_l2 * std::sin(m_theta2), m_l1 + m_l2 * std::cos(m_theta2));

    update();
}
