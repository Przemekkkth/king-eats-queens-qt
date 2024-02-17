#ifndef GRASS_H
#define GRASS_H
#include <QString>
#include <QPoint>
#include <QRandomGenerator>
#include "utils.h"

struct Grass {
    QString m_grassImgPath;
    const int m_width;
    const int m_height;
    int m_x, m_y;
    QPoint getRandomOffCameraPos(int camerax, int cameray) {
        int x = QRandomGenerator::global()->bounded(camerax - SCREEN::PHYSICAL_SIZE.width(), camerax + (2 * SCREEN::PHYSICAL_SIZE.width()));
        int y = QRandomGenerator::global()->bounded(cameray - SCREEN::PHYSICAL_SIZE.height(), camerax + (2 * SCREEN::PHYSICAL_SIZE.height()));
        //Check if the object is off-camera
        if (x + m_width < camerax || x > camerax + SCREEN::PHYSICAL_SIZE.width() || y + m_height < cameray || y > cameray + SCREEN::PHYSICAL_SIZE.height())
        {
            return QPoint(x, y);
        }
        else {
            return QPoint(0, 0);
        }
    }

    Grass(int camerax, int cameray) : m_width(64), m_height(64) {
        int idx = QRandomGenerator::global()->bounded(0, 5);
        m_grassImgPath = CONF::PATH_TO_GRASS_IMG[idx];
        QPoint pos = getRandomOffCameraPos(camerax, cameray);
        m_x = pos.x();
        m_y = pos.y();
    }
};

#endif // GRASS_H
