#ifndef COLOURWIDGET_H
#define COLOURWIDGET_H

#include "XGuiGlobal.h"
#include "QtHelper/XQWidget.h"
#include "Math/XColour.h"
#include "Math/XMathVector.h"

class QVBoxLayout;
class QHBoxLayout;
class QtColorTriangle;
class XVector3DWidget;
class XFloatWidget;
class QGroupBox;

class EKSGUI_EXPORT XColourWidget : public QWidget
    {
Q_OBJECT
public:
    XColourWidget( QWidget *parent = 0, const Eks::Colour &col = Eks::Colour(), bool hasAlpha=true );

    double alpha();
    Eks::Colour colour();

Q_SIGNALS:
    void colourChanged( const Eks::Colour & );

public Q_SLOTS:
    void setColour( const Eks::Colour & );

private Q_SLOTS:
    void setAlpha(double);
    void rgbChanged( const Eks::Vector3D & );
    void pickerChanged( QColor );

private:
    XVector3DWidget *_slider;
    XFloatWidget *_alpha;
    QtColorTriangle *_picker;
    QVBoxLayout *_outerLayout;
    QHBoxLayout *_alphaLayout;
    QVBoxLayout *_sliderLayout;
    QVBoxLayout *_pickerLayout;
    bool _setting;
    };

#endif // COLOURWIDGET_H
