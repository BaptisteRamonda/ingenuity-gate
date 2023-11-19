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

#ifndef QMLINSTRUMENT_H
#define QMLINSTRUMENT_H

#include <QObject>
#include <QRegularExpression>
#include <QQmlEngine>

class SearchElement
{

public:
    enum SearchElementType {STANDARD, ANY, PARENT, INDEX, ATTACHED_INDEX};

    bool valid();

    const QString &value() const;
    void setValue(const QString &value);

    const QRegularExpression &valueRegex() const;
    void setValueRegex(const QRegularExpression &valueRegex);

    bool recursive() const;
    void setRecursive(bool recursive);

    bool component() const;
    void setComponent(bool component);

    SearchElementType type() const;
    void setType(SearchElementType type);

    int index() const;
    void setIndex(int index);

    QString toString() const;

private:
    QString _value;
    QRegularExpression _valueRegex;
    bool _recursive {false};
    bool _component {false};
    SearchElementType _type {STANDARD};
};

class Command
{

public:
    explicit Command();
    explicit Command(const QString& command, const QString& elementPath);

    bool valid() const;
    void setValid(bool valid);
    const QString &errorMessage() const;
    void setErrorMessage(const QString &errorMessage);

    QList<SearchElement> &elements();
    QMap<QString, QVariant> &actions();
    QString toString();
    QString elementPath();

    bool operator==(const Command& other);

private:
    QList<SearchElement> _elements;
    bool _valid{false};
    QString _errorMessage;
    QMap<QString, QVariant> _actions;
    QString _command;
    QString _elementPath;
};

class QmlInstrument : public QObject
{
    Q_OBJECT

public:

    QmlInstrument(QObject *rootObject = nullptr, QObject* parent = nullptr);

    QObject *rootObject() const;
    void setRootObject(QObject *rootObject);

Q_SIGNALS:
    void launchCommand(const QString& command);

private Q_SLOTS:
    void _onLaunchCommand(const QString& command);

private:
    Command _buildCommand(const QString& command);
    void _searchElements(const QList<SearchElement> & elements, int searchElementIndex, const QList<QObject *>& rootObjects);
    void _executeCommand(Command &cmd);

    QList<QObject*> _getComponentElements(QObject* root, const QString& value, Qt::FindChildOption findOption);

    QObject* _getElementByIndex(QObject *root,
                                 const QString &value,
                                 SearchElement::SearchElementType type,
                                 const QList<QObject*>& rootObjects);


    QList<QObject*> _getElementsByValue(QObject *root,
                                 const QString &value,
                                 const QRegularExpression& valueRegex,
                                 Qt::FindChildOption findChildOption,
                                 bool isFirst);


    QObject* _rootObject {};
    QList<QObject*> _objects;
    Command _lastCommand;
    qint64 _lastCommandTime {-1};
};

#endif // QMLINSTRUMENT_H


