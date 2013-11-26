#include "genericgauge.h"
#include <QLabel>
#include <QCheckBox>
#include "../units.h"
#include "../widgets/numberinputlineedit.h"
#include "../valueinterpolator.h"
#include "../needles/gabalancedneedle.h"

//debug
#include "../util/console.h"

REGISTER_WITH_PANEL_ITEM_FACTORY( GenericGauge, "indicator/generic/round" );

GenericGauge::GenericGauge( ExtPlanePanel *panel, ExtPlaneConnection *conn ) : NeedleInstrument( panel ),
    _client( this, typeName(), conn ), interpolator( 0, 3 ) {
    conn->registerClient( &_client );
    setDataRefName( "sim/cockpit/radios/transponder_brightness" );

    /// interpolate for non-array numeric return datarefs
    connect( &_client, SIGNAL( refChanged( QString, double ) ),
             &interpolator, SLOT( valueChanged( QString, double ) ) );
    connect( &interpolator, SIGNAL( interpolatedValueChanged( QString, double ) ),
             this, SLOT( valueChanged( QString, double ) ) );

    /// todo: figure out how to use interpolator for array values
    connect( &_client, SIGNAL( refChanged( QString, QString ) ),
             this, SLOT( valueChanged( QString, QString ) ) );

    connect( &_client, SIGNAL( refChanged( QString, QStringList ) ),
             this, SLOT( valueChanged( QString, QStringList ) ) );

    setDataRefIndex( 0 );
    _label = "New Gauge";
    setBars( 1, 0.5 );
    setNumbers( 1 );
    _zeroAngle = 0.0f;
    _maxAngle = 180.0f;
    _zeroValue = 0.0f;
    _maxValue = 1.0f;

    setNeedle( new GABalancedNeedle( this ) );

}

void GenericGauge::valueChanged( QString name , double val ) {
    setValue( val );
}

void GenericGauge::valueChanged( QString name , QString val ) {
    valueChanged( name, val.toDouble() );
}

void GenericGauge::valueChanged( QString name, QStringList values) {
    uint i = _index;
    if( i >= values.count() ) {
        i = values.count() - 1;
    }
    setValue( values.at( i ).toFloat() );
}

void GenericGauge::setLabel( QString text ) {
    /// todo: allow users to use escape sequences to insert newlines
    NeedleInstrument::setLabel( text );
}

void GenericGauge::storeSettings( QSettings &settings ) {
    PanelItem::storeSettings( settings );

    settings.setValue( "datarefname", _currentName );
    settings.setValue( "datarefindex", _index );
    settings.setValue( "label", _label );

    settings.setValue( "minValue", _zeroValue );
    settings.setValue( "maxValue", _maxValue );
    settings.setValue( "minAngle", _zeroAngle );
    settings.setValue( "maxAngle", _maxAngle );

    settings.setValue( "thickBars", _thickBars );
    settings.setValue( "thinBars", _thinBars );
    settings.setValue( "numberInterval", _numbers );
    settings.setValue( "numberScale", _numberScale );
}

void GenericGauge::loadSettings( QSettings &settings ) {

    //debug
    //INFO << "New GenericGauge " << this;

    PanelItem::loadSettings( settings );

    setDataRefName( settings.value( "datarefname","sim/cockpit/radios/transponder_brightness" ).toString() );
    setDataRefIndex( settings.value( "datarefindex", 0 ).toUInt( ) );
    setLabel( settings.value( "label", "").toString() );

    setZeroValue( settings.value( "minValue", 0 ).toDouble() );
    setMaxValue( settings.value( "maxValue", 300 ).toDouble() );
    setZeroAngle( settings.value( "minAngle", 0 ).toDouble() );
    setMaxAngle( settings.value( "maxAngle", 270 ).toDouble() );

    setThickBar( settings.value( "thickBars", ( _maxValue-_zeroValue )/4.0f ).toDouble() );
    setThinBar( settings.value( "thinBars", ( _maxValue-_zeroValue )/16.0f ).toDouble() );
    setNumbers( settings.value( "numberInterval", _thickBars ).toDouble() );
    setNumberScale( settings.value( "numberScale", 1 ).toDouble() );

    //debug
    //INFO << "Dataref: " << _currentName << "[" << _index << "]";

}

void GenericGauge::setDataRefName( QString name ) {
    // Unsubscribe old
    if( _currentName != "" ) _client.unsubscribeDataRef( _currentName );
    _currentName = name;

    // Subscribe new
    if( name != "" ) _client.subscribeDataRef( name ); // NO idea what Accuracy is for so leaving it default 0. - JSD
    update();
}

void GenericGauge::setDataRefIndex( float index ) {
    _index = (uint)index;
    setDataRefName( _currentName );
}

void GenericGauge::setZeroValue( float zv ) {
    _zeroValue = zv;
    repaintPixmaps();
}

void GenericGauge::setMaxValue( float mv ) {
    _maxValue = mv;
    repaintPixmaps();
}

void GenericGauge::setZeroAngle( float za ) {
    _zeroAngle = za;
    repaintPixmaps();
}

void GenericGauge::setMaxAngle( float ma ) {
    _maxAngle = ma;
    repaintPixmaps();
}

void GenericGauge::setThickBar( float tb ) {
    NeedleInstrument::setBars( tb, _thinBars );
}

void GenericGauge::setThinBar( float tb ) {
    NeedleInstrument::setBars( _thickBars, tb );
}

void GenericGauge::setNumbers( float div ){
    NeedleInstrument::setNumbers( div );
}

void GenericGauge::setNumberScale( float ns ) {
    NeedleInstrument::setNumberScale( ns );
}

void GenericGauge::createSettings( QGridLayout *layout ) {
    createLineEditSetting(    layout, "DataRef",       _currentName, SLOT( setDataRefName( QString ) ) );
    createNumberInputSetting( layout, "DataRef Index", _index,       SLOT( setDataRefIndex( float ) ) );
    createLineEditSetting(    layout, "Label",         _label,       SLOT( setLabel( QString ) ) );

    createNumberInputSetting( layout, "Minimum value", _zeroValue, SLOT( setZeroValue( float ) ) );
    createNumberInputSetting( layout, "Minimum angle", _zeroAngle, SLOT( setZeroAngle( float ) ) );
    createNumberInputSetting( layout, "Maximum value", _maxValue,  SLOT( setMaxValue( float ) ) );
    createNumberInputSetting( layout, "Maximum angle", _maxAngle,  SLOT( setMaxAngle( float ) ) );

    createNumberInputSetting( layout, "Thin bar interval",  _thinBars,  SLOT( setThinBar( float ) ) );
    createNumberInputSetting( layout, "Thick bar interval", _thickBars, SLOT( setThickBar( float ) ) );
    createNumberInputSetting( layout, "Number interval",    _numbers,   SLOT( setNumbers( float ) ) );
    createNumberInputSetting( layout, "Number scale",       _numberScale,  SLOT( setNumberScale( float ) ) );

}

void GenericGauge::tickTime( double dt, int total ) {
    interpolator.tickTime( dt, total );
}
void GenericGauge::setInterpolationEnabled( bool ie ) {
    interpolator.setEnabled( ie );
}
