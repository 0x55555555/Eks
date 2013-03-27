#include "XColourWidget"
#include "pickerWidget/qtcolortriangle.h"
#include "XVector3DWidget"
#include "XFloatWidget"
#include "QtWidgets/QVBoxLayout"
#include "QtWidgets/QHBoxLayout"
#include "QtWidgets/QGroupBox"
#include "QtWidgets/QTabWidget"
#include "QtWidgets/QLabel"

#include "QDebug"

XColourWidget::XColourWidget(  QWidget *parent, const Eks::Colour &col, bool hA ) : QWidget(parent),
        _slider( new XVector3DWidget( this, Eks::Vector3D( 0, 0, 0 ), QStringList() << "R" << "G" << "B" )),
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
    connect( _slider, SIGNAL(valueChanged(Eks::Vector3D)), this, SLOT(rgbChanged(Eks::Vector3D)));

    setColour( col );
    }

void XColourWidget::pickerChanged( QColor col )
    {
    setColour( Eks::Colour( col.redF(), col.greenF(), col.blueF(), alpha() ) );
    }

void XColourWidget::rgbChanged( const Eks::Vector3D &col )
    {
    setColour( Eks::Colour( col.x(), col.y(), col.z(), alpha() ) );
    }

void XColourWidget::setAlpha( double in )
    {
    Eks::Colour s( colour() );
    s.w() = in;
    setColour( s );
    }

void XColourWidget::setColour( const Eks::Colour &inCol )
    {
    if( !_setting )
        {
        Eks::Colour col = inCol;
        _setting = true;

        col.w() = alpha();

        _slider->setValue( Eks::Vector3D( col.x(), col.y(), col.z() ) );
        _picker->setColor( QColor::fromRgbF( col.x(), col.y(), col.z(), col.w() ) );

        if( _alpha )
            {
            _alpha->setValue( col.w() );
            }

        Q_EMIT colourChanged( col );

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

Eks::Colour XColourWidget::colour()
    {
    return Eks::Colour( _slider->value().x(), _slider->value().y(), _slider->value().z(), 1.0 );
    }
