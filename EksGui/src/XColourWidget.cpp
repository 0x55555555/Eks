#include "XColourWidget"
#include "pickerWidget/qtcolortriangle.h"
#include "XVector3DWidget"
#include "XFloatWidget"
#include "QVBoxLayout"
#include "QHBoxLayout"
#include "QGroupBox"
#include "QTabWidget"
#include "QLabel"

#include "QDebug"

XColourWidget::XColourWidget(  QWidget *parent, const XColour &col, bool hA ) : QWidget(parent),
        _slider( new XVector3DWidget( this, XVector3D( 0, 0, 0 ), QStringList() << "R" << "G" << "B" )),
        _picker( new QtColorTriangle( this ) ),
        _outerLayout( new QVBoxLayout( this ) ),
        _sliderLayout( new QVBoxLayout( ) ),
        _pickerLayout( new QVBoxLayout( ) ),
        _setting( false )
    {
    _outerLayout->setContentsMargins( 0, 0, 0, 0 );

    QTabWidget *tabs = new QTabWidget( this );

    QWidget *sliderWidget( new QWidget );
    QWidget *pickerWidget( new QWidget );

    sliderWidget->setLayout( _sliderLayout );
    pickerWidget->setLayout( _pickerLayout );
    _sliderLayout->addWidget( _slider );
    _pickerLayout->addWidget( _picker );

    tabs->addTab( pickerWidget, "Picker" );
    tabs->addTab( sliderWidget, "Sliders" );
    tabs->setTabPosition( QTabWidget::West );


    _sliderLayout->setContentsMargins( 2, 2, 2, 2 );
    _pickerLayout->setContentsMargins( 2, 2, 2, 2 );

    _outerLayout->addWidget( tabs );

    if( hA )
        {
        _alphaLayout = new QHBoxLayout( this );
        _alphaLayout->setContentsMargins( 0, 0, 0, 0 );

        _alpha = new XFloatWidget( this, col.w() );

        _alphaLayout->addWidget( new QLabel( "Alpha", this ) );
        _alphaLayout->addWidget( _alpha );

        _outerLayout->addLayout( _alphaLayout );

        connect( _alpha, SIGNAL(valueChanged(double)), this, SLOT(setAlpha(double)));
        }
    else
        {
        _alpha = 0;
        }

    connect( _picker, SIGNAL(colorChanged(QColor)), this, SLOT(pickerChanged(QColor)));
    connect( _slider, SIGNAL(valueChanged(XVector3D)), this, SLOT(rgbChanged(XVector3D)));

    setColour( col );
    }

void XColourWidget::pickerChanged( QColor col )
    {
    setColour( XColour( col.redF(), col.greenF(), col.blueF(), alpha() ) );
    }

void XColourWidget::rgbChanged( const XVector3D &col )
    {
    setColour( XColour( col.x(), col.y(), col.z(), alpha() ) );
    }

void XColourWidget::setAlpha( double in )
    {
    XColour s( colour() );
    s.w() = in;
    setColour( s );
    }

void XColourWidget::setColour( const XColour &inCol )
    {
    if( !_setting )
        {
        XColour col = inCol;
        _setting = true;

        col.w() = alpha();

        _slider->setValue( XVector3D( col.x(), col.y(), col.z() ) );
        _picker->setColor( QColor::fromRgbF( col.x(), col.y(), col.z(), col.w() ) );

        if( _alpha )
            {
            _alpha->setValue( col.w() );
            }

        emit colourChanged( col );

        _setting = false;
        }
    }

double XColourWidget::alpha()
    {
    if( _alpha )
        {
        return _alpha->value();
        }
    return 1;
    }

XColour XColourWidget::colour()
    {
    return XColour( _slider->value().x(), _slider->value().y(), _slider->value().z(), 1.0 );
    }
