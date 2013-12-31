#ifndef XFILESEQUENCE_H
#define XFILESEQUENCE_H

#include "XGlobal.h"
#include "QString"
#include "Utilities/XProperty.h"

class XVariant;

class EKSCORE_EXPORT XFileSequence
    {
public:
    // parsed is file.$4F.jpg, numeric extract is file.1020.jpg
    enum stringMode { Auto, Parsed, NumericExtract };
    // construct a sequence sequence from the name of a file in the sequence
    XFileSequence( QString="", stringMode=Auto );
    // construct a sequence sequence from the name of a file in the sequence
    XFileSequence( const char *, stringMode=Auto );
#if 0
    // construct a sequence from a variant
    explicit XFileSequence( XVariant );

    // compress a sequence to a variant
    operator XVariant();
#endif

    // set the sequence
    void setSequence( QString, stringMode=Auto );

    QString niceFilename() const;
    QString parsedFilename( ) const;

    void setHasFrameNumber( bool );
    void setPreNumberString( QString );
    void setPadding( uint );
    void setPostNumberString( QString );


    bool hasFrameNumber( ) const;
    QString preNumberString( ) const;
    uint padding( ) const;
    QString postNumberString( ) const;

    QString getFilename( int frame ) const;

private:
    QString _pre;
    uint _padding;
    QString _post;
    bool _hasNumber;
    };

#endif // XFILESEQUENCE_