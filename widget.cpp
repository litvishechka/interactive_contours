#include "widget.h"
#include <QPainter>
#include <QPainterPath>
#include <QMenuBar>
#include <QActionGroup>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QFileDialog>
#include <QFile>

Widget::Widget(QWidget *parent)
    : QWidget(parent)

{

    QMenuBar *menuBar = new QMenuBar(this);

    // Добавляем пункты меню
    QMenu *fileMenu = menuBar->addMenu(tr("&Файл"));
    QAction *saveAction = fileMenu->addAction(tr("&Сохранить в файл"), this, &Widget::saveContoursToFile);
    QAction *loadAction = fileMenu->addAction(tr("&Загрузить из файла"), this, &Widget::loadContoursFromFile);

    QMenu *creating_contour = menuBar->addMenu("Создание контура");
    QAction *create_contour = creating_contour->addAction("Создать контур");

    QMenu *editing_contour = menuBar->addMenu("Редактирование контура");

    QMenu *selection_contour = menuBar->addMenu("Выделение контура");
    QAction *select_working_contour = selection_contour->addAction("Выделить рабочий контур");
    // QAction *create_contour = creating_contour->addAction("Создать контур");
    QAction *delete_point = editing_contour->addAction("Удалить точку");
    QAction *delete_contour = editing_contour->addAction("Удалить контур");
    QAction *append_point = editing_contour->addAction("Добавить точку");
    QAction *moving_point = editing_contour->addAction("Переместить точку");
    // QAction *select_working_contour = selection_contour->addAction("Выделить рабочий контур");

    create_contour->setCheckable(true);
    delete_point->setCheckable(true);
    delete_contour->setCheckable(true);
    append_point->setCheckable(true);
    select_working_contour->setCheckable(true);
    moving_point->setCheckable(true);
    saveAction->setCheckable(true);
    setMouseTracking(true);

    connect(create_contour, &QAction::toggled, this, &Widget::onCreateContourToggled);
    connect(delete_point, &QAction::toggled, this, &Widget::onDeletePointToggled);
    connect(delete_contour, &QAction::toggled, this, &Widget::onDeleteContourToggled);
    connect(select_working_contour, &QAction::toggled, this, &Widget::toggleBlockOtherContours);
    connect(append_point, &QAction::toggled, this, &Widget::toggleAppendPointContours);
    connect(moving_point, &QAction::toggled, this, &Widget::toggleMovingPointContours);

    QActionGroup *contourGroupCreateSelect = new QActionGroup(this);
    contourGroupCreateSelect->addAction(create_contour);
    contourGroupCreateSelect->addAction(select_working_contour);

    connect(contourGroupCreateSelect, &QActionGroup::triggered, this, [contourGroupCreateSelect](QAction *action) {
        QList<QAction *> actions = contourGroupCreateSelect->actions();
        for (QAction *otherAction : actions) {
            contourGroupCreateSelect->setExclusive(true);
            if (otherAction != action) {
                otherAction->setChecked(false);
                contourGroupCreateSelect->setExclusive(false);
            }
            else {
                contourGroupCreateSelect->setExclusive(false);
            }


        }
    });


    connect(create_contour, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour](bool checked) {
        if (checked) {
            delete_point->setChecked(false);
            append_point->setChecked(false);
            moving_point->setChecked(false);
            delete_contour->setChecked(false);
        }
    });

    connect(delete_point, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour](bool checked) {
        if (checked) {
            create_contour->setChecked(false);
            append_point->setChecked(false);
            moving_point->setChecked(false);
            delete_contour->setChecked(false);
        }
    });

    connect(append_point, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour](bool checked) {
        if (checked) {
            create_contour->setChecked(false);
            delete_point->setChecked(false);
            moving_point->setChecked(false);
            delete_contour->setChecked(false);
        }
    });

    connect(moving_point, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour](bool checked) {
        if (checked) {
            create_contour->setChecked(false);
            delete_point->setChecked(false);
            append_point->setChecked(false);
            delete_contour->setChecked(false);
        }
    });


    connect(delete_contour, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour](bool checked) {
        if (checked) {
            create_contour->setChecked(false);
            delete_point->setChecked(false);
            append_point->setChecked(false);
            moving_point->setChecked(false);
        }
    });

    connect(saveAction, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour, saveAction, loadAction, select_working_contour](bool checked) {
        if (checked) {
            create_contour->setChecked(false);
            delete_point->setChecked(false);
            append_point->setChecked(false);
            moving_point->setChecked(false);
            delete_contour->setChecked(false);
            loadAction->setChecked(false);
            select_working_contour->setChecked(false);
        }
    });
    connect(loadAction, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour, saveAction, loadAction, select_working_contour](bool checked) {
        if (checked) {
            create_contour->setChecked(false);
            delete_point->setChecked(false);
            append_point->setChecked(false);
            moving_point->setChecked(false);
            delete_contour->setChecked(false);
            saveAction->setChecked(false);
            select_working_contour->setChecked(false);
        }
    });
    connect(select_working_contour, &QAction::toggled, this, [create_contour, delete_point, append_point, moving_point, delete_contour, saveAction, loadAction, select_working_contour](bool checked) {
        if (checked) {
            create_contour->setChecked(false);
            delete_point->setChecked(false);
            append_point->setChecked(false);
            moving_point->setChecked(false);
            delete_contour->setChecked(false);
            saveAction->setChecked(false);
            loadAction->setChecked(false);
        }
    });

}

Widget::~Widget()
{

}

void Widget::mousePressEvent(QMouseEvent *event)
{
    if (m_blockMousePress && event->buttons() == Qt::LeftButton) {
        for (int i = 0; i < points.size(); ++i) {
            if (points[i].x() - 8 < event->pos().x() && event->pos().x() < points[i].x() + 8 && points[i].y() - 8 < event->pos().y() && event->pos().y() < points[i].y() + 8) {
                collision_points = true;
            }
        }
        if(!collision_points) points.append(event->pos());
        update();
    }
    if (m_blockDeletePoint && event->buttons() == Qt::LeftButton) {
        QList<QList<QPointF>>::iterator it = contours_points.begin();
        auto removePointLambda = [&event](const QPointF &point) {
            return point.x() - 8 < event->pos().x() && event->pos().x() < point.x() + 8
                   && point.y() - 8 < event->pos().y() && event->pos().y() < point.y() + 8;
        };

        while (it != contours_points.end()) {
            bool isClosedContour = !it->isEmpty() && (it->first() == it->last());
            if (m_activeContourIndex >= 0 && m_activeContourIndex < contours_points.count()) {
                if (m_blockOtherContours) {
                    if ((*it == contours_points[m_activeContourIndex] && m_blockOtherContours)
                        || contours_points.count() == 1) {
                        QPointF firstPoint = isClosedContour ? it->first() : QPointF();

                        it->erase(std::remove_if(it->begin(), it->end(), removePointLambda), it->end());

                        if (it->isEmpty()) {
                            it = contours_points.erase(it);
                            setActiveContour(-1);
                        } else {
                            if (isClosedContour && (it->isEmpty() || it->first() != firstPoint)) {
                                it->prepend(firstPoint);
                            }
                            ++it;
                        }
                    } else ++it;
                }
                else {
                    QMessageBox msgBox;
                    msgBox.setText("Выберите рабочий контур!");
                    msgBox.exec();
                }
            } else {
                QPointF firstPoint = isClosedContour ? it->first() : QPointF();

                it->erase(std::remove_if(it->begin(), it->end(), removePointLambda), it->end());

                if (it->isEmpty()) {
                    it = contours_points.erase(it);
                } else {
                    if (isClosedContour && (it->isEmpty() || it->first() != firstPoint)) {
                        it->prepend(firstPoint);
                    }
                    ++it;
                }
            }
        }
        update();
    }

    if (m_blockOtherContours && event->buttons() == Qt::LeftButton) {
        for (int k = 0; k < contours_points.size(); ++k) {
            for (int i = 0; i < contours_points[k].size(); ++i) {
                if (contours_points[k][i].x() - 8 < event->pos().x() && event->pos().x() < contours_points[k][i].x() + 8 && contours_points[k][i].y() - 8 < event->pos().y() && event->pos().y() < contours_points[k][i].y() + 8) {
                    setActiveContour(k);
                }
            }
        }
    }

    if (m_blockAppendPointContours && event->buttons() == Qt::LeftButton) {
        for (int k = 0; k < contours_points.size(); ++k) {
            bool isClosedContour = contours_points[k][0] == contours_points[k][contours_points[k].size() - 1];

            if (k == m_activeContourIndex || contours_points.size() == 1) {
                bool pointInserted = false;
                for (int i = 0; i < contours_points[k].size(); ++i) {
                    QPointF currentPoint = contours_points[k][i];
                    QPointF nextPoint = (i == contours_points[k].size() - 1) ? contours_points[k][0] : contours_points[k][i + 1];

                    if (isClosedContour && i == contours_points[k].size() - 1) {
                        continue;
                    }

                    if ((!isClosedContour || i != contours_points[k].size() - 1) &&
                        ((currentPoint.x() <= event->pos().x() && event->pos().x() <= nextPoint.x() ||
                          nextPoint.x() <= event->pos().x() && event->pos().x() <= currentPoint.x()) &&
                         (currentPoint.y() <= event->pos().y() && event->pos().y() <= nextPoint.y() ||
                          nextPoint.y() <= event->pos().y() && event->pos().y() <= currentPoint.y()))) {

                        // Вставка точки в контур
                        contours_points[k].insert(i + 1, event->pos());
                        pointInserted = true;
                        break;
                    }
                }

                if (!pointInserted && !isClosedContour) {
                    // Добавление точки в незамкнутый контур
                    contours_points[k].push_back(event->pos());
                }
            }
        }
    }



    if (m_blockMovingPointContours && event->buttons() == Qt::LeftButton) {
        for (int k = 0; k < contours_points.size(); ++k) {
            if (k == m_activeContourIndex || contours_points.size() == 1) {
                for (int i = 0; i < contours_points[k].size(); ++i) {
                    if(contours_points[k][i].x() - 8 < event->pos().x() && event->pos().x() < contours_points[k][i].x() + 8 &&
                        contours_points[k][i].y() - 8 < event->pos().y() && event->pos().y() < contours_points[k][i].y() + 8) {
                        moving_index_contour = k;
                        moving_index_point = i;
                    }
                }

            }
        }
    }

    if (m_blockDeleteContour && event->buttons() == Qt::LeftButton) {
        if (m_activeContourIndex >= 0 && m_activeContourIndex < contours_points.count()) {
            auto it = contours_points.begin() + m_activeContourIndex;
            if (m_blockOtherContours) {
                it->clear(); // Clear the active contour
                contours_points.erase(it); // Remove the active contour from the list
                setActiveContour(-1); // Reset the active contour
            }
        } else {
            if (contours_points.count() == 1) {
                contours_points.clear();
                setActiveContour(-1); // Reset the active contour index
            }
            else {
                QMessageBox msgBox;
                msgBox.setText("Выберите рабочий контур!");
                msgBox.exec();
                }
        }
        update();
    }

}

void Widget::mouseMoveEvent(QMouseEvent *event)
{
    if (m_blockMovingPointContours && moving_index_point != -1 && moving_index_contour != -1 && event->buttons() == Qt::LeftButton) {
        if (contours_points[moving_index_contour][moving_index_point] == contours_points[moving_index_contour][0]) {
            contours_points[moving_index_contour][moving_index_point] = event->pos();
            contours_points[moving_index_contour][0] = contours_points[moving_index_contour][moving_index_point];
        }
        else {
            contours_points[moving_index_contour][moving_index_point] = event->pos();
        }
        update();
    }
}


void Widget::mouseReleaseEvent(QMouseEvent *event)
{
    moving_index_point = -1;
    moving_index_contour = -1;
    update();
}

void Widget::paintEvent(QPaintEvent *) {
    if (!m_blockOtherContours) setActiveContour(-1);
    for (int k = 0; k < contours_points.size(); ++k) {
        QPainterPath path(contours_points[k][0]);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        if (k == m_activeContourIndex) {
            painter.setPen(QPen(Qt::blue, 2)); // Цвет рабочего контура
        } else {
            painter.setPen(QPen(Qt::black, 2)); // Цвет остальных контуров
        }
        // Рассчитываем
        for (int i = 0; i < contours_points[k].size() - 1; ++i) {
            if (contours_points[k].first() == contours_points[k].last() && i == contours_points[k].size() - 2) {
                QPointF sp = contours_points[k].value(i);
                QPointF ep = contours_points[k].value(0);
                QPointF c1 = sp + (contours_points[k].value(0) - contours_points[k].value(i)) / 6;
                QPointF c2 = ep - (contours_points[k].value(1) - contours_points[k].value(i-1)) / 6;
                path.cubicTo(c1, c2, ep);
            }
            else {
                QPointF sp = contours_points[k][i];
                QPointF ep = contours_points[k][i+1];
                QPointF c1 = sp + (contours_points[k].value(i+1) - contours_points[k].value(i-1)) / 6;
                QPointF c2 = ep - (contours_points[k].value(i+2) - contours_points[k].value(i)) / 6;

                path.cubicTo(c1, c2, ep);
            }
        }
        // Рисуем путь
        painter.drawPath(path);
        // Рисуем точку на кривой
        painter.setBrush(Qt::gray);
        // Рисуем точку на кривой
        for (int i = 0; i < contours_points[k].size(); ++i) {
            painter.drawEllipse(contours_points[k][i], 4, 4);
        }
    }

    if (points.length() > 0) {
        QPainterPath path(points[0]);
        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setPen(QPen(Qt::black, 2));
        for (int i = 0; i < points.size() - 1; ++i) {
            if (points.first() == points.last() && i == points.size() - 2) {
                QPointF sp = points[points.size() - 2];
                QPointF ep = points.first();
                QPointF c1 = sp + (points[0] - points[points.size() - 2]) / 6;
                QPointF c2 = ep - (points[1] - points[points.size() - 1]) / 6;
                path.cubicTo(c1, c2, ep);
            }
            else {
                QPointF sp = points[i];
                QPointF ep = points[i+1];

                QPointF c1 = sp + (points.value(i+1) - points.value(i-1)) / 6;
                QPointF c2 = ep - (points.value(i+2) - points.value(i)) / 6;
                path.cubicTo(c1, c2, ep);  
            }
        }
        if (collision_points && points.first() != points.last()) {
            points.append(points.first());
            QPointF sp = points[points.size() - 2];
            QPointF ep = points.first();
            QPointF c1 = sp + (points[0] - points[points.size() - 2]) / 6;
            QPointF c2 = ep - (points[1] - points[points.size() - 1]) / 6;
            path.cubicTo(c1, c2, ep);
            update();
        }

        // Рисуем путь
        painter.drawPath(path);
        // Рисуем точку на кривой
        painter.setBrush(Qt::gray);
        // Рисуем точку на кривой
        for (int i = 0; i < points.size(); ++i) {
            painter.drawEllipse(points[i], 4, 4);
        }
    }

    if (!m_blockMousePress) {
        if (!(points.isEmpty())) {
            contours_points.append(points);
            points.clear();
            update();
        }
    }
}

void Widget::onCreateContourToggled(bool checked)
{
    m_blockMousePress = checked;
    if(!checked) collision_points = false;
}

void Widget::onDeletePointToggled(bool checked)
{
    m_blockDeletePoint = checked;
}

void Widget::onDeleteContourToggled(bool checked)
{
    m_blockDeleteContour = checked;
}


void Widget::setActiveContour(int index) {
    m_activeContourIndex = index;
    update();
}

void Widget::toggleBlockOtherContours(bool block) {
    m_blockOtherContours = block;
}

void Widget::toggleAppendPointContours(bool checked) {
    m_blockAppendPointContours = checked;
}

void Widget::toggleMovingPointContours(bool checked) {
    m_blockMovingPointContours = checked;
}

void Widget::saveContoursToFile() {
    QJsonArray contourArray;
    for (const auto& contour : contours_points) {
        QJsonArray pointArray;
        for (const auto& point : contour) {
            QJsonObject pointObject;
            pointObject["x"] = point.x();
            pointObject["y"] = point.y();
            pointArray.append(pointObject);
        }
        contourArray.append(pointArray);
    }

    QJsonObject rootObject;
    rootObject["contours"] = contourArray;

    QJsonDocument doc(rootObject);
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Contours"), "", tr("JSON Files (*.json)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::WriteOnly)) {
            file.write(doc.toJson());
            file.close();
        } else {
            qDebug() << "Не удалось открыть файл для записи.";
        }
    }
}

void Widget::loadContoursFromFile() {
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open Contours"), "", tr("JSON Files (*.json)"));
    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QIODevice::ReadOnly)) {
            QByteArray data = file.readAll();
            QJsonDocument doc(QJsonDocument::fromJson(data));
            QJsonArray contourArray = doc.object()["contours"].toArray();

            contours_points.clear();
            for (int i = 0; i < contourArray.size(); ++i) {
                QList<QPointF> contour;
                QJsonArray pointArray = contourArray[i].toArray();
                for (int j = 0; j < pointArray.size(); ++j) {
                    QJsonObject pointObject = pointArray[j].toObject();
                    contour.append(QPointF(pointObject["x"].toDouble(), pointObject["y"].toDouble()));
                }
                contours_points.append(contour);
            }
            update();
        }
    }
}
