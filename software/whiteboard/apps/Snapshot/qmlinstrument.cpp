/*
 *	clock
 *
 *  Copyright © 2022 Ingenuity i/o. All rights reserved.
 *
 *	See license terms for the rights and conditions
 *	defined by copyright holders.
 *
 *
 *	Contributors:
 *      Ingescape Circle <...@ingenuity.io>
 *
 */

#include "QtCore/qdatetime.h"
#include "QtCore/qstringliteral.h"

#if defined(INGESCAPE_FROM_PRI)
    #include "ingescape.h"
#else
    #include <ingescape/ingescape.h>
#endif // INGESCAPE_FROM_PRI

#include "qmlinstrument.h"


// =========================
// === SEARCH ELEMENT CLASS
// =========================

bool SearchElement::valid()
{
    return (!_value.isEmpty() || !_valueRegex.pattern().isEmpty() || _type == SearchElement::ANY
            || _type == SearchElement::PARENT);
}

const QString &SearchElement::value() const
{
    return _value;
}

void SearchElement::setValue(const QString &value)
{
    _value = value;
}

const QRegularExpression &SearchElement::valueRegex() const
{
    return _valueRegex;
}

void SearchElement::setValueRegex(const QRegularExpression &valueRegex)
{
    _valueRegex = valueRegex;
}

bool SearchElement::recursive() const
{
    return _recursive;
}

void SearchElement::setRecursive(bool recursive)
{
    _recursive = recursive;
}

bool SearchElement::component() const
{
    return _component;
}

void SearchElement::setComponent(bool component)
{
    _component = component;
}

SearchElement::SearchElementType SearchElement::type() const
{
    return _type;
}

void SearchElement::setType(SearchElementType type)
{
    _type = type;
}

QString SearchElement::toString() const
{
    QString type = (_type == SearchElement::STANDARD) ? "Standard" :  (_type == SearchElement::ANY) ? "Any"
                   : (_type == SearchElement::PARENT) ? "Parent" : (_type == SearchElement::INDEX) ? "Index" : "Attached index";

    return QString("{value: %1; valueRegex: %2; isRecursive: %3; isComponent: %4; type: %5;}")
            .arg(_value,
                 _valueRegex.pattern(),
                 (_recursive ? "true" : "false"),
                 (_component ? "true" : "false"),
                 type);
}

// =========================
// === COMMAND CLASS
// =========================

Command::Command() : _valid{false}
{

}

Command::Command(const QString &command, const QString &elementPath) :
    _valid{false}
  , _command(command)
  , _elementPath(elementPath)
{

}

bool Command::valid() const
{
    return _valid;
}

void Command::setValid(bool valid)
{
    _valid = valid;
}

const QString &Command::errorMessage() const
{
    return _errorMessage;
}

void Command::setErrorMessage(const QString &errorMessage)
{
    _errorMessage = errorMessage;
}

QList<SearchElement> &Command::elements()
{
    return _elements;
}

QMap<QString, QVariant> &Command::actions()
{
    return _actions;
}

QString Command::elementPath()
{
    return _elementPath;
}

QString Command::toString()
{
    return _command;
}

bool Command::operator==(const Command &other)
{
    return _elementPath == other._elementPath;
}

// =========================
// === INGESCAPE CALLBACK
// =========================

void commandCallback(igs_iop_type_t iopType, const char* name, igs_iop_value_type_t valueType,
                        void* value, size_t valueSize, void* myData)
{
    Q_UNUSED(iopType)
    Q_UNUSED(name)
    Q_UNUSED(valueType)
    Q_UNUSED(valueSize)

    auto command = QString((char *)value);

    if(auto qmlInstrument = static_cast<QmlInstrument*>(myData); qmlInstrument && !command.isEmpty())
        Q_EMIT qmlInstrument->launchCommand(command);
}


// =========================
// === QML INSTRUMENT CLASS
// =========================

constexpr int MAX_MS_SINCE_LAST_COMMAND = 1000;
const QString PARENT_SYNTAX = u".."_qs;
const QString COMPONENT_SYNTAX = u"#"_qs;
const QString PATH_SEPARATOR_SYNTAX = u"/"_qs;
const QString ACTION_SEPARATOR_SYNTAX = u"="_qs;
const QString ALL_SYNTAX = u"*"_qs;

QmlInstrument::QmlInstrument(QObject *rootObject, QObject *parent) :
    QObject(parent)
  , _rootObject(rootObject)
{
    connect(this, &QmlInstrument::launchCommand, this, &QmlInstrument::_onLaunchCommand);

    igs_input_create("ui_command", IGS_STRING_T, nullptr, 0);
    igs_observe_input("ui_command", commandCallback, this);

    igs_output_create("ui_error", IGS_STRING_T, nullptr, 0);
}

void QmlInstrument::_onLaunchCommand(const QString &command)
{
    auto newCommand = _buildCommand(command);
    auto now = QDateTime::currentMSecsSinceEpoch();
    auto elapsedTimeSinceLastCommand = now - _lastCommandTime;

    // Use last object found if last command and
    if(_lastCommandTime >= 0 && elapsedTimeSinceLastCommand < MAX_MS_SINCE_LAST_COMMAND && _lastCommand.valid() && _lastCommand == newCommand)
    {
        if(!_objects.isEmpty())
            _executeCommand(newCommand);
    }
    else if(newCommand.valid())
    {
        _lastCommand = newCommand;
        _objects.clear();
        _searchElements(newCommand.elements(), 0, {_rootObject});

        if(!_objects.isEmpty())
            _executeCommand(newCommand);
    }
    else
        igs_output_set_string("ui_error", newCommand.errorMessage().toStdString().c_str());

    _lastCommandTime = now;
}

Command QmlInstrument::_buildCommand(const QString &command)
{
    auto formattedCommand = command;
    formattedCommand.replace("= ", ACTION_SEPARATOR_SYNTAX);
    formattedCommand.replace(" =", ACTION_SEPARATOR_SYNTAX);

    auto commandParts = formattedCommand.split(" ");
    auto elementsPath = commandParts.at(0);
    auto elements = elementsPath.split(PATH_SEPARATOR_SYNTAX);
    auto commandResult = Command(formattedCommand, elementsPath);

    // check somes basic cases (empty command, no "/" at the begining)
    if(formattedCommand.isEmpty() || elements.count() < 2 || (elements.count() > 0 && !elements.at(0).isEmpty()))
        return commandResult;

    bool nextIsRecursive {false};

    // handle elements selection, first element must be empty so we begin to i=1
    for (int i = 1; i < elements.count(); i++)
    {
        auto element = elements.at(i);
        SearchElement searchElement;
        SearchElement attachedIndexElement;
        attachedIndexElement.setType(SearchElement::ATTACHED_INDEX);

        if(element.isEmpty())
        {
            if(i == 1)
            {
                nextIsRecursive = true; // case where we have "//" at the begining
                continue;
            }
            else // case where we have "//" at the middle => invalid case
            {
                auto errorMsg = QString("Invalid command, '//' syntax is only working at the begining of the command, {command: \"%1\"}.").arg(formattedCommand);
                qWarning() << QString("[Qml instrument] %1").arg(errorMsg);
                commandResult.setErrorMessage(errorMsg);
                return commandResult;
            }
        }
        else if(element == PARENT_SYNTAX)
        {
            searchElement.setType(SearchElement::PARENT);
            if(i == (elements.count()-1)) // case where we have ".." at the end => invalid case
            {
                auto errorMsg = QString("Invalid command, parent type syntax ('..') at the end of the command is not possible, {command: \"%1\"}.").arg(formattedCommand);
                qWarning() << QString("[Qml instrument] %1").arg(errorMsg);
                commandResult.setErrorMessage(errorMsg);
                return commandResult;
            }
        }
        else if(element == ALL_SYNTAX)
        {
            if(nextIsRecursive)
            {
                searchElement.setRecursive(true);
                nextIsRecursive = false;
            }
            searchElement.setType(SearchElement::ANY);
        }
        else
        {
            if(nextIsRecursive)
            {
                searchElement.setRecursive(true);
                nextIsRecursive = false;
            }

            // component case
            if(element.left(1) == QString(COMPONENT_SYNTAX))
            {
                element = element.right(element.count()-1);
                searchElement.setComponent(true);
            }

            // index + element name regex
            QRegularExpression indexForNameRegex(R"RX((.*)\[(-?[0-9]{1,})\]$)RX");
            auto indexForNameMatch = indexForNameRegex.match(element);

            // only index
            QRegularExpression indexRegex(R"RX(^\[(-?[0-9]{1,})\]$)RX");
            auto indexMatch = indexRegex.match(element);


            QRegularExpression regexRegex(R"RX((.+\*[^\[\]]*)(\[-?[0-9]{1,}\])?$)RX");
            auto regexMatch = regexRegex.match(element);

            // regex case
            if(regexMatch.hasMatch() && regexMatch.lastCapturedIndex() >= 1)
            {
                auto regexElt = regexMatch.captured(1).replace(ALL_SYNTAX, "."+ALL_SYNTAX);
                searchElement.setValueRegex(QRegularExpression(regexElt));
            }

            // index cases
            if(indexMatch.hasMatch() && indexMatch.lastCapturedIndex() == 1)
            {
                bool convertOk{false};
                int index = indexMatch.captured(1).toInt(&convertOk);
                if(convertOk && index >= 0)
                {
                    searchElement.setValue(QString::number(index));
                    searchElement.setType(SearchElement::INDEX);
                }
                else
                {
                    auto errorMsg = QString("Invalid index found, {index: %1, command: \"%2\"}.").arg(indexMatch.captured(), formattedCommand);
                    qWarning() << QString("[Qml instrument] %1").arg(errorMsg);
                    commandResult.setErrorMessage(errorMsg);
                    return commandResult;
                }
            }
            // index for name case
            else if(indexForNameMatch.hasMatch() && indexForNameMatch.lastCapturedIndex() == 2)
            {
                if(searchElement.valueRegex().pattern().isEmpty())
                    searchElement.setValue(indexForNameMatch.captured(1));

                bool convertOk{false};
                int index = indexForNameMatch.captured(2).toInt(&convertOk);
                if(convertOk && index >= 0)
                {
                    attachedIndexElement.setValue(QString::number(index));
                    attachedIndexElement.setType(SearchElement::ATTACHED_INDEX);
                }
                else
                {
                    auto errorMsg = QString("Invalid index found, {index: %1, command: \"%2\"}.").arg(indexMatch.captured(), formattedCommand);
                    qWarning() << QString("[Qml instrument] %1").arg(errorMsg);
                    commandResult.setErrorMessage(errorMsg);
                    return commandResult;
                }
            }
            else if(searchElement.valueRegex().pattern().isEmpty()) // nominal case
                searchElement.setValue(element);
        }

        if(searchElement.valid())
            commandResult.elements().append(searchElement);

        if(attachedIndexElement.valid() && searchElement.type() != SearchElement::INDEX)
            commandResult.elements().append(attachedIndexElement);
    }

    // handle actions
    if(commandParts.count() > 1)
    {
        QString previousActionName;
        for(int i = 1; i < commandParts.count(); i++)
        {
            auto action = commandParts.at(i);
            auto actionRegex = QRegularExpression(R"RX(^(.+)=(.+)$)RX");
            auto actionMatch = actionRegex.match(action);

            if(actionMatch.hasMatch() && actionMatch.lastCapturedIndex() == 2)
            {
                auto value = QVariant(actionMatch.captured(2));
                auto key = actionMatch.captured(1);
                commandResult.actions().insert(key, value);
                previousActionName = key;
            }
            else if(!action.isEmpty() && !previousActionName.isEmpty())
            {
                auto previousValue = commandResult.actions().value(previousActionName).toString();
                previousValue += " " + action;
                commandResult.actions().insert(previousActionName, previousValue);
            }
            else
            {
                auto errorMsg = QString("Invalid action provided into the command. Syntax is \"action=value\" {command: \"%1\"}.").arg(formattedCommand);
                qWarning() << QString("[Qml instrument] %1").arg(errorMsg);
                commandResult.setErrorMessage(errorMsg);
                return commandResult;
            }
        }
    }
    else
    {
        auto errorMsg = QString("No action provided into the command, {command: \"%1\"}.").arg(formattedCommand);
        qWarning() << QString("[Qml instrument] %1").arg(errorMsg);
        commandResult.setErrorMessage(errorMsg);
        return commandResult;
    }

    commandResult.setValid(true);
    return commandResult;
}

void QmlInstrument::_searchElements(const QList<SearchElement> & elements, int searchElementIndex, const QList<QObject *> &rootObjects)
{
    if(searchElementIndex >= 0 && searchElementIndex < elements.count())
    {
        auto currentElt = elements.at(searchElementIndex);
        auto isLast = (searchElementIndex == (elements.count()-1));
        auto isFirst = (searchElementIndex == 0);
        auto isRecursive = currentElt.recursive();
        auto isComponent = currentElt.component();
        auto type = currentElt.type();
        auto value = currentElt.value();
        auto valueRegex = currentElt.valueRegex();
        auto findChildOption = isRecursive ? Qt::FindChildrenRecursively : Qt::FindDirectChildrenOnly;

        for(auto root : rootObjects)
        {
            if(root != nullptr)
            {
                if(isLast)
                {
                    QList<QObject*> objects;

                    if(type == SearchElement::INDEX || type == SearchElement::ATTACHED_INDEX)
                    {
                        objects = {_getElementByIndex(root, value, type, rootObjects)};
                    }
                    else if(isComponent && !value.isEmpty())
                    {
                        objects = _getComponentElements(root, value, findChildOption);
                    }
                    else if(!value.isEmpty() || !valueRegex.pattern().isEmpty())
                    {
                       objects = _getElementsByValue(root, value, valueRegex, findChildOption, isFirst);
                    }
                    else if(currentElt.type() == SearchElement::ANY)
                    {
                        objects = root->findChildren<QObject*>(QString(), findChildOption);
                    }

                    // appends all the objects found
                    for( auto obj : objects)
                    {
                        if(obj != nullptr && !_objects.contains(obj) && !obj->objectName().isEmpty())
                            _objects.append(obj);
                    }

                }
                else if(type == SearchElement::INDEX || type == SearchElement::ATTACHED_INDEX)
                {
                    if(auto indexedElement = _getElementByIndex(root, value, type, rootObjects))
                    {
                        searchElementIndex++;
                        _searchElements(elements, searchElementIndex, {indexedElement});
                    }
                }
                else if(isComponent && !value.isEmpty())
                {
                    if(auto objects = _getComponentElements(root, value, findChildOption); !objects.isEmpty())
                    {
                        searchElementIndex++;
                        _searchElements(elements, searchElementIndex, objects);
                    }
                }
                else if(!value.isEmpty() || !valueRegex.pattern().isEmpty())
                {
                   auto objectsFound = _getElementsByValue(root, value, valueRegex, findChildOption, isFirst);
                   searchElementIndex++;
                   _searchElements(elements, searchElementIndex, objectsFound);
                }
                else if(currentElt.type() == SearchElement::ANY)
                {
                    searchElementIndex++;
                    auto objectsFound = root->findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly);
                    _searchElements(elements, searchElementIndex, objectsFound);
                }
                else if(currentElt.type() == SearchElement::PARENT)
                {
                    searchElementIndex++;
                    _searchElements(elements, searchElementIndex, {root->parent()});
                }
            }
        }
    }
    else
        QString("[Qml instrument] Unable to perform research.");
}

void QmlInstrument::_executeCommand(Command &cmd)
{
    for(auto obj : _objects)
    {
        if(obj != nullptr)
        {
            for(auto actionKey : cmd.actions().keys())
            {
                auto value = cmd.actions().value(actionKey);
                if(!obj->setProperty(actionKey.toStdString().c_str(), value))
                    qWarning() << QString("[Qml instrument] Unable to performe action, {name: %1, value: \"%2\", command: \"%3\"}.")
                                  .arg(actionKey, value.toString(), cmd.toString());
            }
        }
    }
}

QList<QObject *> QmlInstrument::_getComponentElements(QObject *root, const QString &value, Qt::FindChildOption findOption)
{
    Q_ASSERT(root);

    QList<QObject*> objects;
    auto allChildren = root->findChildren<QObject*>(QString(), findOption);

    for(auto obj : allChildren)
    {
        if(obj != nullptr)
        {
            if(QString(obj->metaObject()->className()).contains(value))
                objects.append(obj);
        }
    }

    return objects;
}

QObject * QmlInstrument::_getElementByIndex(QObject *root,
                                              const QString &value,
                                              SearchElement::SearchElementType type,
                                              const QList<QObject*>& rootObjects)
{
    Q_ASSERT(root);

    QObject * indexedElement {nullptr};
    bool ok;
    auto index = value.toInt(&ok);

    if(type == SearchElement::ATTACHED_INDEX)
    {
        if(ok && index >= 0 && index < rootObjects.size())
            indexedElement = rootObjects.at(index);
    }
    else
    {
        QList<QObject*> objectsWithName;
        for(auto obj : root->findChildren<QObject*>(QString(), Qt::FindDirectChildrenOnly))
        {
            if(obj != nullptr && !obj->objectName().isEmpty())
                objectsWithName.append(obj);
        }

        if(ok && index >= 0 && index < objectsWithName.size())
            indexedElement = objectsWithName.at(index);
    }

    return indexedElement;
}

QList<QObject*> QmlInstrument::_getElementsByValue(QObject *root,
                                            const QString &value,
                                            const QRegularExpression& valueRegex,
                                            Qt::FindChildOption findChildOption,
                                            bool isFirst)
{
    Q_ASSERT(root);

    QList<QObject *> elements;

    if(!valueRegex.pattern().isEmpty())
        elements = root->findChildren<QObject*>(valueRegex, findChildOption);
    else if(!value.isEmpty())
        elements = root->findChildren<QObject*>(value, findChildOption);
    if(isFirst && elements.isEmpty()  && !root->objectName().isEmpty()
       && (root->objectName() == value || (!valueRegex.pattern().isEmpty()  && valueRegex.match(root->objectName()).hasMatch())))
    {
        elements = {root};
    }

    return elements;
}

QObject *QmlInstrument::rootObject() const
{
    return _rootObject;
}

void QmlInstrument::setRootObject(QObject *rootObject)
{
    _rootObject = rootObject;
}


