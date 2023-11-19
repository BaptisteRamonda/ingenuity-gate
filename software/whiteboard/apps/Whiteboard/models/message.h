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


#ifndef MESSAGE_H
#define MESSAGE_H

#include <QObject>
#include <QtQml>

#include "helpers/i2qmlpropertyhelpers.h"


class Message : public QObject
{
    Q_OBJECT

    I2_QT5_QML_CONSTANT_PROPERTY(bool, fromChatBot)
    I2_QT5_QML_CONSTANT_PROPERTY(QString, chatBotName)
    I2_QT5_QML_CONSTANT_PROPERTY(QString, text)


public:
    explicit Message(const QString& text, QObject *parent = nullptr);
    explicit Message(const QString& chatName, const QString& text, QObject *parent = nullptr);
    ~Message() override;
};

QML_DECLARE_TYPE(Message)

#endif // MESSAGE_H
