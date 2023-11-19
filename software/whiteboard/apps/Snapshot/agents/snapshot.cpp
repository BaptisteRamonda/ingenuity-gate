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

#include "snapshot.h"

#if defined(INGESCAPE_FROM_PRI)
    #include "ingescape.h"
#else
    #include <ingescape/ingescape.h>
#endif // INGESCAPE_FROM_PRI

#include <QQuickItemGrabResult>



void Snapshot_snapshotResultCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceerAgentUUID)
    Q_UNUSED(serviceName)
    Q_UNUSED(nbArgs)
    Q_UNUSED(token)

    if (auto agent = static_cast<Snapshot*>(myData))
    {
        auto base64Png = QByteArray::fromRawData(static_cast<char*>(firstArgument->data), firstArgument->size);
        Q_EMIT agent->showSnapshot(base64Png);
    }
}


void Snapshot_elementCreatedCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceerAgentUUID)
    Q_UNUSED(serviceName)
    Q_UNUSED(nbArgs)
    Q_UNUSED(token)
    Q_UNUSED(myData)

    if (firstArgument)
        qInfo() << "Element created: " << firstArgument->i;
}


Snapshot::Snapshot(QObject *parent)
    : QObject(parent)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    igs_agent_set_name("Snapshot");

    igs_service_init("snapshotResult", Snapshot_snapshotResultCallback, this);
    igs_service_arg_add("snapshotResult", "base64Png", IGS_DATA_T);

    igs_service_init("elementCreated", Snapshot_elementCreatedCallback, this);
    igs_service_arg_add("elementCreated", "elementId", IGS_INTEGER_T);

    connect(this, &Snapshot::showSnapshot, this, &Snapshot::_showSnapshot);
}


Snapshot& Snapshot::instance()
{
    static Snapshot _instance;
    return _instance;
}


QObject* Snapshot::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return &Snapshot::instance();
}


void Snapshot::getSnapshot()
{
    igs_service_call("Whiteboard", "snapshot", nullptr, nullptr);
}


void Snapshot::sendImage(QQuickItem* item, qreal x, qreal y, qreal width, qreal height)
{
    if (!item)
    {
        qWarning() << "sendImage warning: QML item is not defined";
        return;
    }

    if (auto grabResult = item->grabToImage(item->size().toSize()); !grabResult.isNull())
    {
        QObject::connect(grabResult.data(), &QQuickItemGrabResult::ready, this, [=]() {
            auto image = grabResult->image();
            if (!image.isNull())
            {
                QByteArray byteArray;
                QBuffer buffer(&byteArray);
                if (image.save(&buffer, "PNG"))
                {
                    byteArray = byteArray.toBase64();

                    igs_service_arg_t* arguments {};
                    igs_service_args_add_data(&arguments, byteArray.data(), byteArray.size());
                    igs_service_args_add_double(&arguments, x);
                    igs_service_args_add_double(&arguments, y);
                    igs_service_args_add_double(&arguments, width);
                    igs_service_args_add_double(&arguments, height);

                    igs_service_call("Whiteboard", "addImage", &arguments, nullptr);
                }
                else
                    qWarning() << "sendImage warning: failed to take a snapshot of our QML item";
            }
            else
                qWarning() << "sendImage warning: failed to take a snapshot of our QML item";

            if (grabResult.data())
                grabResult.data()->disconnect();
        });
    }
}


void Snapshot::_showSnapshot(const QByteArray& base64Png)
{
    if (!base64Png.isEmpty())
    {
        QString image {"data:image/png;base64,"};
        image.append(QString::fromLatin1(base64Png.data()));
        setsnapshot(image);
    }
    else
        setsnapshot("");
}


