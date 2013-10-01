#ifndef QKAPIHANDLER_H
#define QKAPIHANDLER_H

#include <QObject>
#include <QMap>
#include <QSet>
#include "qk.h"

class QkConnectionManager;
class QkConnection;
class QSignalMapper;

class QkAPIHandler : public QObject
{
    Q_OBJECT
public:
    explicit QkAPIHandler(QkConnectionManager *connManager, QObject *parent = 0);
    
    void setRealTime(bool enabled);

signals:
    void sendJson(const QJsonDocument &doc);
    
public slots:
    void handleJsonRpc(const QString &jsonRpc);

private slots:
    void _handleConnectionAdded(QkConnection *conn);
    void _handleConnectionRemoved(QkConnection *conn);
    void _handleDataReceived(int address);
    void _handleEventReceived(int address, QkDevice::Event event);
    void _handleDebugStringReceived(int address, QString str);

private:
    enum Constants
    {
        APITreeColumns = 3
    };
    enum MethodAttribute
    {
        GET = (1<<0),
        SET = (1<<1)
    };
    typedef MethodAttribute MethodType;

    enum RPCError
    {
        RPCError_ParseError = -32700,
        RPCError_MethodNotFound = -32601,
        RPCError_InvalidParams = -32602,
        RPCError_InternalError = -32603,
        RPCError_Qk = 123 //TODO add error codes
    };

    class RPCArgs
    {
    public:
        MethodType methodType;
        QMap<QString,QVariant> *parsedValues;
        QVariantList *params;
    };

    class RPCArgsRT
    {
    public:
        int connID;
        QkConnection *conn;
        int address;
        QkDevice::Event *event;
        QString text;
        int errCode, errArg;
    };

    typedef QJsonObject (QkAPIHandler::*RPC)(RPCArgs *args);

    class APITreeItem
    {
    public:
        APITreeItem(QString resource, QString rpcName, APITreeItem *parent = 0);

        QString resource;
        int attributes;
        QString rpcName;
        APITreeItem *parent;
        QList<APITreeItem*> children;

        void setAttributesFromString(const QString &attrStr);
    };


    void setupConnections();
    void create();
    QJsonObject run(const QString &methodName, const QString &path, QVariantList params);

    QJsonObject rpc_error(int code, const QString &message);
    QJsonObject rpc_result(int value);
    QJsonObject rpc_listQk(RPCArgs *args);
    QJsonObject rpc_listConns(RPCArgs *args);
    QJsonObject rpc_conn(RPCArgs *args);
    QJsonObject rpc_listNodes(RPCArgs *args);
    QJsonObject rpc_samplingFrequency(RPCArgs *args);
    QJsonObject rpc_samplingMode(RPCArgs *args);
    QJsonObject rpc_updateDevice(RPCArgs *args);
    QJsonObject rpc_listCmds(RPCArgs *args);
    QJsonObject rpc_search(RPCArgs *args);
    QJsonObject rpc_start(RPCArgs *args);
    QJsonObject rpc_stop(RPCArgs *args);
    QJsonObject rpc_listSubscriptions(RPCArgs *args);
    QJsonObject rpc_subData(RPCArgs *args);
    QJsonObject rpc_subEvent(RPCArgs *args);
    QJsonObject rpc_subDebug(RPCArgs *args);

    // Real-time (sent when subscribed)
    QJsonObject rpc_rt_data(RPCArgsRT *args);
    QJsonObject rpc_rt_event(RPCArgsRT *args);
    QJsonObject rpc_rt_debug(RPCArgsRT *args);

    bool validate_rpc_device(RPCArgs *args, QJsonObject *errorObj);

    QkConnectionManager *m_connManager;
    APITreeItem *m_apiTreeRoot;
    QMap<QString,RPC> m_rpcMapper;

    QSet<QString> m_subscribedNames;

    bool m_realTimeEnabled;
};

#endif // QKAPIHANDLER_H
