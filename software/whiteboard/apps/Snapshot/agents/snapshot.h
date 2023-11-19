/*
 *	Snapshot
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

#ifndef SNAPSHOT_H
#define SNAPSHOT_H


#include <QObject>
#include <QtQml>
#include <QQuickItem>
#include <QColor>

#include "helpers/i2qmlpropertyhelpers.h"


class Snapshot : public QObject
{
    Q_OBJECT

    I2_QT5_QML_READONLY_PROPERTY(QString, snapshot)


public:
    static Snapshot& instance();
    static QObject* qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine);


Q_SIGNALS:
    void showSnapshot(const QByteArray& base64Png);


public Q_SLOTS:
    void getSnapshot();
    void sendImage(QQuickItem* item, qreal x, qreal y, qreal width, qreal height);


private Q_SLOTS:
    void _showSnapshot(const QByteArray& base64Png);


private:
    explicit Snapshot(QObject *parent = nullptr);
    ~Snapshot() = default; 
};

QML_DECLARE_TYPE(Snapshot)

#endif // SNAPSHOT
