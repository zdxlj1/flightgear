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

#include "fgqcanvasimage.h"

#include <QPainter>
#include <QDebug>
#include <QQmlComponent>

#include "fgcanvaspaintcontext.h"
#include "localprop.h"
#include "fgqcanvasimageloader.h"
#include "canvasitem.h"

static QQmlComponent* static_imageComponent = nullptr;

FGQCanvasImage::FGQCanvasImage(FGCanvasGroup* pr, LocalProp* prop) :
    FGCanvasElement(pr, prop)
{

}

void FGQCanvasImage::setEngine(QQmlEngine *engine)
{
    static_imageComponent = new QQmlComponent(engine, QUrl("image.qml"));
    qDebug() << static_imageComponent->errorString();
}

void FGQCanvasImage::doPaint(FGCanvasPaintContext *context) const
{
    if (_imageDirty) {
        rebuildImage();
        _imageDirty = false;
    }

    if (_sourceRectDirty) {
        recomputeSourceRect();
    }

    QRectF dstRect(0.0, 0.0, _destSize.width(), _destSize.height());
    context->painter()->drawPixmap(dstRect, _image, _sourceRect);
}

bool FGQCanvasImage::onChildAdded(LocalProp *prop)
{
    if (FGCanvasElement::onChildAdded(prop)) {
        return true;
    }

    const QByteArray nm = prop->name();
    if ((nm == "src") || (nm == "size") || (nm == "file")) {
        connect(prop, &LocalProp::valueChanged, this, &FGQCanvasImage::markImageDirty);
        return true;
    }

    if (nm == "source") {
        FGQCanvasImage* self = this;
        connect(prop, &LocalProp::childAdded, [self](LocalProp* newChild) {
            connect(newChild, &LocalProp::valueChanged, self, &FGQCanvasImage::markSourceDirty);
        });
        return true;
    }

    qDebug() << "image saw child:" << prop->name();
    return false;
}

void FGQCanvasImage::markImageDirty()
{
    _imageDirty = true;
}

void FGQCanvasImage::markSourceDirty()
{
    _sourceRectDirty = true;
}

void FGQCanvasImage::recomputeSourceRect() const
{
    const float imageWidth = _image.width();
    const float imageHeight = _image.height();
    _sourceRect = QRectF(0, 0, imageWidth, imageHeight);
    if (!_propertyRoot->hasChild("source")) {
        return;
    }

    const bool normalized = _propertyRoot->value("source/normalized", true).toBool();
    float left =  _propertyRoot->value("source/left", 0.0).toFloat();
    float top =  _propertyRoot->value("source/top", 0.0).toFloat();
    float right =  _propertyRoot->value("source/right", 1.0).toFloat();
    float bottom =  _propertyRoot->value("source/bottom", 1.0).toFloat();

    if (normalized) {
        left *= imageWidth;
        right *= imageWidth;
        top *= imageHeight;
        bottom *= imageHeight;
    }

    _sourceRect =  QRectF(left, top, right - left, bottom - top);
    _sourceRectDirty = false;
}

void FGQCanvasImage::rebuildImage() const
{
    QByteArray file = _propertyRoot->value("file", QByteArray()).toByteArray();
    if (!file.isEmpty()) {
         _image = FGQCanvasImageLoader::instance()->getImage(file);


        if (_image.isNull()) {
            // get notified when the image loads
            FGQCanvasImageLoader::instance()->connectToImageLoaded(file,
                                                                   const_cast<FGQCanvasImage*>(this),
                                                                   SLOT(markImageDirty()));
        } else {
            // loaded image ok!
        }

     } else {
        qDebug() << "src" << _propertyRoot->value("src", QString());
    }

    _destSize = QSizeF(_propertyRoot->value("size[0]", 0.0).toFloat(),
                       _propertyRoot->value("size[1]", 0.0).toFloat());

    _imageDirty = false;
}

void FGQCanvasImage::markStyleDirty()
{
}


CanvasItem *FGQCanvasImage::createQuickItem(QQuickItem *parent)
{
    Q_ASSERT(static_imageComponent);
    _quickItem = qobject_cast<CanvasItem*>(static_imageComponent->create());
    _quickItem->setParentItem(parent);
    return _quickItem;
}

CanvasItem *FGQCanvasImage::quickItem() const
{
    return _quickItem;
}
