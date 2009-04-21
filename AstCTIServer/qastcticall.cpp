/* Copyright (C) 2007-2009 Bruno Salzano
 * http://centralino-voip.brunosalzano.com
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 *
 * As a special exception, you may use this file as part of a free software
 * library without restriction.  Specifically, if other files instantiate
 * templates or use macros or inline functions from this file, or you compile
 * this file and link it with other files to produce an executable, this
 * file does not by itself cause the resulting executable to be covered by
 * the GNU General Public License.  This exception does not however
 * invalidate any other reasons why the executable file might be covered by
 * the GNU General Public License.
 *
 * This exception applies only to the code released under the name GNU
 * AstCTIServer.  If you copy code from other releases into a copy of GNU
 * AstCTIServer, as the General Public License permits, the exception does
 * not apply to the code that you add in this way.  To avoid misleading
 * anyone as to the status of such modified files, you must delete
 * this exception notice from them.
 *
 * If you write modifications of your own for AstCTIServer, it is your choice
 * whether to permit this exception to apply to your modifications.
 * If you do not wish that, delete this exception notice.
 */

#include "qastcticall.h"

QAstCTICall::QAstCTICall(QObject* parent)
        : QObject(parent), channel(""), parsed_channel(""),
        callerid_num(""), callerid_name(""),
        uniqueid(""), context(""), state("")
{
    this->variables = new QHash <QString,QString>;
    this->application = 0;
}

QAstCTICall::~QAstCTICall()
{
    if (this->variables != 0) delete(this->variables);
    if (this->application != 0) delete (this->application);

}

QString& QAstCTICall::get_channel()
{
    return this->channel;
}
void QAstCTICall::set_channel(QString channel)
{
    this->channel = channel;
    this->parse_channel();
}
QString& QAstCTICall::get_parsed_channel()
{
    return this->parsed_channel;
}
void QAstCTICall::set_parsed_channel(QString parsedchannel)
{
    this->parsed_channel = parsedchannel;
}

QString& QAstCTICall::get_dest_channel()
{
    return this->dest_channel;
}
void QAstCTICall::set_dest_channel(QString channel)
{
    this->dest_channel = channel;
    this->parse_dest_channel();
}
QString& QAstCTICall::get_parsed_dest_channel()
{
    return this->parsed_dest_channel;
}
void QAstCTICall::set_parsed_dest_channel(QString parsedchannel)
{
    this->parsed_dest_channel = parsedchannel;
}

QString& QAstCTICall::get_callerid_num()
{
    return this->callerid_num;
}
void QAstCTICall::set_callerid_num(QString calleridnum)
{
    this->callerid_num = calleridnum;
}
QString& QAstCTICall::get_callerid_name()
{
    return this->callerid_name;
}
void QAstCTICall::set_callerid_name(QString calleridname)
{
    this->callerid_name = calleridname;
}
QString& QAstCTICall::get_uniqueid()
{
    return this->uniqueid;
}
void QAstCTICall::set_uniqueid(QString uniqueid)
{
    this->uniqueid = uniqueid;
}

QString& QAstCTICall::get_dest_uniqueid()
{
    return this->dest_uniqueid;
}
void QAstCTICall::set_dest_uniqueid(QString uniqueid)
{
    this->dest_uniqueid = uniqueid;
}

QString& QAstCTICall::get_context()
{
    return this->context;
}
void QAstCTICall::set_context(QString context)
{
    this->context = context;
}
QString& QAstCTICall::get_state()
{
    return this->state;
}
void QAstCTICall::set_state(QString state)
{
    this->state = state;
}

void QAstCTICall::add_variable(QString key, QString value)
{
    // We need only one variable with the same name..
    if (this->variables->contains(key))
    {
        this->variables->remove(key);
    }
    this->variables->insert(key, value);
}

void QAstCTICall::parse_channel()
{
    // Channels are usually built like SIP/200-0899e2c8
    // So we need to extract channel by split the string
    if (this->channel.contains('-'))
    {
        QStringList parts = this->channel.split('-');
        if (parts.count() == 2)
        {
            QString parsed_chan = parts.at(0);
            this->set_parsed_channel(parsed_chan);
        }
    }
}

void QAstCTICall::parse_dest_channel()
{
    // Channels are usually built like SIP/200-0899e2c8
    // So we need to extract channel by split the string
    if (this->dest_channel.contains('-'))
    {
        QStringList parts = this->dest_channel.split('-');
        if (parts.count() == 2)
        {
            QString parsed_chan = parts.at(0);
            this->set_parsed_dest_channel(parsed_chan);
        }
    }
}

void QAstCTICall::set_applications(QAstCTIServicesApplications* applications)
{
    this->applications = applications;
}

void QAstCTICall::set_operating_system(QString operating_system)
{
    this->client_operating_system = operating_system;
    this->set_application();

}

void QAstCTICall::set_application()
{
    if (this->client_operating_system.length() > 0)
    {
        this->application = this->applications->operator []( this->client_operating_system );
        // here we can parse application parameters
        this->parse_application_parameters();
    }

}

void QAstCTICall::parse_application_parameters()
{
    if (this->application != 0)
    {
        if (this->variables->count() > 0)
        {
            QString parameters = this->application->get_parameters();

            QHash<QString, QString>::const_iterator var_list = this->variables->constBegin();
            while (var_list != this->variables->constEnd())
            {
                QString the_key = QString("{%1}").arg( ((QString)var_list.key()).toUpper());
                parameters = parameters.replace(the_key, var_list.value());

                ++var_list;
            }
            this->application->set_parameters(parameters);

        }


    }
}

QString QAstCTICall::to_xml()
{
    QDomDocument doc("AstCTICallXML");
    QDomElement root = doc.createElement("call");
    root.setAttribute("uniqueid", this->uniqueid);
    doc.appendChild( root );

    // DestUniqueId
    if (this->dest_uniqueid.length() > 0)
    {
        QDomElement tag = doc.createElement("DestUniqueId");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->dest_uniqueid);
        tag.appendChild(t);
    }

    // Context
    if (this->context.length() > 0)
    {
        QDomElement tag = doc.createElement("Context");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->context);
        tag.appendChild(t);
    }

    // Channel
    if (this->channel.length() > 0)
    {
        QDomElement tag = doc.createElement("Channel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->channel);
        tag.appendChild(t);
    }

    // ParsedChannel
    if (this->parsed_channel.length() > 0)
    {
        QDomElement tag = doc.createElement("ParsedChannel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->parsed_channel);
        tag.appendChild(t);
    }

    // DestChannel
    if (this->dest_channel.length() > 0)
    {
        QDomElement tag = doc.createElement("DestChannel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->dest_channel);
        tag.appendChild(t);
    }

    // ParsedChannelDest
    if (this->parsed_dest_channel.length() > 0)
    {
        QDomElement tag = doc.createElement("ParsedDestChannel");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->parsed_dest_channel);
        tag.appendChild(t);
    }

    // Callerid Name
    if (this->callerid_name.length() > 0)
    {
        QDomElement tag = doc.createElement("CallerIdName");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->callerid_name);
        tag.appendChild(t);
    }

    // Callerid Num
    if (this->callerid_name.length() > 0)
    {
        QDomElement tag = doc.createElement("CallerIdNum");
        root.appendChild(tag);
        QDomText t = doc.createTextNode(this->callerid_num);
        tag.appendChild(t);
    }

    if (this->variables->count() > 0)
    {
        QString var_count = QString(this->variables->count());
        QDomElement xmlvars = doc.createElement("Variables");
        xmlvars.attribute("count", var_count);
        root.appendChild(xmlvars);

        QHash<QString, QString>::const_iterator var_list = this->variables->constBegin();
        while (var_list != this->variables->constEnd())
        {
            QDomElement tag = doc.createElement(var_list.key());
            xmlvars.appendChild(tag);
            QDomText t = doc.createTextNode(var_list.value() );
            tag.appendChild(t);
            ++var_list;
        }

    }

    if (this->application != 0)
    {
        QDomElement xmlapplication = doc.createElement("Application");
        root.appendChild(xmlapplication);

        QDomElement xmlapppath = doc.createElement("Path");
        xmlapplication.appendChild(xmlapppath);
        QDomText xmlapppathval = doc.createTextNode(this->application->get_application_path());
        xmlapppath.appendChild(xmlapppathval);

        QDomElement xmlappparams = doc.createElement("Parameters");
        xmlapplication.appendChild(xmlappparams);
        QDomText xmlapppathparmval = doc.createTextNode(this->application->get_parameters());
        xmlappparams.appendChild(xmlapppathparmval);
    }

    return doc.toString();

}
