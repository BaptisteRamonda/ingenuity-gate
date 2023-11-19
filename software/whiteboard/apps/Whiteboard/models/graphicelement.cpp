/*
 *	Whiteboard
 *
 *  Copyright © 2022 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#include "graphicelement.h"

#include <QtGlobal>




QString GraphicElementType::enumToString(int value)
{
    switch (static_cast<GraphicElementType::Value>(value))
    {
        case GraphicElementType::Image:
            return "image";

        case GraphicElementType::Text:
            return "text";

        case GraphicElementType::Rectangle:
            return "rectangle";

        case GraphicElementType::Ellipse:
            return "ellipse";

        default:
            return "undefined";
    }
}



int GraphicElement::_uidCounter {-1};


GraphicElement::GraphicElement(GraphicElementType::Value type, QObject* parent)
    : QObject{parent}
    , _uid(nextUid())
    , _type(type)
    , _opacity(1.0)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
    _increaseUidCounter();
}


GraphicElement::~GraphicElement() = default;


void GraphicElement::setopacity(I2BestTypeDef<qreal> value)
{
    value = qMax(0.0, qMin(value, 1.0));
    if (!qFuzzyCompare(_opacity, value))
    {
        _opacity = value;
        Q_EMIT opacityChanged();
    }
}


void GraphicElement::translate(qreal dx, qreal dy)
{
    setx(_x + dx);
    sety(_y + dy);
}


void GraphicElement::moveTo(qreal x, qreal y)
{
    setx(x);
    sety(y);
}


bool GraphicElement::setDoubleProperty(const QString& property, qreal value)
{
    bool succeeded {};

    if (QString::compare(property, "x", Qt::CaseInsensitive) == 0)
    {
        setx(value);
        succeeded = true;
    }
    else if (QString::compare(property, "y", Qt::CaseInsensitive) == 0)
    {
        sety(value);
        succeeded = true;
    }
    else if (QString::compare(property, "opacity", Qt::CaseInsensitive) == 0)
    {
        setopacity(value);
        succeeded = true;
    }

    return succeeded;
}


bool GraphicElement::setStringProperty(const QString& property, const QString& value)
{
    Q_UNUSED(property)
    Q_UNUSED(value)
    return false;
}


int GraphicElement::nextUid()
{
    return (_uidCounter + 1);
}


void GraphicElement::resetUidCounter()
{
    _uidCounter = -1;
}


void GraphicElement::_increaseUidCounter()
{
    ++_uidCounter;
}



ImageElement::ImageElement(QObject* parent)
    : GraphicElement{GraphicElementType::Image, parent}
    , _width(qQNaN())
    , _height(qQNaN())
{
}


ImageElement::~ImageElement() = default;


void ImageElement::setwidth(I2BestTypeDef<qreal> value)
{
    value = qMax(0.0, value);
    if (!qFuzzyCompare(_width, value))
    {
        _width = value;
        setexplicitWidth(true);
        Q_EMIT widthChanged();
    }
}


void ImageElement::setheight(I2BestTypeDef<qreal> value)
{
    value = qMax(0.0, value);
    if (!qFuzzyCompare(_height, value))
    {
        _height = value;
        setexplicitHeight(true);
        Q_EMIT heightChanged();
    }
}


bool ImageElement::setDoubleProperty(const QString& property, qreal value)
{
    bool succeeded {};

    if (QString::compare(property, "width", Qt::CaseInsensitive) == 0)
    {
        setwidth(value);
        succeeded = true;
    }
    else if (QString::compare(property, "height", Qt::CaseInsensitive) == 0)
    {
        setheight(value);
        succeeded = true;
    }
    else
        succeeded = GraphicElement::setDoubleProperty(property, value);

    return succeeded;
}


bool ImageElement::setStringProperty(const QString& property, const QString& value)
{
    bool succeeded {};

    if (QString::compare(property, "source", Qt::CaseInsensitive) == 0)
    {
        setsource(value);
        succeeded = true;
    }
    else
        succeeded = GraphicElement::setStringProperty(property, value);

    return succeeded;
}


QJsonObject ImageElement::toJson()
{
    QJsonObject json;
    json.insert("id", _uid);
    json.insert("type", GraphicElementType::instance().enumToString(_type));
    json.insert("x", _x);
    json.insert("y", _y);
    if (_explicitWidth)
        json.insert("width", _width);
    if (_explicitHeight)
        json.insert("height", _height);
    json.insert("source", _source);

    return json;
}


TextElement::TextElement(QObject* parent)
    : GraphicElement{GraphicElementType::Text, parent}
    , _color(Qt::black)
{
}


TextElement::~TextElement() = default;


bool TextElement::setStringProperty(const QString& property, const QString& value)
{
    bool succeeded {};

    if (QString::compare(property, "text", Qt::CaseInsensitive) == 0)
    {
        settext(value);
        succeeded = true;
    }
    else if (QString::compare(property, "color", Qt::CaseInsensitive) == 0)
    {
        if (auto color = QColor(value); color.isValid())
        {
            setcolor(value);
            succeeded = true;
        }
    }
    else
        succeeded = GraphicElement::setStringProperty(property, value);

    return succeeded;
}


QJsonObject TextElement::toJson()
{
    QJsonObject json;
    json.insert("id", _uid);
    json.insert("type", GraphicElementType::instance().enumToString(_type));
    json.insert("x", _x);
    json.insert("y", _y);
    json.insert("text", _text);
    json.insert("color", _color.name());

    return json;
}


ShapeElement::ShapeElement(GraphicElementType::Value type, QObject* parent)
    : GraphicElement{type, parent}
    , _fill(Qt::white)
    , _stroke(Qt::black)
    , _strokeWidth(1)
{
}


ShapeElement::~ShapeElement() = default;


bool ShapeElement::setDoubleProperty(const QString& property, qreal value)
{
    bool succeeded {};

    if (QString::compare(property, "width", Qt::CaseInsensitive) == 0)
    {
        setwidth(value);
        succeeded = true;
    }
    else if (QString::compare(property, "height", Qt::CaseInsensitive) == 0)
    {
        setheight(value);
        succeeded = true;
    }
    else if (QString::compare(property, "strokewidth", Qt::CaseInsensitive) == 0)
    {
        setstrokeWidth(value);
        succeeded = true;
    }
    else
        succeeded = GraphicElement::setDoubleProperty(property, value);

    return succeeded;
}


bool ShapeElement::setStringProperty(const QString& property, const QString& value)
{
    bool succeeded {};

    if (QString::compare(property, "fill", Qt::CaseInsensitive) == 0)
    {
        if (auto color = QColor(value); color.isValid())
        {
            setfill(value);
            succeeded = true;
        }
    }
    else if (QString::compare(property, "stroke", Qt::CaseInsensitive) == 0)
    {
        if (auto color = QColor(value); color.isValid())
        {
            setstroke(value);
            succeeded = true;
        }
    }
    else
        succeeded = GraphicElement::setStringProperty(property, value);

    return succeeded;
}


QJsonObject ShapeElement::toJson()
{
    QJsonObject json;
    json.insert("id", _uid);
    json.insert("type", GraphicElementType::instance().enumToString(_type));
    json.insert("x", _x);
    json.insert("y", _y);
    json.insert("width", _width);
    json.insert("height", _height);
    json.insert("fill", _fill.name());
    json.insert("stroke", _stroke.name());
    json.insert("strokeWidth", _strokeWidth);

    return json;
}


void ShapeElement::setwidth(I2BestTypeDef<qreal> value)
{
    value = qMax(0.0, value);
    if (!qFuzzyCompare(_width, value))
    {
        _width = value;
        Q_EMIT widthChanged();
    }
}


void ShapeElement::setheight(I2BestTypeDef<qreal> value)
{
    value = qMax(0.0, value);
    if (!qFuzzyCompare(_height, value))
    {
        _height = value;
        Q_EMIT heightChanged();
    }
}


void ShapeElement::setstrokeWidth(I2BestTypeDef<qreal> value)
{
    value = qMax(0.0, value);
    if (!qFuzzyCompare(_strokeWidth, value))
    {
        _strokeWidth = value;
        Q_EMIT strokeWidthChanged();
    }
}


RectangleElement::RectangleElement(QObject* parent)
    : ShapeElement{GraphicElementType::Rectangle, parent}
{
}


RectangleElement::~RectangleElement() = default;



EllipseElement::EllipseElement(QObject* parent)
    : ShapeElement{GraphicElementType::Ellipse, parent}
{
}


EllipseElement::~EllipseElement() = default;

