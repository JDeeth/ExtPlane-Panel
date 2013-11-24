#ifndef GENERICGAUGE_H
#define GENERICGAUGE_H

#include "needleinstrument.h"
/**
 * General-purpose dataref-driven needle gauge
 *
 * To be driven by an arbitrary dataref and given a scale configured at runtime.
 *
 * Derived from variometer and datarefdebugger
 */
#define VARIOMETER_ACCURACY 0.05
#define VARIOMETER_ACCURACY_TE 20

class GenericGauge : public NeedleInstrument
{
    Q_OBJECT

public:
    Q_INVOKABLE GenericGauge(ExtPlanePanel *panel, ExtPlaneConnection *conn);
    virtual void storeSettings(QSettings &settings);
    virtual void loadSettings(QSettings &settings);
    virtual QString typeName();
    virtual void createSettings(QGridLayout *layout);

public slots:
    void setDataRefName( QString name );
    void setDataRefIndex( float index ); //needs to be float else ERRORS
    void setLabel( QString text );

    void valueChanged( QString name, double val );
    void valueChanged( QString name, QString val);
    void valueChanged( QString name, QStringList values);
    void setZeroValue( float zv );
    void setMaxValue(  float mv );
    void setZeroAngle( float za );
    void setMaxAngle(  float ma );

    void setThickBar(  float tb );
    void setThinBar(   float tb );
    void setNumbers(   float div);
    void setNumberScale(float ns);

    virtual void tickTime(double dt, int total);
    virtual void setInterpolationEnabled(bool ie);

protected:

private:
    ExtPlaneClient _client;
    QString _currentName;
    uint _index;
    ValueInterpolator interpolator;
};

#endif // GENERICGAUGE_H
