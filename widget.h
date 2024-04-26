#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QMouseEvent>
#include <QPainterPath>

QT_BEGIN_NAMESPACE
namespace Ui {
class Widget;
}
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    explicit Widget(QWidget *parent = nullptr);
    void paintEvent(QPaintEvent *) override;
    void setActiveContour(int index);
    ~Widget();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void saveContoursToFile();
    void loadContoursFromFile();
    QPainterPath path;
    QList<QPointF> points;
    QList<QList<QPointF>> contours_points;
    bool collision_points = false;
private:
    bool m_blockMousePress = false;
    bool m_blockDeletePoint = false;
    bool m_blockDeleteContour = false;
    int m_activeContourIndex = -1;
    bool m_blockOtherContours = false;
    bool m_blockAppendPointContours = false;
    int index_append = -1;
    bool m_blockMovingPointContours = false;
    int moving_index_contour = -1;
    int moving_index_point = -1;
    QPointF initial_mouse_pos;
    // bool toggleBlockOtherContours(bool block);
signals:
    void blockMousePress(bool block);
public slots:
    void onCreateContourToggled(bool checked);
    void onDeletePointToggled(bool checked);
    // void setActiveContour(int index);
    void toggleBlockOtherContours(bool block);
    void toggleAppendPointContours(bool checked);
    void toggleMovingPointContours(bool checked);
    void onDeleteContourToggled(bool checked);
};
#endif // WIDGET_H
