#include "qkapihandler.h"

#include "qkconnectionmanager.h"
#include "qk.h"

#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include <QFile>
#include <QStringList>
#include <QRegularExpression>
#include <QSignalMapper>

QkAPIHandler::QkAPIHandler(QkConnectionManager *connManager, QObject *parent) :
    QObject(parent),
    m_connManager(connManager)
{
    m_realTimeEnabled = true;
    m_apiTreeRoot = 0;
    create();
    setupConnections();
}

void QkAPIHandler::setRealTime(bool enabled)
{
    m_realTimeEnabled = enabled;
}

void QkAPIHandler::handleJsonRpc(const QString &jsonRpc)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonRpc.toUtf8(), &parseError);
    if(parseError.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return;
    }
    if(!doc.isObject())
    {
        qDebug() << "JSON document is not an object";
        return;
    }

    QJsonObject obj = doc.object();
    QVariantMap map = obj.toVariantMap();
    qDebug() << map;
    QString methodName = map["method"].toString();
    QVariantList params = map["params"].toList();
    if(params.count() == 0) {
        qDebug() << "JSON RPC cannot have zero parameters";
        return;
    }
    QString path = params.takeFirst().toString();

    QJsonDocument jsonDoc;
    jsonDoc.setObject(run(methodName, path, params));
    emit sendJson(jsonDoc);
}

QJsonObject QkAPIHandler::run(const QString &methodName, const QString &path, QVariantList params)
{
    QStringList resourceList = path.split('/', QString::SkipEmptyParts);
    APITreeItem *nextTreeItem, *curTreeItem = m_apiTreeRoot;

    QMap<QString,QVariant> parsedValues;

    qDebug() << "resourceList:" << resourceList;

    QString resource = resourceList.takeFirst();
    if(resource != curTreeItem->resource)
    {
        qDebug() << "The first RPC name must be \"qk\"," << resource << curTreeItem->resource;
        return rpc_error(RPCError_Qk, "The first name must be qk");
    }

    while(!resourceList.isEmpty())
    {
        resource = resourceList.takeFirst();
        nextTreeItem = 0;

        // Try to find a child with the same resource name
        foreach(APITreeItem *child, curTreeItem->children)
        {
            qDebug() << child->resource << resource;
            if(child->resource == resource)
            {
                nextTreeItem = child;
                break;
            }
        }
        // Try to get a value from resource
        if(nextTreeItem == 0)
        {
            foreach(APITreeItem *child, curTreeItem->children)
            {
                qDebug() << child->resource << resource;
                if(child->resource.contains(QRegularExpression("\\{.*?\\}")))
                {
                    parsedValues.insert(child->resource, QVariant(resource));
                    qDebug() << "parsedValues" << parsedValues;
                    nextTreeItem = child;
                    break;
                }
            }
        }

        if(nextTreeItem != 0)
            curTreeItem = nextTreeItem;
        else
        {
            qDebug() << "Parsing error: resource not found";
            return rpc_error(RPCError_Qk, "Resource name not found: " + resource);
        }
    }

    // Check if requested method type is supported by resource
    if((methodName == "get" && (curTreeItem->attributes & GET) == 0) ||
       (methodName == "set" && (curTreeItem->attributes & SET) == 0))
    {
        return rpc_error(RPCError_Qk, "Unsupported method name");
    }

    QString rpcName = curTreeItem->rpcName;
    qDebug() << "call RPC:" << curTreeItem->rpcName;
    if(!m_rpcMapper.contains(rpcName))
    {
        qDebug() << "RPC mapper doesn't contain" << rpcName;
        return rpc_error(RPCError_Qk, "RPC mapper doesn't have " + rpcName + " (internal)");
    }

    RPCArgs rpcArgs;
    rpcArgs.parsedValues = &parsedValues;
    rpcArgs.params = &params;

    RPC rpc = m_rpcMapper.value(rpcName);
    return (*this.*rpc)(&rpcArgs);
}


void QkAPIHandler::create()
{
    QFile file(":/api/qkapi.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
       qDebug() << "Unable to read qkapi.txt";
       return;
    }

    QTextStream in(&file);
    QString line;
    int level, maxLevel = 0;
    while(!in.atEnd())
    {
        line = in.readLine();
        level = line.count('\t');
        if(level > maxLevel)
            maxLevel = level;
    }

    APITreeItem *treeItem, *parentItem;
    QVector<APITreeItem*> parentArray(maxLevel);
    in.seek(0);
    while(!in.atEnd())
    {
        line = in.readLine();

        if(line.at(0) == '#') // comment
            continue;

        QStringList cols = line.split(',');
        if(cols.count() != APITreeColumns)
        {
            qDebug() << "qkapi.txt: wrong number of columns:" << cols.count() << "should be:" << APITreeColumns;
            return;
        }

        QString resource = cols[0];
        QString attrStr = cols[1].remove(QRegularExpression("\\t|\\s"));
        QString rpcName = cols[2].remove(QRegularExpression("\\t|\\s"));

        level = resource.count('\t');
        resource.remove('\\');
        resource.remove(QRegularExpression("\\t|\\s"));

        if(level == 0)
        {
            if(m_apiTreeRoot == 0)
            {
                treeItem = new APITreeItem(resource, rpcName);
                treeItem->setAttributesFromString(attrStr);
                m_apiTreeRoot = treeItem;
            }
            else
            {
                qDebug() << "ERROR: multiple items at level 0";
                return;
            }
        }
        else
        {
            parentItem = parentArray.at(level - 1);
            if(parentItem == 0)
                qDebug() << "ERROR: parent is null";
            treeItem = new APITreeItem(resource, rpcName, parentItem);
            treeItem->setAttributesFromString(attrStr);
            parentItem->children.append(treeItem);
        }

        parentArray[level] = treeItem;
        qDebug() << level << treeItem->resource;
    }
    file.close();

    m_rpcMapper.insert("listQk",            &QkAPIHandler::rpc_listQk);
    m_rpcMapper.insert("listConns",         &QkAPIHandler::rpc_listConns);
    m_rpcMapper.insert("conn",              &QkAPIHandler::rpc_conn);
    m_rpcMapper.insert("listNodes",         &QkAPIHandler::rpc_listNodes);
    m_rpcMapper.insert("samplingFrequency", &QkAPIHandler::rpc_samplingFrequency);
    m_rpcMapper.insert("samplingMode",      &QkAPIHandler::rpc_samplingMode);
    m_rpcMapper.insert("updateDevice",      &QkAPIHandler::rpc_updateDevice);
    m_rpcMapper.insert("listCmds",          &QkAPIHandler::rpc_listCmds);
    m_rpcMapper.insert("search",            &QkAPIHandler::rpc_search);
    m_rpcMapper.insert("start",             &QkAPIHandler::rpc_start);
    m_rpcMapper.insert("stop",              &QkAPIHandler::rpc_stop);
    m_rpcMapper.insert("listSubscriptions", &QkAPIHandler::rpc_listSubscriptions);
    m_rpcMapper.insert("subData",           &QkAPIHandler::rpc_subData);
    m_rpcMapper.insert("subEvent",          &QkAPIHandler::rpc_subEvent);
    m_rpcMapper.insert("subDebug",          &QkAPIHandler::rpc_subDebug);
}

QJsonObject QkAPIHandler::rpc_error(int code, const QString &message)
{
    qDebug() << "rpc_error";
    QJsonObject obj, valueObj, errObj;
    valueObj.insert("value", QJsonValue(255));
    errObj.insert("code", QJsonValue(code));
    errObj.insert("message", QJsonValue(message));
    valueObj.insert("error",errObj);
    obj.insert("result", QJsonValue(valueObj));
    return obj;
}

QJsonObject QkAPIHandler::rpc_result(int value)
{
    qDebug() << "rpc_result";
    QJsonObject obj, valueObj;
    valueObj.insert("value", QJsonValue(value));
    obj.insert("result", QJsonValue(valueObj));
    return obj;
}

QJsonObject QkAPIHandler::rpc_listQk(RPCArgs *args)
{
    qDebug() << "rpc_listQk";

    QJsonObject obj;
    obj.insert("conns", QJsonValue(rpc_listConns(args)));
    obj.insert("nodes", QJsonValue(rpc_listNodes(args)));
    return obj;
}

QJsonObject QkAPIHandler::rpc_listConns(RPCArgs *args)
{
    qDebug() << "rpc_listConns";

    int i,j;
    (void)args; // not used
    QList<QkConnection::Descriptor> connDesc;

    foreach(QkConnection *conn, m_connManager->connections())
        connDesc.append(conn->descriptor);

    QJsonObject obj;
    QJsonArray connsArray, paramsArray;
    QJsonObject connObj;
    for(i = 0; i < connDesc.count(); i++)
    {
        QkConnection::Descriptor desc = connDesc[i];
        connObj.insert("type", QJsonValue(QkConnection::typeToString(desc.type)));

        for(j = 0; j < desc.params.count(); j++)
            paramsArray.insert(j, QJsonValue(desc.params[j]));
        connObj.insert("params", QJsonValue(paramsArray));

        connsArray.insert(i, QJsonValue(connObj));
    }
    obj.insert("conns", QJsonValue(connsArray));
    return obj;
}

QJsonObject QkAPIHandler::rpc_conn(RPCArgs *args)
{
    qDebug() << "rpc_conn";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    int i;
    bool ok;
    QString parsedValueStr;
    parsedValueStr = args->parsedValues->value("{connID}").toString();
    int connID = parsedValueStr.toInt(&ok);
    if(!ok)
        return rpc_error(RPCError_Qk, tr("Invalid connID: ") + parsedValueStr);

    QkConnection::Descriptor connDesc;
    if(connID < 0 || connID > m_connManager->connections().count() - 1)
        ok = false;
    else
        connDesc = m_connManager->connections().at(connID)->descriptor;

    if(!ok)
        return rpc_error(RPCError_Qk, tr("Invalid connID: ") + parsedValueStr);

    QJsonObject obj;
    QJsonObject conn;
    conn.insert("type", QJsonValue(QkConnection::typeToString(connDesc.type)));
    for(i = 0; i < connDesc.params.count(); i++)
        conn.insert("param" + QString::number(i), QJsonValue(connDesc.params[i]));

    obj.insert("conn" + QString::number(connID), QJsonValue(conn));

    return obj;
}

QJsonObject QkAPIHandler::rpc_listNodes(RPCArgs *args)
{
    qDebug() << "rpc_listNodes";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    int i;
    QJsonObject obj;

    QkConnection *conn = m_connManager->defaultConnection();

    if(conn != 0)
    {
        QMap<int, QkNode*> nodes = conn->qk->nodes();

        QJsonObject nodesObj;
        QJsonObject nodeObj, commObj, deviceObj;

        foreach(QkNode *node, nodes)
        {
            if(node->module() != 0)
            {
                commObj.insert("name", node->module()->name());
            }
            nodeObj.insert("comm", QJsonValue(commObj));

            if(node->device() != 0)
            {
                deviceObj.insert("name", node->device()->name());
            }
            nodeObj.insert("device", QJsonValue(deviceObj));

            nodesObj.insert(QString::number(node->address()), QJsonObject(nodeObj));
        }

        obj.insert("nodes", QJsonValue(nodesObj));
    }

    return obj;
}

QJsonObject QkAPIHandler::rpc_samplingFrequency(RPCArgs *args)
{
    qDebug() << "rpc_samplingFrequency";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    QMutexLocker locker(m_connManager->mutex());

    bool ok;
    QJsonObject errObj;

    if(!validate_rpc_device(args, &errObj))
        return errObj;

    int addr =  args->parsedValues->value("{addr}").toInt();

    if(args->params->count() != 1)
        return rpc_error(RPCError_Qk, tr("Invalid parameters"));

    QString sampFreqStr = args->params->at(0).toString();
    int sampFreq = sampFreqStr.toInt(&ok);
    if(!ok)
        return rpc_error(RPCError_Qk, tr("Invalid sampling frequency: ") + sampFreqStr);

    QkConnection *conn = m_connManager->defaultConnection();
    conn->qk->node(addr)->device()->setSamplingFrequency(sampFreq);

    return rpc_result(0);
}

QJsonObject QkAPIHandler::rpc_samplingMode(RPCArgs *args)
{
    qDebug() << "rpc_samplingMode";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    QMutexLocker locker(m_connManager->mutex());

    bool ok;
    QJsonObject errObj;

    if(!validate_rpc_device(args, &errObj))
        return errObj;

    int addr =  args->parsedValues->value("{addr}").toInt();

    if(args->params->count() != 1)
        return rpc_error(RPCError_Qk, tr("Invalid parameters"));

    QString modeStr = args->params->at(0).toString();
    QkDevice::SamplingMode mode;
    if(modeStr == "continuous")
        mode = QkDevice::smContinuous;
    else if(modeStr == "triggered")
        mode = QkDevice::smTriggered;
    else
        return rpc_error(RPCError_Qk, tr("Unkown sampling mode:") + modeStr);

    QkConnection *conn = m_connManager->defaultConnection();
    conn->qk->node(addr)->device()->setSamplingMode(mode);

    return rpc_result(0);
}

QJsonObject QkAPIHandler::rpc_updateDevice(RPCArgs *args)
{
    qDebug() << "rpc_updateDevice";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    //QMutexLocker locker(m_connManager->mutex());

    QJsonObject errObj;

    if(!validate_rpc_device(args, &errObj))
        return errObj;

    int addr =  args->parsedValues->value("{addr}").toInt();

    QkConnection *conn = m_connManager->defaultConnection();
    conn->qk->node(addr)->device()->update();
    conn->qk->getNode(addr);

    return errObj;
}

QJsonObject QkAPIHandler::rpc_listCmds(RPCArgs *args)
{
    qDebug() << "rpc_listCmds";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    QJsonObject obj;
    QJsonArray cmdsObj;

    int i;
    QStringList cmds;
    cmds << "search" << "start" << "stop";
    for(i = 0; i < cmds.count(); i++)
        cmdsObj.insert(i, QJsonValue(cmds.at(i)));

    obj.insert("cmds", cmdsObj);

    return obj;
}

QJsonObject QkAPIHandler::rpc_search(RPCArgs *args)
{

}

QJsonObject QkAPIHandler::rpc_start(RPCArgs *args)
{
    //QMutexLocker locker(m_connManager->mutex());

    QJsonObject obj;
    QkConnection *defaultConn = m_connManager->defaultConnection();
    if(defaultConn == 0)
        obj = rpc_error(RPCError_Qk, tr("No connection available"));
    else
    {
        //obj = rpc_result(0);
        defaultConn->qk->start();
    }

    return obj;
}

QJsonObject QkAPIHandler::rpc_stop(RPCArgs *args)
{
    QJsonObject obj;
    QkConnection *defaultConn = m_connManager->defaultConnection();
    if(defaultConn == 0)
        obj = rpc_error(RPCError_Qk, tr("No connection available"));
    else
    {
        //obj = rpc_result(0);
        defaultConn->qk->stop();
    }
    return obj;
}

QJsonObject QkAPIHandler::rpc_listSubscriptions(RPCArgs *args)
{
    qDebug() << "rpc_listSubscriptions";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    QJsonObject obj;
    QJsonArray subs;

    int i;
    QStringList subNames;
    subNames << "data";
    for(i = 0; i < subNames.count(); i++)
        subs.insert(i, QJsonValue(subNames.at(i)));

    obj.insert("subscriptions", subs);

    return obj;
}

QJsonObject QkAPIHandler::rpc_subData(RPCArgs *args)
{
    qDebug() << "rpc_subData";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    bool subscribe = args->params->at(0).toBool();
    if(subscribe)
        m_subscribedNames.insert("data");
    else
        m_subscribedNames.remove("data");

    return rpc_result(0);
}

QJsonObject QkAPIHandler::rpc_subEvent(RPCArgs *args)
{
    qDebug() << "rpc_subEvent";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    bool subscribe = args->params->at(0).toBool();
    if(subscribe)
        m_subscribedNames.insert("event");
    else
        m_subscribedNames.remove("event");

    return rpc_result(0);
}

QJsonObject QkAPIHandler::rpc_subDebug(RPCArgs *args)
{
    qDebug() << "rpc_subDebug";
    qDebug() << *(args->parsedValues);
    qDebug() << *(args->params);

    bool subscribe = args->params->at(0).toBool();
    if(subscribe)
        m_subscribedNames.insert("debug");
    else
        m_subscribedNames.remove("debug");

    return rpc_result(0);
}

QJsonObject QkAPIHandler::rpc_rt_data(RPCArgsRT *args)
{
    qDebug() << "rpc_rt_data";

    QJsonObject obj;

    QkNode *node = args->conn->qk->node(args->address);
    QkDevice *device = node->device();

    QVector<QkDevice::Data> data = device->data();

    QJsonObject dataObj, valueObj;
    QJsonArray valuesArray;

    dataObj.insert("connID", QJsonValue(args->connID));
    dataObj.insert("address", QJsonValue(args->address));
    int i;
    for(i = 0; i < data.count(); i++)
    {
        valueObj.insert("label", QJsonValue(data[i].label()));
        valueObj.insert("value", QJsonValue(data[i].value()));
        valuesArray.insert(i, QJsonValue(valueObj));
    }
    dataObj.insert("values", QJsonValue(valuesArray));
    obj.insert("data", QJsonValue(dataObj));

    return obj;
}

QJsonObject QkAPIHandler::rpc_rt_event(RPCArgsRT *args)
{
    qDebug() << "rpc_rt_event";

    QJsonObject obj;

    QkDevice::Event *event = args->event;

    QJsonObject eventObj;
    QJsonArray argsArray;

    eventObj.insert("connID", QJsonValue(args->connID));
    eventObj.insert("address", QJsonValue(args->address));
    eventObj.insert("label", QJsonValue(event->label()));
    eventObj.insert("message", QJsonValue(event->message()));

    int i;
    for(i = 0; i < event->args().count(); i++)
    {
        argsArray.insert(i, QJsonValue(event->args().at(i)));
    }
    eventObj.insert("args", QJsonValue(argsArray));
    obj.insert("event", QJsonValue(eventObj));

    return obj;
}

QJsonObject QkAPIHandler::rpc_rt_debug(RPCArgsRT *args)
{
    qDebug() << "rpc_rt_debug";

    QJsonObject obj;

    QJsonObject debugObj;

    debugObj.insert("connID", QJsonValue(args->connID));
    debugObj.insert("address", QJsonValue(args->address));
    debugObj.insert("message", QJsonValue(args->text));

    obj.insert("debug", QJsonValue(debugObj));

    return obj;
}

bool QkAPIHandler::validate_rpc_device(RPCArgs *args, QJsonObject *errorObj)
{
    bool ok;
    QkConnection *conn;
    QString addrStr = args->parsedValues->value("{addr}").toString();
    int addr = addrStr.toInt(&ok);
    if(!ok)
    {
        *errorObj = rpc_error(RPCError_Qk, tr("Invalid address: ") + addrStr);
        return false;
    }

    conn = m_connManager->defaultConnection();
    if(conn->qk->node(addr) == 0)
    {
        *errorObj = rpc_error(RPCError_Qk, tr("Node not found"));
        return false;
    }
    if(conn->qk->node(addr)->device() == 0)
    {
        *errorObj = rpc_error(RPCError_Qk, tr("Device not found"));
        return false;
    }

    return true;
}

void QkAPIHandler::_handleDataReceived(int address)
{
    qDebug() << "handle data received from" << address << "sender" << sender();

    if(!m_realTimeEnabled || !m_subscribedNames.contains("data"))
        return;

    QkCore *qk = (QkCore*)sender();
    QkConnection *conn = (QkConnection*)qk->parent();

    int connID = m_connManager->connectionID(conn);

    QJsonDocument jsonDoc;
    RPCArgsRT args;
    args.conn = conn;
    args.connID = connID;
    args.address = address;

    jsonDoc.setObject(rpc_rt_data(&args));
    emit sendJson(jsonDoc);
}

void QkAPIHandler::_handleEventReceived(int address, QkDevice::Event event)
{
    qDebug() << "handle event received from" << address;

    if(!m_realTimeEnabled || !m_subscribedNames.contains("event"))
        return;

    QkCore *qk = (QkCore*)sender();
    QkConnection *conn = (QkConnection*)qk->parent();

    int connID = m_connManager->connectionID(conn);

    QJsonDocument jsonDoc;
    RPCArgsRT args;
    args.conn = conn;
    args.connID = connID;
    args.address = address;
    args.event = &event;

    jsonDoc.setObject(rpc_rt_event(&args));
    emit sendJson(jsonDoc);
}

void QkAPIHandler::_handleDebugStringReceived(int address, QString str)
{
    qDebug() << "handle debug received from" << address;

    if(!m_realTimeEnabled || !m_subscribedNames.contains("debug"))
        return;

    QkCore *qk = (QkCore*)sender();
    QkConnection *conn = (QkConnection*)qk->parent();

    int connID = m_connManager->connectionID(conn);

    QJsonDocument jsonDoc;
    RPCArgsRT args;
    args.conn = conn;
    args.connID = connID;
    args.address = address;
    args.text = str;

    jsonDoc.setObject(rpc_rt_debug(&args));
    emit sendJson(jsonDoc);
}

void QkAPIHandler::_handleConnectionAdded(QkConnection *conn)
{
    connect(conn->qk, SIGNAL(dataReceived(int)), this, SLOT(_handleDataReceived(int)));
    connect(conn->qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(_handleEventReceived(int,QkDevice::Event)));
    connect(conn->qk, SIGNAL(debugString(int,QString)), this, SLOT(_handleDebugStringReceived(int,QString)));
}

void QkAPIHandler::_handleConnectionRemoved(QkConnection *conn)
{
    disconnect(conn->qk, SIGNAL(dataReceived(int)), this, SLOT(_handleDataReceived(int)));
    disconnect(conn->qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(_handleEventReceived(int,QkDevice::Event)));
    disconnect(conn->qk, SIGNAL(debugString(int,QString)), this, SLOT(_handleDebugStringReceived(int,QString)));
}

void QkAPIHandler::setupConnections()
{
    connect(m_connManager, SIGNAL(connectionAdded(QkConnection*)),
            this, SLOT(_handleConnectionAdded(QkConnection*)));
    connect(m_connManager, SIGNAL(connectionRemoved(QkConnection*)),
            this, SLOT(_handleConnectionRemoved(QkConnection*)));

    int connID;
    QList<QkConnection*> conns = m_connManager->connections();
    for(connID = 0; connID < conns.count(); connID++)
    {
        connect(conns[connID]->qk, SIGNAL(dataReceived(int)), this, SLOT(_handleDataReceived(int)));
        connect(conns[connID]->qk, SIGNAL(eventReceived(int,QkDevice::Event)), this, SLOT(_handleEventReceived(int,QkDevice::Event)));
        connect(conns[connID]->qk, SIGNAL(debugString(int,QString)), this, SLOT(_handleDebugStringReceived(int,QString)));
    }
}

QkAPIHandler::APITreeItem::APITreeItem(QString resource, QString rpcName, APITreeItem *parent)
{
    this->resource = resource;
    this->attributes = 0;
    this->rpcName = rpcName;
    this->parent = parent;
}

void QkAPIHandler::APITreeItem::setAttributesFromString(const QString &attrStr)
{
    this->attributes = 0;
    if(attrStr.contains("get"))
        this->attributes |= GET;
    if(attrStr.contains("set"))
        this->attributes |= SET;
}
