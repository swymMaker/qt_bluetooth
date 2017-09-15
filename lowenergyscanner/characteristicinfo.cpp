/***************************************************************************
**
** Copyright (C) 2013 BlackBerry Limited. All rights reserved.
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the QtBluetooth module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "characteristicinfo.h"
#include "qbluetoothuuid.h"
#include <QByteArray>
#include <string>
using namespace std;

CharacteristicInfo::CharacteristicInfo()
{
}

CharacteristicInfo::CharacteristicInfo(const QLowEnergyCharacteristic &characteristic, QLowEnergyService * service):
    m_characteristic(characteristic),
    srv(service)
{
    descriptor = m_characteristic.descriptor(QBluetoothUuid::ClientCharacteristicConfiguration);
       if(m_characteristic.isValid()){
           if(m_characteristic.properties() & QLowEnergyCharacteristic::Notify){ // enable notification
               srv->writeDescriptor(descriptor, QByteArray::fromHex("0100"));
           }
           if(m_characteristic.properties() & QLowEnergyCharacteristic::Indicate){ // enable indication
               srv->writeDescriptor(descriptor, QByteArray::fromHex("0200"));
           }
           srv->readDescriptor(descriptor);
           srv->readCharacteristic(m_characteristic);
       }
}

void CharacteristicInfo::setCharacteristic(const QLowEnergyCharacteristic &characteristic)
{
    m_characteristic = characteristic;
    emit characteristicChanged();
}

QString CharacteristicInfo::getName() const
{
    //! [les-get-descriptors]
    QString name = m_characteristic.name();
    if (!name.isEmpty())
        return name;

    // find descriptor with CharacteristicUserDescription
    foreach (const QLowEnergyDescriptor &descriptor, m_characteristic.descriptors()) {
        if (descriptor.type() == QBluetoothUuid::CharacteristicUserDescription) {
            name = descriptor.value();
            break;
        }
    }
    //! [les-get-descriptors]

    if (name.isEmpty()) {

       srv->writeDescriptor(descriptor, QByteArray::fromHex("0100"));

        if( getUuid() == "6e400002-b5a3-f393-e0a9-e50e24dcca9e") name = "Nordic UART TX";
        else if( getUuid() == "6e400003-b5a3-f393-e0a9-e50e24dcca9e") name = "Nordic UART RX";
        else name = "Unknown";
    }
    return name;
}

QString CharacteristicInfo::getUuid() const
{
    const QBluetoothUuid uuid = m_characteristic.uuid();
    bool success = false;
    quint16 result16 = uuid.toUInt16(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result16, 16);

    quint32 result32 = uuid.toUInt32(&success);
    if (success)
        return QStringLiteral("0x") + QString::number(result32, 16);

    return uuid.toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));
}

QString CharacteristicInfo::getValue() const
{
    srv->readDescriptor(descriptor);
    srv->readCharacteristic(m_characteristic);
    // Show raw string first and hex value below
    QByteArray a = m_characteristic.value();
    QString result;

    if (a.isEmpty()) {
        const QBluetoothUuid uuid = descriptor.uuid();

        bool success16 = false;
        bool success32 = false;

        quint16 result16 = uuid.toUInt16(&success16);
        quint32 result32 = uuid.toUInt32(&success32);

        if      (success16) result = QStringLiteral("0x") + QString::number(result16, 16);
        else if (success32) result = QStringLiteral("0x") + QString::number(result32, 16);
        else                result = uuid.toString().remove(QLatin1Char('{')).remove(QLatin1Char('}'));

        result += "(d_uuid)\n";
        result += descriptor.value().toHex() + " (d_val)\n";
        result += srv->serviceUuid().toString().remove(QLatin1Char('{')).remove(QLatin1Char('}')) + "(srv_uuid)\n";

        return result;
    }

    result = "";
    for ( int i = 0; i < a.size(); i++ )
        result += QString::number(a[i]) + ",";

    return result;
}

QString CharacteristicInfo::getHandle() const
{
    return QStringLiteral("0x") + QString::number(m_characteristic.handle(), 16);
}

QString CharacteristicInfo::getPermission() const
{
    QString properties = "( ";
    int permission = m_characteristic.properties();
    if (permission & QLowEnergyCharacteristic::Read)
        properties += QStringLiteral(" Read");
    if (permission & QLowEnergyCharacteristic::Write)
        properties += QStringLiteral(" Write");
    if (permission & QLowEnergyCharacteristic::Notify)
        properties += QStringLiteral(" Notify");
    if (permission & QLowEnergyCharacteristic::Indicate)
        properties += QStringLiteral(" Indicate");
    if (permission & QLowEnergyCharacteristic::ExtendedProperty)
        properties += QStringLiteral(" ExtendedProperty");
    if (permission & QLowEnergyCharacteristic::Broadcasting)
        properties += QStringLiteral(" Broadcast");
    if (permission & QLowEnergyCharacteristic::WriteNoResponse)
        properties += QStringLiteral(" WriteNoResp");
    if (permission & QLowEnergyCharacteristic::WriteSigned)
        properties += QStringLiteral(" WriteSigned");
    properties += " )";
    return properties;
}

QLowEnergyCharacteristic CharacteristicInfo::getCharacteristic() const
{
    return m_characteristic;
}
