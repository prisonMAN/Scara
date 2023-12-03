#ifndef ARMVISUALIZEWIDGET_H
#define ARMVISUALIZEWIDGET_H

#include <QFrame>

namespace Ui {
class ArmVisualizeWidget;
}

class ArmVisualizeWidget : public QFrame
{
    Q_OBJECT

public:
    explicit ArmVisualizeWidget(QWidget *parent = nullptr, double l1 = 1.0, double l2 = 1.0, double l3 = 1.0);
    ~ArmVisualizeWidget();

    void setSegment1Length(double l1) { if (l1 <= 0) return; m_l1 = l1; solve(); }
    void setSegment2Length(double l2) { if (l2 <= 0) return; m_l2 = l2; solve(); }
    void setSegment3Length(double l3) { if (l3 <= 0) return; m_l3 = l3; solve(); }
    void setTargetCoord(QPointF p) { m_targetCoord = p; solve(); }
    void setLink3Yaw(double yaw) {m_yaw = yaw;}
    void setHandcoor(bool isright);

protected:
    void paintEvent(QPaintEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override { m_mousePress = true; }
    void mouseReleaseEvent(QMouseEvent* event) override { m_mousePress = false; }

private:
    void solve();

private:
    Ui::ArmVisualizeWidget *ui;

    // Arm properties
    bool handcoor; //初始为右手系，0为左手系,1为右手系
    double m_l1, m_l2, m_l3;
    double m_theta1, m_theta2, m_theta3,
           m_lasttheta1, m_lasttheta2;
    double m_yaw;
    QPointF m_targetCoord;

    // Painting prop
    double m_coord2ScreenRatio;

    // UI Interaction States
    bool m_mousePress = false;
};

#endif // ARMVISUALIZEWIDGET_H
