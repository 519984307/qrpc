#pragma once

#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDebug>
#include <QStringList>
#include <QTimer>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QLocale>
#include <QJsonDocument>
#include <gtest/gtest.h>

namespace QRpc {

class SDKGoogleTest : public testing::Test{
public:

    explicit SDKGoogleTest(){
        this->clear();
        QLocale::setDefault(QLocale(QLocale::Portuguese, QLocale::Brazil));
    }

    virtual bool clear(){
        return true;
    }

    virtual bool serviceStart(){
        return this->clear();
    }

    virtual bool serviceStop(){
        return this->clear();
    }

    virtual QStringList arguments(){
        return qApp->arguments();
    }

    static QByteArray toMd5(const QVariant&v){
        QByteArray bytes;
        if(v.typeId()==QMetaType::QVariantList || v.typeId()==QMetaType::QStringList || v.typeId()==QMetaType::QVariantMap || v.typeId()==QMetaType::QVariantHash)
            bytes=QJsonDocument::fromVariant(v).toJson(QJsonDocument::Compact);
        else
            bytes=v.toByteArray();
        return QCryptographicHash::hash(bytes, QCryptographicHash::Md5).toHex();
    }

    static QVariant toVar(const QVariant&v){
        if(v.typeId()==QMetaType::QString || v.typeId()==QMetaType::QByteArray)
            return QJsonDocument::fromJson(v.toByteArray()).toVariant();
        else
            return v;
    }

    static void SetUpTestCase()
    {
    }

    virtual void SetUp()
    {
    }

    virtual void TearDown()
    {
    }

    static void TearDownTestCase()
    {
    }

public:

};

}


