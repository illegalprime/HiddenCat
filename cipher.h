#ifndef CIPHER_H
#define CIPHER_H

#define BITS_PER_UCHAR 8
#define FIRST_BIT_MASK     0x0001
#define NOT_FIRST_BIT_MASK 0xFFF0
#define TERMINATOR         0x0000

#include <QString>
#include <QImage>
//#include <inttypes.h>
#include <limits>

class Cipher
{
public:
    Cipher(QImage& startingImage, int bitsPerPixel);
    int getMaxMsgLen();
    int setNewBitsUsedPerPixel(int bits);
    void setMessage(QString message);
    int  getMessageLength();
    bool isMessageTooLong();
    QImage* encodeImage();
    QImage* noShameImage();
    QString decodeImage();
    static const int bitsPerChar = sizeof(QChar) * BITS_PER_UCHAR;
    static const int header_len  = Cipher::bitsPerChar * 2;
    static const int last_header_index = header_len - 1;

private:
    int maxMsgLen;
    int bitsPerPixel;
    int imgArea;
    QImage& image;
    QString message;
    int findNullTerminator();
    const int maxHeadLen;
};

#endif // CIPHER_H
