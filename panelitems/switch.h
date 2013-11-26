#ifndef SWITCH_H
#define SWITCH_H

#include "panelitem.h"

class Switch : public PanelItem {
    Q_OBJECT
public:
    Q_INVOKABLE Switch(ExtPlanePanel *panel, ExtPlaneConnection *conn);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QString typeName();
    virtual void storeSettings(QSettings &settings);
    virtual void loadSettings(QSettings &settings);
    virtual void createSettings(QGridLayout *layout);
    virtual void applySettings();
protected:
    virtual void mousePressEvent ( QGraphicsSceneMouseEvent * event );
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
signals:

public slots:
    void setLabel( QString txt );
    void setRef( QString ref );
    void setRefIndex( float index ); //needs to be float else ERRORS
    void valueChanged( QString ref, double newValue);
    void valueChanged( QString name, QString val);
    void valueChanged( QString name, QStringList values);

private slots:

private:
    ExtPlaneClient _client;
    int _buttonId;
    QString _label;
    QString _refName;
    uint _refIndex;
    bool _value;
    ClientDataRef* _ref;
    int _switchWidth;
};

#endif // SWITCH_H
