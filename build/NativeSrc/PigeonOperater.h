#ifndef __UNITY_PIGEON_H__
#define __UNITY_PIGEON_H__

#include "pigeon/NetController.h"
#include "pigeon/NetActivity.h"
#include "pigeon/RPCTransport.h"
#include "tjConfig.h"

#include <string>
#include <map>

using std::string;

#include <functional>
extern void pigeonPerformInLuaThread(const std::function<void()> &function);

typedef void(* NetErrorCallback)(int errcode, int detail);
typedef void(* NetOnConnectedCallback)(bool firstConnect);
typedef void(* NetOnNotifiedCallback)(int event, const char* data, int len, float timestamp);

class PigeonOperater
{
public:
    PigeonOperater();
    ~PigeonOperater();

    void CreateActive();
    void DestroyActive();

    typedef void(* UnityRequestCallback)(int opCode, int req, int err, const char* resp, int len, float timestamp);
    typedef UnityRequestCallback RequestCallback;

    bool connect(const char* host, in_port_t port, long timeout);
    bool repair(bool force);
    void close();
    int request(int method,
              char* params,
              size_t len,
              RequestCallback callback,
              uint16_t extraRTO);
    int rerequest(int reqId);
    int notify(int event, const void* data, size_t len);
    int respond(int request, int err, const void* data, size_t len);
    int isvalid();

    void setHeartbeatInterval(long millisecond);
    long getHeartbeatInterval();
    void setHeartbeatTimeout(long millisecond);
    void setLoopInterval(int loopInterval);
    void setReqRTOBounds(int lbound, int ubound);
    void setReqTimeout(int milliseconds);
    int getReqTimeout();
    long getMaxReqDelay();
    long getAvgReqDelay();
    long remoteTimestamp(long* elb, long* eub);
    long remoteTimeMillis(long* elb, long* eub);
    double remoteTimescale(double* lb, double* ub);
    void timeCorrectorReset();
    string getCryptoConfig();
    int getCryptoType();

    void enableRecvPushBuffer();
    void resetRecvPushBuffer(int recvPushId);

    void setComprType(int type);

    void setListenerCallback(NetErrorCallback errorCallback, NetOnConnectedCallback onConnectCallback, NetOnNotifiedCallback onNotifiedCallback);

private:
    NetController* net;
    RPCActivity* activity;
};

class UnityClientListener: public RPCClient::Listener
{
private:
    RPCActivity* _activity;

    NetErrorCallback _ErrorCallback;
    NetOnConnectedCallback _OnConnectedCallback;
    NetOnNotifiedCallback _OnNotifiedCallback;
public:
    void setListenerCallback(NetErrorCallback errorCallback, NetOnConnectedCallback onConnectCallback, NetOnNotifiedCallback onNotifiedCallback)
    {
        _ErrorCallback = errorCallback;
        _OnConnectedCallback = onConnectCallback;
        _OnNotifiedCallback = onNotifiedCallback;
    }
public:
    UnityClientListener(RPCActivity* activity) {
        _activity = activity;
    }
    virtual ~UnityClientListener() 
    {
        _ErrorCallback = NULL;
        _OnConnectedCallback = NULL;
        _OnNotifiedCallback = NULL;
    }
    virtual void onBinded(RPCClient* client) {}
    virtual void onUnbinded(RPCClient* client) { delete this; }

    virtual void onError(RPCClient* client, int errcode, int detail) {
        pigeonPerformInLuaThread([=]() {
            if (_ErrorCallback != NULL && _activity == client) {
                _ErrorCallback(errcode, detail);
            }
        });
    }
    virtual void onConnected(RPCClient* client, bool firstConnect) {
        pigeonPerformInLuaThread([=]() {
            if (_OnConnectedCallback != NULL && _activity == client) {
                _OnConnectedCallback(firstConnect);
            }
        });
    }
    virtual void onRequested(RPCClient* client, int request, int method, const std::string& params) {
        
    }
    virtual void onNotified(RPCClient* client, int event, const std::string& data,float timestamp) {
        pigeonPerformInLuaThread([=]() {
            if (_OnNotifiedCallback != NULL && _activity == client) {
                _OnNotifiedCallback(event, data.data(), data.size(), timestamp);
            }
        });
    }
};

#endif //__UNITY_PIGEON_H__