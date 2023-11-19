/*
 *	Whiteboard
 *
 *  Copyright © 2022-2023 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Alexandre Lemort <lemort@ingenuity.io>
 *
 */

#ifndef WHITEBOARD_H
#define WHITEBOARD_H


#include <QObject>
#include <QtQml>
#include <QQuickItem>
#include <QColor>

#include "I2PropertyHelpers.h"

#include "models/graphicelement.h"
#include "models/message.h"


class Whiteboard : public QObject
{
    Q_OBJECT

    I2_QT5_QML_PROPERTY(QQuickItem*, qmlItem)

    I2_QT5_QML_PROPERTY(QString, title)
    I2_QT5_QML_PROPERTY(QColor, backgroundColor)

    I2_QT5_CPP_PROPERTY_WITH_CUSTOM_SETTER(QString, lastChatMessage)
    I2_QT5_CPP_PROPERTY_WITH_CUSTOM_SETTER(QString, lastAction)

    I2_QML_QOBJECT_LIST_PROPERTY(Message, messages)
    I2_QML_QOBJECT_LIST_PROPERTY(GraphicElement, elements)


public:
    static Whiteboard& instance();
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


Q_SIGNALS:
    void chatMessage(const QString& text);
    void chatBot(const QString& agentName, const QString& text);

    void clear();

    void sendSnapshot(const QString& agentUUID, const QString& token);

    void addImage(qreal x, qreal y, qreal width, qreal height, const QByteArray& data, const QString& agentUUID, const QString& token);
    void addImageFromUrl(qreal x, qreal y, const QString& url, const QString& agentUUID, const QString& token);
    void addText(qreal x, qreal y, const QString& text, const QColor& color, const QString& agentUUID, const QString& token);
    void addShape(const QString& type, qreal x, qreal y, qreal width, qreal height, const QColor& fill, const QColor& stroke, qreal strokeWidth, const QString& agentUUID, const QString& token);
    void remove(int elementUID, const QString& agentUUID, const QString& token);
    void translate(int elementUID, qreal dx, qreal dy, const QString& agentUUID, const QString& token);
    void moveTo(int elementUID, qreal x, qreal y, const QString& agentUUID, const QString& token);
    void setStringProperty(int elementUID, const QString& property, const QString& value, const QString& agentUUID, const QString& token);
    void setDoubleProperty(int elementUID, const QString& property, qreal value, const QString& agentUUID, const QString& token);
    void getElementIds(const QString& agentUUID, const QString& token);
    void getElements(const QString& agentUUID, const QString& token);


public Q_SLOTS:
    void removeElement(GraphicElement* element);


private Q_SLOTS:
    void _chatMessage(const QString& text);
    void _chatBot(const QString& agentName, const QString& text);
    void _clear();
    void _sendSnapshot(const QString& agentUUID, const QString& token);
    void _addImage(qreal x, qreal y, qreal width, qreal height, const QByteArray& data, const QString& agentUUID, const QString& token);
    void _addImageFromUrl(qreal x, qreal y, const QString& url, const QString& agentUUID, const QString& token);
    void _addText(qreal x, qreal y, const QString& text, const QColor& color, const QString& agentUUID, const QString& token);
    void _addShape(const QString& type, qreal x, qreal y, qreal width, qreal height, const QColor& fill, const QColor& stroke, qreal strokeWidth, const QString& agentUUID, const QString& token);
    void _remove(int elementUID, const QString& agentUUID, const QString& token);
    void _translate(int elementUID, qreal dx, qreal dy, const QString& agentUUID, const QString& token);
    void _moveTo(int elementUID, qreal x, qreal y, const QString& agentUUID, const QString& token);
    void _setStringProperty(int elementUID, const QString& property, const QString& value, const QString& agentUUID, const QString& token);
    void _setDoubleProperty(int elementUID, const QString& property, qreal value, const QString& agentUUID, const QString& token);
    void _getElementIds(const QString& agentUUID, const QString& token);
    void _getElements(const QString& agentUUID, const QString& token);


private:
    explicit Whiteboard(QObject *parent = nullptr);
    ~Whiteboard() override;


private:
    void _sendElementCreated(int elementUid, const QString& agentUUID, const QString& token);
    void _sendActionResult(bool succeeded, const QString& agentUUID, const QString& token);

    QString _convertToBase64PngImageSource(const QByteArray& base64Data);


private:
    QHash<int, GraphicElement*> _elementsById;
};

QML_DECLARE_TYPE(Whiteboard)

#endif // WHITEBOARD
