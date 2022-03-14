#pragma once

#include <QObject>
#include <QVariantHash>
#include <QVariantList>
#include <QString>
#include "./qrpc_types.h"

namespace Private {
const QVariantHash &___opHead();

const QVariantHash &___opGet();

const QVariantHash &___opPost();

const QVariantHash &___opPut();

const QVariantHash &___opDelete();

const QVariantHash &___opOptions();

const QVariantHash &___opPatch();

const QVariantHash &___opTrace();

const QVariantList &___opCrud();

const QVariantList &___opPostPut();
}

namespace QRpc
{

class NotationsExtended
{
public:
    explicit NotationsExtended(QObject*parent=nullptr);
    virtual ~NotationsExtended();
public:
    const QVariantList &opPostPut=Private::___opPostPut();
    const QVariantList &opCrud=Private::___opCrud();
    const QVariantHash &opTrace=Private::___opTrace();
    const QVariantHash &opPatch=Private::___opPatch();
    const QVariantHash &opOptions=Private::___opOptions();
    const QVariantHash &opDelete=Private::___opDelete();
    const QVariantHash &opPut=Private::___opPut();
    const QVariantHash &opPost=Private::___opPost();
    const QVariantHash &opGet=Private::___opGet();
    const QVariantHash &opHead=Private::___opHead();
private:

};

}
