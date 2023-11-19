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


#ifndef GRAPHICELEMENT_H
#define GRAPHICELEMENT_H

#include <QObject>
#include <QColor>
#include <QtQml>
#include <QJsonDocument>


#include "I2PropertyHelpers.h"


I2_CUSTOM_ENUM(GraphicElementType
            , Undefined = 0
            , Image
            , Text
            , Rectangle
            , Ellipse
            )


class GraphicElement : public QObject
{
    Q_OBJECT

    I2_QT5_QML_CONSTANT_PROPERTY(int, uid)
    I2_QT5_QML_CONSTANT_PROPERTY(GraphicElementType::Value, type)
    I2_QT5_QML_PROPERTY(qreal, x)
    I2_QT5_QML_PROPERTY(qreal, y)
    I2_QT5_QML_PROPERTY_WITH_CUSTOM_SETTER(qreal, opacity)


public:
    explicit GraphicElement(GraphicElementType::Value type, QObject *parent = nullptr);
    virtual ~GraphicElement() override;

    void translate(qreal dx, qreal dy);
    void moveTo(qreal x, qreal y);

    virtual bool setDoubleProperty(const QString& property, qreal value);
    virtual bool setStringProperty(const QString& property, const QString& value);

    virtual QJsonObject toJson() = 0;

    static int nextUid();
    static void resetUidCounter();


private:
    static void _increaseUidCounter();


private:
    static int _uidCounter;
};

QML_DECLARE_TYPE(GraphicElement)



class ImageElement : public GraphicElement
{
    Q_OBJECT

    I2_QT5_QML_READONLY_PROPERTY(bool, explicitWidth)
    I2_QT5_QML_PROPERTY_WITH_CUSTOM_SETTER(qreal, width)
    I2_QT5_QML_READONLY_PROPERTY(bool, explicitHeight)
    I2_QT5_QML_PROPERTY_WITH_CUSTOM_SETTER(qreal, height)
    I2_QT5_QML_PROPERTY(QString, source)

public:
    explicit ImageElement(QObject *parent = nullptr);
    ~ImageElement() override;

    bool setDoubleProperty(const QString& property, qreal value) override;
    bool setStringProperty(const QString& property, const QString& value) override;

    QJsonObject toJson() override;
};

QML_DECLARE_TYPE(ImageElement)



class TextElement : public GraphicElement
{
    Q_OBJECT

    I2_QT5_QML_PROPERTY(QString, text)
    I2_QT5_QML_PROPERTY(QColor, color)

public:
    explicit TextElement(QObject *parent = nullptr);
    ~TextElement() override;

    bool setStringProperty(const QString& property, const QString& value) override;

    QJsonObject toJson() override;
};

QML_DECLARE_TYPE(TextElement)



class ShapeElement : public GraphicElement
{
    Q_OBJECT

    I2_QT5_QML_PROPERTY_WITH_CUSTOM_SETTER(qreal, width)
    I2_QT5_QML_PROPERTY_WITH_CUSTOM_SETTER(qreal, height)
    I2_QT5_QML_PROPERTY(QColor, fill)
    I2_QT5_QML_PROPERTY(QColor, stroke)
    I2_QT5_QML_PROPERTY_WITH_CUSTOM_SETTER(qreal, strokeWidth)

public:
    explicit ShapeElement(GraphicElementType::Value type, QObject *parent = nullptr);
    ~ShapeElement() override;

    bool setDoubleProperty(const QString& property, qreal value) override;
    bool setStringProperty(const QString& property, const QString& value) override;

    QJsonObject toJson() override;
};

QML_DECLARE_TYPE(ShapeElement)



class RectangleElement : public ShapeElement
{
    Q_OBJECT

public:
    explicit RectangleElement(QObject *parent = nullptr);
    ~RectangleElement() override;
};

QML_DECLARE_TYPE(RectangleElement)


class EllipseElement: public ShapeElement
{
    Q_OBJECT

public:
    explicit EllipseElement(QObject *parent = nullptr);
    ~EllipseElement() override;
};

QML_DECLARE_TYPE(EllipseElement)


#endif // GRAPHICELEMENT_H
