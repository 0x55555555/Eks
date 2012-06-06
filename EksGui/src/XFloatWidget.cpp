#include "XFloatWidget"
#include "QHBoxLayout"
#include "QDoubleSpinBox"
#include "QSlider"
#include "math.h"

#include "QDebug"

XFloatWidget::XFloatWidget( QWidget *parent, qreal val ) : QWidget( parent ),
        _layout( new QHBoxLayout( this ) ),
        _spinner( new QDoubleSpinBox( this ) ),
        _slider( new QSlider( Qt::Horizontal, this ) ),
        _setting(false),
        _settingValue(false),
        _settingSlider(false)
    {
    _layout->addWidget( _slider );
    _layout->addWidget( _spinner );
    _layout->setContentsMargins( 0, 0, 0, 0 );
    _layout->setSpacing( 5 );

    connect( _spinner, SIGNAL(valueChanged(double)), this, SLOT(setValueFromSpinner(double)));
    connect( _slider, SIGNAL(valueChanged(int)), this, SLOT(setValueFromSlider(int)));

    _spinner->setSizePolicy( QSizePolicy::Expanding, QSizePolicy::Fixed );

    _slider->setMaximum( 0x7FFF );
    _slider->setMinimum( -0x7FFF );
    _slider->setSingleStep( 250 );
    _slider->setPageStep( 5000 );

    _spinner->setDecimals( 4 );
    _spinner->setMinimumWidth( 40 );

    this->blockSignals(true);

    setMaximum( HUGE_VAL );
    setMinimum( -HUGE_VAL );
    setValue( val );
    updateUsed();

    this->blockSignals(false);
    }

XFloatWidget::XFloatWidget( qreal val, qreal min, qreal max, QWidget *parent ) : QWidget( parent ),
        _layout( new QHBoxLayout( this ) ),
        _spinner( new QDoubleSpinBox( this ) ),
        _slider( new QSlider( Qt::Horizontal, this ) ),
        _setting(false),
        _settingValue(false),
        _settingSlider(false)
    {
    _layout->addWidget( _slider );
    _layout->addWidget( _spinner );
    _layout->setContentsMargins( 0, 0, 0, 0 );
    _layout->setSpacing( 5 );

    connect( _spinner, SIGNAL(valueChanged(double)), this, SLOT(setValueFromSpinner(double)));
    connect( _slider, SIGNAL(valueChanged(int)), this, SLOT(setValueFromSlider(int)));

    _slider->setMaximum( 0x7FFF );
    _slider->setMinimum( -0x7FFF );
    _slider->setSingleStep( 250 );
    _slider->setPageStep( 5000 );

    _spinner->setDecimals( 4 );
    _spinner->setMinimumWidth( 50 );

    if( val > max )
        {
        max = val;
        }

    this->blockSignals(true);

    setMaximum( max );
    setMinimum( min );
    setValue( val );
    updateUsed();

    this->blockSignals(false);
    }

void XFloatWidget::setValueFromSpinner( double in )
    {
    if( !_setting )
        {
        _setting = true;
        setValue( in );
        if( !_slider->isSliderDown() )
            {
            updateUsed();
            }
        _setting = false;
        }
    }

void XFloatWidget::forceValue( double in )
    {
    _spinner->setValue( in );
    setSliderFromValue( in );
    }

void XFloatWidget::setValue( qreal in )
    {
    if( !_settingValue )
        {
        if( in > maximum() )
            {
            in = maximum();
            }
        if( in < minimum() )
            {
            in = minimum();
            }

        _settingValue = true;
        if( !qFuzzyCompare( in, _spinner->value() ) )
            {
            _spinner->setValue( in );
            }
        setSliderFromValue( in );
        emit valueChanged( this );
        emit valueChanged( _spinner->value() );
        _settingValue = false;
        }
    }

double XFloatWidget::value() const
    {
    return _spinner->value();
    }

void XFloatWidget::setSliderFromValue( double in )
    {
    int scaled = ( ( ( in - usedMinimum() ) / usedRange() ) * (double)0xFFFE ) - (double)0x7FFF;
    _settingSlider = true;
    _slider->setValue( scaled );
    _settingSlider = false;
    }

void XFloatWidget::setValueFromSlider( int val )
    {
    if( !_settingSlider )
        {
        _settingSlider = true;
        qreal scaled = ( ( ( (double)val + (double)0x7FFF ) / (double)0xFFFE ) * usedRange() ) + usedMinimum();
        setValue( scaled );
        _settingSlider = false;
        }
    }

qreal XFloatWidget::usedMaximum() const
    {
    return _usedMaximum;
    }

qreal XFloatWidget::usedMinimum() const
    {
    return _usedMinimum;
    }

double XFloatWidget::usedRange() const
    {
    return usedMaximum() - usedMinimum();
    }

void XFloatWidget::updateUsed()
    {
    int p = 0;
    qreal ret = 0;
    while( ret > value() )
        {
        ret = -10 ^( ++p );
        }
    _usedMinimum = - 10^p;
    if( _usedMinimum < minimum() )
        {
        _usedMinimum = minimum();
        }

    p = 0;
    ret = 0;
    while( ret < value() )
        {
        ret = 10 ^ ++p;
        }
    _usedMaximum = 10 ^ p;
    if( _usedMaximum > maximum() )
        {
        _usedMaximum = maximum();
        }

    setSliderFromValue( value() );
    }

bool XFloatWidget::isReadOnly() const
    {
    return _spinner->isReadOnly();
    }

void XFloatWidget::setReadOnly(bool t)
    {
    _spinner->setReadOnly(t);
    }

void XFloatWidget::setMaximum( double in )
    {
    _spinner->setMaximum( in );
    if( minimum() > HUGE_VAL && maximum() < HUGE_VAL )
        {
        _spinner->setSingleStep( range() / 20 );
        }
    else
        {
        _spinner->setSingleStep( 1 );
        }
    }

double XFloatWidget::maximum() const
    {
    return _spinner->maximum();
    }

void XFloatWidget::setMinimum( double in )
    {
    _spinner->setMinimum( in );
    if( minimum() > HUGE_VAL && maximum() < HUGE_VAL )
        {
        _spinner->setSingleStep( range() / 20 );
        }
    else
        {
        _spinner->setSingleStep( 1 );
        }
    }

double XFloatWidget::minimum() const
    {
    return _spinner->minimum();
    }

double XFloatWidget::range() const
    {
    return _spinner->maximum() - _spinner->minimum();
    }
