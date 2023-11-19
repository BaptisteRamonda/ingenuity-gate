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

#include "whiteboard.h"

#if defined(INGESCAPE_FROM_PRI)
    #include "ingescape.h"
#else
    #include <ingescape/ingescape.h>
#endif // INGESCAPE_FROM_PRI

#include <QQuickItemGrabResult>
#include <QImage>
#include <QFuture>
#include <QFutureWatcher>
#include <QtConcurrent>
#include <QJsonDocument>

#include <algorithm>



void Whiteboard_inputTitleCallback(igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType,
	                    void* value, size_t valueSize, void* myData) {
    Q_UNUSED(iopType)
    Q_UNUSED(name)
    Q_UNUSED(valueType)
    Q_UNUSED(valueSize)

    if (auto agent = static_cast<Whiteboard*>(myData))
    {
        QString newTitle = QString(static_cast<char*>(value));
        agent->settitle(newTitle);
        agent->setlastAction(QObject::tr("Input title: %1").arg(newTitle));
    }
}


void Whiteboard_inputBackgroundColorCallback(igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType,
	                    void* value, size_t valueSize, void* myData) {
    Q_UNUSED(iopType)
    Q_UNUSED(name)
    Q_UNUSED(valueType)
    Q_UNUSED(valueSize)

    if (auto agent = static_cast<Whiteboard*>(myData))
    {
        auto newColor = QString(static_cast<char*>(value));
        agent->setbackgroundColor(QColor(newColor));
        agent->setlastAction(QObject::tr("Input backgroundColor: %1").arg(newColor));
    }
}


void Whiteboard_inputChatMessageCallback(igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType,
	                    void* value, size_t valueSize, void* myData) {
    Q_UNUSED(iopType)
    Q_UNUSED(name)
    Q_UNUSED(valueType)
    Q_UNUSED(valueSize)

    if (auto agent = static_cast<Whiteboard*>(myData))
        Q_EMIT agent->chatMessage(QString(static_cast<char*>(value)));
}


void Whiteboard_inputClearCallback(igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType,
	                    void* value, size_t valueSize, void* myData) {
    Q_UNUSED(iopType)
    Q_UNUSED(name)
    Q_UNUSED(valueType)
    Q_UNUSED(value)
    Q_UNUSED(valueSize)

    if (auto agent = static_cast<Whiteboard*>(myData))
    {
        Q_EMIT agent->clear();
        agent->setlastAction(QObject::tr("Input clear"));
    }
}


void Whiteboard_chatCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentUUID)
    Q_UNUSED(serviceName)
    Q_UNUSED(nbArgs)
    Q_UNUSED(token)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument)
        Q_EMIT agent->chatBot(QString(serviceerAgentName), QString(firstArgument->c));
}


void Whiteboard_snapshotCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)
    Q_UNUSED(firstArgument)
    Q_UNUSED(nbArgs)
    Q_UNUSED(token)

    if (auto agent = static_cast<Whiteboard*>(myData))
    {
        auto agentUUID = QString(serviceerAgentUUID);
        auto replyToken = QString(token);
        Q_EMIT agent->sendSnapshot(agentUUID, replyToken);
    }
}


void Whiteboard_clearCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)
    Q_UNUSED(serviceerAgentUUID)
    Q_UNUSED(firstArgument)
    Q_UNUSED(nbArgs)
    Q_UNUSED(token)

    if (auto agent = static_cast<Whiteboard*>(myData))
    {
        Q_EMIT agent->clear();
        agent->setlastAction(QObject::tr("Service clear"));
    }
}


void Whiteboard_addShapeCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 8))
    {
        auto type = QString(firstArgument->c);
        double x = firstArgument->next->d;
        double y = firstArgument->next->next->d;
        double width = firstArgument->next->next->next->d;
        double height = firstArgument->next->next->next->next->d;
        auto fill = QColor(firstArgument->next->next->next->next->next->c);
        auto stroke = QColor(firstArgument->next->next->next->next->next->next->c);
        double strokeWidth = firstArgument->next->next->next->next->next->next->next->d;

        Q_EMIT agent->addShape(type, x, y, width, height, fill, stroke, strokeWidth, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_addTextCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 4))
    {
        auto text = QString(firstArgument->c);
        double x = firstArgument->next->d;
        double y = firstArgument->next->next->d;
        auto color = QColor(firstArgument->next->next->next->c);

        Q_EMIT agent->addText(x, y, text, color, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_addImageCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)
    Q_UNUSED(nbArgs)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 5))
    {
        QByteArray base64 = QByteArray::fromRawData(static_cast<char*>(firstArgument->data), firstArgument->size);
        double x = firstArgument->next->d;
        double y = firstArgument->next->next->d;
        double width = firstArgument->next->next->next->d;
        double height = firstArgument->next->next->next->next->d;

        Q_EMIT agent->addImage(x, y, width, height, base64, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_addImageFromUrlCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 3))
    {
        auto url = QString(firstArgument->c);
        double x = firstArgument->next->d;
        double y = firstArgument->next->next->d;

        Q_EMIT agent->addImageFromUrl(x, y, url, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_removeCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 1))
    {
         int elementUid = firstArgument->i;

        Q_EMIT agent->remove(elementUid, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_translateCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 3))
    {
        int elementUid = firstArgument->i;
        double dx = firstArgument->next->d;
        double dy = firstArgument->next->next->d;

        Q_EMIT agent->translate(elementUid, dx, dy, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_moveToCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 3))
    {
        int elementUid = firstArgument->i;
        double x = firstArgument->next->d;
        double y = firstArgument->next->next->d;

        Q_EMIT agent->moveTo(elementUid, x, y, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_setStringPropertyCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 3))
    {
        int elementId = firstArgument->i;
        auto property = QString(firstArgument->next->c);
        auto value = QString(firstArgument->next->next->c);

        Q_EMIT agent->setStringProperty(elementId, property, value, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_setDoublePropertyCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)

    if (auto agent = static_cast<Whiteboard*>(myData); agent && firstArgument && (nbArgs == 3))
    {
        int elementId = firstArgument->i;
        auto property = QString(firstArgument->next->c);
        double value = firstArgument->next->next->d;

        Q_EMIT agent->setDoubleProperty(elementId, property, value, QString(serviceerAgentUUID), QString(token));
    }
}


void Whiteboard_getElementIdsCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)
    Q_UNUSED(firstArgument)
    Q_UNUSED(nbArgs)

    if (auto agent = static_cast<Whiteboard*>(myData))
        Q_EMIT agent->getElementIds(QString(serviceerAgentUUID), QString(token));
}


void Whiteboard_getElementsCallback(const char *serviceerAgentName, const char *serviceerAgentUUID,
                          const char *serviceName, igs_service_arg_t *firstArgument, size_t nbArgs,
                          const char *token, void* myData){
    Q_UNUSED(serviceerAgentName)
    Q_UNUSED(serviceName)
    Q_UNUSED(firstArgument)
    Q_UNUSED(nbArgs)

    if (auto agent = static_cast<Whiteboard*>(myData))
        Q_EMIT agent->getElements(QString(serviceerAgentUUID), QString(token));
}



Whiteboard::Whiteboard(QObject *parent)
    : QObject(parent)
    , _title("Whiteboard")
    , _backgroundColor(Qt::white)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);

    igs_agent_set_name("Whiteboard");

    igs_input_create("title", IGS_STRING_T, nullptr, 0);
    igs_observe_input("title", Whiteboard_inputTitleCallback, this);
    igs_input_create("backgroundColor", IGS_STRING_T, nullptr, 0);
    igs_observe_input("backgroundColor", Whiteboard_inputBackgroundColorCallback, this);
    igs_input_create("chatMessage", IGS_STRING_T, nullptr, 0);
    igs_observe_input("chatMessage", Whiteboard_inputChatMessageCallback, this);
    igs_input_create("clear", IGS_IMPULSION_T, nullptr, 0);
    igs_observe_input("clear", Whiteboard_inputClearCallback, this);

    igs_output_create("lastChatMessage", IGS_STRING_T, nullptr, 0);
    igs_output_create("lastAction", IGS_STRING_T, nullptr, 0);

    igs_service_init("chat", Whiteboard_chatCallback, this);
    igs_service_arg_add("chat", "message", IGS_STRING_T);

    igs_service_init("snapshot", Whiteboard_snapshotCallback, this);
    igs_service_reply_add("snapshot", "snapshotResult");
    igs_service_reply_arg_add("snapshot", "snapshotResult", "base64Png", IGS_DATA_T);

    igs_service_init("clear", Whiteboard_clearCallback, this);

    igs_service_init("addShape", Whiteboard_addShapeCallback, this);
    igs_service_arg_add("addShape", "type", IGS_STRING_T);
    igs_service_arg_add("addShape", "x", IGS_DOUBLE_T);
    igs_service_arg_add("addShape", "y", IGS_DOUBLE_T);
    igs_service_arg_add("addShape", "width", IGS_DOUBLE_T);
    igs_service_arg_add("addShape", "height", IGS_DOUBLE_T);
    igs_service_arg_add("addShape", "fill", IGS_STRING_T);
    igs_service_arg_add("addShape", "stroke", IGS_STRING_T);
    igs_service_arg_add("addShape", "strokeWidth", IGS_DOUBLE_T);
    igs_service_reply_add("addShape", "elementCreated");
    igs_service_reply_arg_add("addShape", "elementCreated", "elementId", IGS_INTEGER_T);

    igs_service_init("addText", Whiteboard_addTextCallback, this);
    igs_service_arg_add("addText", "text", IGS_STRING_T);
    igs_service_arg_add("addText", "x", IGS_DOUBLE_T);
    igs_service_arg_add("addText", "y", IGS_DOUBLE_T);
    igs_service_arg_add("addText", "color", IGS_STRING_T);
    igs_service_reply_add("addText", "elementCreated");
    igs_service_reply_arg_add("addText", "elementCreated", "elementId", IGS_INTEGER_T);

    igs_service_init("addImage", Whiteboard_addImageCallback, this);
    igs_service_arg_add("addImage", "base64", IGS_DATA_T);
    igs_service_arg_add("addImage", "x", IGS_DOUBLE_T);
    igs_service_arg_add("addImage", "y", IGS_DOUBLE_T);
    igs_service_arg_add("addImage", "width", IGS_DOUBLE_T);
    igs_service_arg_add("addImage", "height", IGS_DOUBLE_T);
    igs_service_reply_add("addImage", "elementCreated");
    igs_service_reply_arg_add("addImage", "elementCreated", "elementId", IGS_INTEGER_T);

    igs_service_init("addImageFromUrl", Whiteboard_addImageFromUrlCallback, this);
    igs_service_arg_add("addImageFromUrl", "url", IGS_STRING_T);
    igs_service_arg_add("addImageFromUrl", "x", IGS_DOUBLE_T);
    igs_service_arg_add("addImageFromUrl", "y", IGS_DOUBLE_T);
    igs_service_reply_add("addImageFromUrl", "elementCreated");
    igs_service_reply_arg_add("addImageFromUrl", "elementCreated", "elementId", IGS_INTEGER_T);

    igs_service_init("remove", Whiteboard_removeCallback, this);
    igs_service_arg_add("remove", "elementId", IGS_INTEGER_T);
    igs_service_reply_add("remove", "actionResult");
    igs_service_reply_arg_add("remove", "actionResult", "succeeded", IGS_BOOL_T);

    igs_service_init("translate", Whiteboard_translateCallback, this);
    igs_service_arg_add("translate", "elementId", IGS_INTEGER_T);
    igs_service_arg_add("translate", "dx", IGS_DOUBLE_T);
    igs_service_arg_add("translate", "dy", IGS_DOUBLE_T);
    igs_service_reply_add("translate", "actionResult");
    igs_service_reply_arg_add("translate", "actionResult", "succeeded", IGS_BOOL_T);

    igs_service_init("moveTo", Whiteboard_moveToCallback, this);
    igs_service_arg_add("moveTo", "elementId", IGS_INTEGER_T);
    igs_service_arg_add("moveTo", "x", IGS_DOUBLE_T);
    igs_service_arg_add("moveTo", "y", IGS_DOUBLE_T);
    igs_service_reply_add("moveTo", "actionResult");
    igs_service_reply_arg_add("moveTo", "actionResult", "succeeded", IGS_BOOL_T);

    igs_service_init("setStringProperty", Whiteboard_setStringPropertyCallback, this);
    igs_service_arg_add("setStringProperty", "elementId", IGS_INTEGER_T);
    igs_service_arg_add("setStringProperty", "property", IGS_STRING_T);
    igs_service_arg_add("setStringProperty", "value", IGS_STRING_T);
    igs_service_reply_add("setStringProperty", "actionResult");
    igs_service_reply_arg_add("setStringProperty", "actionResult", "succeeded", IGS_BOOL_T);

    igs_service_init("setDoubleProperty", Whiteboard_setDoublePropertyCallback, this);
    igs_service_arg_add("setDoubleProperty", "elementId", IGS_INTEGER_T);
    igs_service_arg_add("setDoubleProperty", "property", IGS_STRING_T);
    igs_service_arg_add("setDoubleProperty", "value", IGS_DOUBLE_T);
    igs_service_reply_add("setDoubleProperty", "actionResult");
    igs_service_reply_arg_add("setDoubleProperty", "actionResult", "succeeded", IGS_BOOL_T);

    igs_service_init("getElementIds", Whiteboard_getElementIdsCallback, this);
    igs_service_reply_add("getElementIds", "elementIds");
    igs_service_reply_arg_add("getElementIds", "elementIds", "jsonArray", IGS_STRING_T);

    igs_service_init("getElements", Whiteboard_getElementsCallback, this);
    igs_service_reply_add("getElements", "elements");
    igs_service_reply_arg_add("getElements", "elements", "jsonArray", IGS_STRING_T);


    connect(this, &Whiteboard::chatMessage, this, &Whiteboard::_chatMessage);
    connect(this, &Whiteboard::chatBot, this, &Whiteboard::_chatBot);
    connect(this, &Whiteboard::clear, this, &Whiteboard::_clear);
    connect(this, &Whiteboard::sendSnapshot, this, &Whiteboard::_sendSnapshot);
    connect(this, &Whiteboard::addImage, this, &Whiteboard::_addImage);
    connect(this, &Whiteboard::addImageFromUrl, this, &Whiteboard::_addImageFromUrl);
    connect(this, &Whiteboard::addText, this, &Whiteboard::_addText);
    connect(this, &Whiteboard::addShape, this, &Whiteboard::_addShape);
    connect(this, &Whiteboard::remove, this, &Whiteboard::_remove);
    connect(this, &Whiteboard::translate, this, &Whiteboard::_translate);
    connect(this, &Whiteboard::moveTo, this, &Whiteboard::_moveTo);
    connect(this, &Whiteboard::setStringProperty, this, &Whiteboard::_setStringProperty);
    connect(this, &Whiteboard::setDoubleProperty, this, &Whiteboard::_setDoubleProperty);
    connect(this, &Whiteboard::getElementIds, this, &Whiteboard::_getElementIds);
    connect(this, &Whiteboard::getElements, this, &Whiteboard::_getElements);
}


Whiteboard::~Whiteboard() = default;


Whiteboard& Whiteboard::instance()
{
    static Whiteboard _instance;
    return _instance;
}


QObject* Whiteboard::qmlSingleton(QQmlEngine* engine, QJSEngine* scriptEngine)
{
    Q_UNUSED(engine)
    Q_UNUSED(scriptEngine)
    return &Whiteboard::instance();
}


void Whiteboard::setlastChatMessage(I2BestTypeDef<QString> value)
{
    igs_output_set_string("lastChatMessage", value.toStdString().c_str());

    if (_lastChatMessage != value)
    {
        _lastChatMessage = value;
        Q_EMIT lastChatMessageChanged();
    }
}


void Whiteboard::setlastAction(I2BestTypeDef<QString> value)
{
    igs_output_set_string("lastAction", value.toStdString().c_str());

    if (_lastAction != value)
    {
        _lastAction = value;
        Q_EMIT lastActionChanged();
    }
}


void Whiteboard::removeElement(GraphicElement* element)
{
    if (element)
    {
        _elementsById.remove(element->uid());
        _elements.remove(element);

        setlastAction(tr("User remove: %1").arg(element->uid()));
        element->deleteLater();
    }
}


void Whiteboard::_chatMessage(const QString& text)
{
    if (auto message = new Message(text))
    {
        _messages.append(message);
        setlastChatMessage(text);
        setlastAction(tr("Input chatMessage: %1").arg(text));
    }
}


void Whiteboard::_chatBot(const QString& agentName, const QString& text)
{
    if (auto message = new Message(agentName, text))
    {
        _messages.append(message);
        setlastAction(tr("Service chat: %1").arg(text));
    }
}


void Whiteboard::_clear()
{
    _messages.deleteAllItems();
    _elementsById.clear();
    _elements.deleteAllItems();
    GraphicElement::resetUidCounter();
}


void Whiteboard::_sendSnapshot(const QString& agentUUID, const QString& token)
{
    if (!_qmlItem)
    {
        qWarning() << "snaphsot warning: root QML item is not defined";
        return;
    }

    if (auto grabResult = _qmlItem->grabToImage(_qmlItem->size().toSize()); !grabResult.isNull())
    {
        QObject::connect(grabResult.data(), &QQuickItemGrabResult::ready, this, [=]() {
            if (!agentUUID.isEmpty())
            {
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

                        igs_service_call(agentUUID.toStdString().c_str(), "snapshotResult",
                                         &arguments, (token.isEmpty() ? nullptr : token.toStdString().c_str()));
                    }
                    else
                        qWarning() << "snapshot warning: failed to take a snapshot of our QML item";
                }
                else
                    qWarning() << "snapshot warning: failed to take a snapshot of our QML item";
            }

            if (grabResult.data())
                grabResult.data()->disconnect();
        });

        setlastAction(tr("Service snapshot"));
    }
}


void Whiteboard::_addImage(qreal x, qreal y, qreal width, qreal height, const QByteArray& data, const QString& agentUUID, const QString& token)
{
    if (auto futureWatcher = new QFutureWatcher<QString>())
    {
        connect(futureWatcher, &QFutureWatcher<QString>::finished, this, [=]() {
            auto future = futureWatcher->future();

            if (auto element = new ImageElement())
            {
                element->setx(x);
                element->sety(y);
                element->setwidth(width);
                element->setheight(height);
                element->setsource(future.result());

                _elements.append(element);
                _elementsById.insert(element->uid(), element);

                setlastAction(tr("Service addImage: %1, %2, %3, %4").arg(QString::number(x), QString::number(y), QString::number(width), QString::number(height)));
                _sendElementCreated(element->uid(), agentUUID, token);
            }

            futureWatcher->deleteLater();
        });

        futureWatcher->setFuture(QtConcurrent::run(&Whiteboard::_convertToBase64PngImageSource, this, data));
    }
}


void Whiteboard::_addImageFromUrl(qreal x, qreal y, const QString& url, const QString& agentUUID, const QString& token)
{
    if (auto element = new ImageElement())
    {
        element->setx(x);
        element->sety(y);
        element->setsource(url);

        _elements.append(element);
        _elementsById.insert(element->uid(), element);

        setlastAction(tr("Service addImageFromUrl: %1, %2, %3").arg(url, QString::number(x), QString::number(y)));
        _sendElementCreated(element->uid(), agentUUID, token);
    }
}


void Whiteboard::_addText(qreal x, qreal y, const QString& text, const QColor& color, const QString& agentUUID, const QString& token)
{
    if (auto element = new TextElement())
    {
        element->setx(x);
        element->sety(y);
        element->settext(text);
        element->setcolor(color);

        _elements.append(element);
        _elementsById.insert(element->uid(), element);

        setlastAction(tr("Service addText: %1, %2, %3, %4").arg(QString::number(x), QString::number(y), text, color.name()));
        _sendElementCreated(element->uid(), agentUUID, token);
    }
}


void Whiteboard::_addShape(const QString& type, qreal x, qreal y, qreal width, qreal height, const QColor& fill, const QColor& stroke, qreal strokeWidth, const QString& agentUUID, const QString& token)
{
    bool isRectangle = (QString::compare(type, "rectangle", Qt::CaseInsensitive) == 0);
    bool isEllipse = (QString::compare(type, "ellipse", Qt::CaseInsensitive) == 0);
    if (isRectangle || isEllipse)
    {
        ShapeElement* element {};
        if (isRectangle)
            element = new RectangleElement();
        else if (isEllipse)
            element = new EllipseElement();

        if (element)
        {
            element->setx(x);
            element->sety(y);
            element->setwidth(width);
            element->setheight(height);
            element->setfill(fill);
            element->setstroke(stroke);
            element->setstrokeWidth(strokeWidth);

            _elements.append(element);
            _elementsById.insert(element->uid(), element);

            setlastAction(tr("Service addShape: %1, %2, %3, %4, %5, %6, %7, %8").arg(type, QString::number(x), QString::number(y),
                                                                     QString::number(width), QString::number(height),
                                                                     fill.name(), stroke.name(), QString::number(strokeWidth)));
            _sendElementCreated(element->uid(), agentUUID, token);
        }
    }
    else
        qWarning() << "addShape warning: unknown type" << type;
}


void Whiteboard::_remove(int elementUID, const QString& agentUUID, const QString& token)
{
    bool succeeded {};

    if (auto element = _elementsById.take(elementUID))
    {
        _elements.remove(element);
        element->deleteLater();
        succeeded = true;
    }
    else
        qWarning() << "translate warning: element" << elementUID << "does not exist";

    setlastAction(tr("Service remove: %1").arg(elementUID));
    _sendActionResult(succeeded, agentUUID, token);
}


void Whiteboard::_translate(int elementUID, qreal dx, qreal dy, const QString& agentUUID, const QString& token)
{
    bool succeeded {};

    if (auto element = _elementsById.value(elementUID, nullptr))
    {
        element->translate(dx, dy);
        succeeded = true;
    }
    else
        qWarning() << "translate warning: element" << elementUID << "does not exist";

    setlastAction(tr("Service translate: %1, %2, %3").arg(QString::number(elementUID), QString::number(dx), QString::number(dy)));
    _sendActionResult(succeeded, agentUUID, token);
}


void Whiteboard::_moveTo(int elementUID, qreal x, qreal y, const QString& agentUUID, const QString& token)
{
    bool succeeded {};

    if (auto element = _elementsById.value(elementUID, nullptr))
    {
        element->moveTo(x, y);
        succeeded = true;
    }
    else
        qWarning() << "moveTo warning: element" << elementUID << "does not exist";

    setlastAction(tr("Service moveTo: %1, %2, %3").arg(QString::number(elementUID), QString::number(x), QString::number(y)));
    _sendActionResult(succeeded, agentUUID, token);
}


void Whiteboard::_setStringProperty(int elementUID, const QString& property, const QString& value, const QString& agentUUID, const QString& token)
{
    bool succeeded {};

    if (auto element = _elementsById.value(elementUID, nullptr))
        succeeded = element->setStringProperty(property, value);
    else
        qWarning() << "setStringProperty warning: element" << elementUID << "does not exist";


    setlastAction(tr("Service setStringProperty: %1, %2, %3").arg(QString::number(elementUID), property, value));
    _sendActionResult(succeeded, agentUUID, token);
}


void Whiteboard::_setDoubleProperty(int elementUID, const QString& property, qreal value, const QString& agentUUID, const QString& token)
{
    bool succeeded {};

    if (auto element = _elementsById.value(elementUID, nullptr))
        succeeded = element->setDoubleProperty(property, value);
    else
        qWarning() << "setDoubleProperty warning: element" << elementUID << "does not exist";

    setlastAction(tr("Service setDoubleProperty: %1, %2, %3").arg(QString::number(elementUID), property, QString::number(value)));
    _sendActionResult(succeeded, agentUUID, token);
}


void Whiteboard::_sendElementCreated(int elementUid, const QString& agentUUID, const QString& token)
{
    if (!agentUUID.isEmpty())
    {
        igs_service_arg_t* arguments {};
        igs_service_args_add_int(&arguments, elementUid);

        igs_service_call(agentUUID.toStdString().c_str(), "elementCreated",
                         &arguments, (token.isEmpty() ? nullptr : token.toStdString().c_str()));
    }
}


void Whiteboard::_sendActionResult(bool succeeded, const QString& agentUUID, const QString& token)
{
    if (!agentUUID.isEmpty())
    {
        igs_service_arg_t* arguments {};
        igs_service_args_add_bool(&arguments, succeeded);

        igs_service_call(agentUUID.toStdString().c_str(), "actionResult",
                         &arguments, (token.isEmpty() ? nullptr : token.toStdString().c_str()));
    }
}


void Whiteboard::_getElementIds(const QString& agentUUID, const QString& token)
{
    setlastAction(QObject::tr("Service getElementIds"));

    if (!agentUUID.isEmpty())
    {
        QJsonArray jsonArray;
        std::for_each(_elements.cbegin(), _elements.cend(), [&jsonArray](auto element) {
            if (element)
                jsonArray.append(QJsonValue(element->uid()));
        });

        QJsonDocument json;
        json.setArray(jsonArray);
        auto jsonString = json.toJson(QJsonDocument::Compact);

        igs_service_arg_t* arguments {};
        igs_service_args_add_string(&arguments, jsonString.toStdString().c_str());

        igs_service_call(agentUUID.toStdString().c_str(), "elementIds",
                         &arguments, (token.isEmpty() ? nullptr : token.toStdString().c_str()));
    }
}


void Whiteboard::_getElements(const QString& agentUUID, const QString& token)
{
    setlastAction(QObject::tr("Service getElements"));

    if (!agentUUID.isEmpty())
    {
        QJsonArray jsonArray;
        std::for_each(_elements.cbegin(), _elements.cend(), [&jsonArray](auto element) {
            if (element)
                jsonArray.append(element->toJson());
        });

        QJsonDocument json;
        json.setArray(jsonArray);
        auto jsonString = json.toJson(QJsonDocument::Compact);

        igs_service_arg_t* arguments {};
        igs_service_args_add_string(&arguments, jsonString.toStdString().c_str());

        igs_service_call(agentUUID.toStdString().c_str(), "elements",
                         &arguments, (token.isEmpty() ? nullptr : token.toStdString().c_str()));
    }
}


QString Whiteboard::_convertToBase64PngImageSource(const QByteArray& base64Data)
{
    auto image = QImage::fromData(QByteArray::fromBase64(base64Data));
    if (!image.isNull())
    {
        QByteArray data;
        QBuffer buffer(&data);
        image.save(&buffer, "PNG");

        QString imageSource {"data:image/png;base64,"};
        imageSource.append(QString::fromLatin1(data.toBase64().data()));

        return imageSource;
    }
    else
        return {};
}




