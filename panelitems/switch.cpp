#include "switch.h"
#include <QString>
#include <QLabel>
#include <QLineEdit>
#include <QCheckBox>
#include "../widgets/numberinputlineedit.h"
#include "extplaneclient.h"
#include "../util/console.h"
#include <QGraphicsSceneMouseEvent>

REGISTER_WITH_PANEL_ITEM_FACTORY(Switch,"switches/generic");

Switch::Switch(ExtPlanePanel *panel, ExtPlaneConnection *conn) :
        PanelItem(panel, PanelItemTypeSwitch, PanelItemShapeRectangular),
        _client(this, typeName(), conn) {
    conn->registerClient(&_client);

    connect( &_client, SIGNAL( refChanged( QString,double ) ),
             this, SLOT( valueChanged( QString,double ) ) );

    connect( &_client, SIGNAL( refChanged( QString, QString ) ),
             this, SLOT( valueChanged( QString, QString ) ) );

    connect( &_client, SIGNAL( refChanged( QString, QStringList ) ),
             this, SLOT( valueChanged( QString, QStringList ) ) );

    _value = false;
    _label = "Switch";
    _ref = 0;
    _refIndex = 0;
    setSize(100,30);
}

void Switch::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    setupPainter(painter);
    painter->setBrush(Qt::gray);
    painter->setPen(Qt::gray);

    double circleSize = height()/3;
    painter->drawEllipse(0, height()/2-circleSize/2, circleSize, circleSize);

    painter->save();
    _switchWidth = height()/3;
    painter->translate(_switchWidth/2, height()/2);

    QPolygon p;
    p << QPoint(-_switchWidth/4, 0) << QPoint(_switchWidth/4, 0)
      << QPoint(_switchWidth/2, height()/2) << QPoint(-_switchWidth/2, height()/2);
    if(_value)
        painter->scale(1,-1);
    if(_value) {
        painter->setBrush(Qt::darkGray);
    } else {
        painter->setBrush(Qt::lightGray);
    }
    painter->drawPolygon(p);

    painter->restore();
    painter->setPen(Qt::white);
    QFont font = defaultFont;
    font.setPixelSize(height()*0.75);
    painter->setFont(font);
    painter->drawText(QRect(_switchWidth,0,width()-_switchWidth, height()), Qt::AlignCenter, _label);
    PanelItem::paint(painter, option, widget);
}

void Switch::storeSettings(QSettings &settings) {
    PanelItem::storeSettings(settings);

    settings.setValue("label", _label);
    settings.setValue("dataref", _refName);
}

void Switch::loadSettings(QSettings &settings) {
    PanelItem::loadSettings(settings);
    setLabel(settings.value("label", "Switch").toString());
    setRef(settings.value("dataref", "").toString());
}

void Switch::createSettings(QGridLayout *layout) {
    layout->addWidget(new QLabel("Label", layout->parentWidget()));
    QLineEdit *labelEdit = new QLineEdit(_label, layout->parentWidget());
    connect(labelEdit, SIGNAL(textChanged(QString)), this, SLOT(setLabel(QString)));
    layout->addWidget(labelEdit);

    layout->addWidget(new QLabel("Dataref", layout->parentWidget()));
    QLineEdit *refEdit = new QLineEdit(_refName, layout->parentWidget());
    connect(refEdit, SIGNAL(textChanged(QString)), this, SLOT(setRef(QString)));
    layout->addWidget(refEdit);

    createNumberInputSetting( layout, "DataRef Index", _refIndex, SLOT( setRefIndex( float ) ) );
}

void Switch::applySettings() {
    if(_ref) {
        _ref->unsubscribe();
        _ref = 0;
    }
    if(!_refName.isEmpty())
        _ref = _client.subscribeDataRef(_refName, _refIndex);
}

void Switch::setLabel(QString txt) {
    _label = txt;
    update();
}

void Switch::setRef(QString txt) {
    if(_ref) {
        _ref->unsubscribe();
        _ref = 0;
    }
    _refName = txt;
    update();
}

void Switch::setRefIndex( float index ) {
    _refIndex = (uint)index;
    setRef( _refName );
}

void Switch::mousePressEvent ( QGraphicsSceneMouseEvent * event ) {
    if(isEditMode()) {
        PanelItem::mousePressEvent(event);
    }
}

void Switch::mouseReleaseEvent(QGraphicsSceneMouseEvent *event) {
    if(isEditMode()) {
        PanelItem::mouseReleaseEvent(event);
    } else if(event->pos().x() < _switchWidth) {
        _value = !_value;
        if(_ref) _ref->setValue(_value ? 1 : 0, _refIndex);
        update();
    }
}

void Switch::valueChanged( QString ref, double newValue) {
    DEBUG << ref << newValue;
    Q_ASSERT(ref==_refName);
    _value = newValue != 0;
    //INFO << "d newValue " << newValue << " gives " << _value;
    update();
}

void Switch::valueChanged( QString ref , QString newValue ) {
    valueChanged( ref, newValue.toDouble() );
    //INFO << "QS newValue " << newValue << " gives " << _value;
}

void Switch::valueChanged( QString ref, QStringList newValues) {
    uint i = _refIndex;
    if( i >= newValues.size() ) {
        i = newValues.size() - 1;
    }
    _value = newValues.at( i ).toInt() != 0;
    //INFO << "QSList newValues " << newValues.at( i ) << " gives " << _value;
    update();
}
