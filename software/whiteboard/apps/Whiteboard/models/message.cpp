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


#include "message.h"


Message::Message(const QString& text, QObject *parent)
    : QObject{parent}
    , _fromChatBot(false)
    , _text(text)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


Message::Message(const QString& chatName, const QString& text, QObject* parent)
    : QObject{parent}
    , _fromChatBot(true)
    , _chatBotName(chatName)
    , _text(text)
{
    QQmlEngine::setObjectOwnership(this, QQmlEngine::CppOwnership);
}


Message::~Message() = default;
