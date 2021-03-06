//
// Copyright (C) 2017 James Turner  zakalawe@mac.com
//
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License as
// published by the Free Software Foundation; either version 2 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful, but
// WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef CANVASITEM_H
#define CANVASITEM_H

#include <QQuickItem>

class LocalTransform;

class CanvasItem : public QQuickItem
{
    Q_OBJECT
public:
    CanvasItem(QQuickItem* pr = nullptr);

    void setTransform(const QMatrix4x4& mat);
signals:

public slots:

private:
    LocalTransform* m_localTransform;
};

#endif // CANVASITEM_H
