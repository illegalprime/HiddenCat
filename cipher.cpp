#include "cipher.h"

Cipher::Cipher(QImage &startingImage, int bitsPerPixel)
    : image(startingImage)
    , maxHeadLen(std::numeric_limits<int>::max())
{
    maxMsgLen = setNewBitsUsedPerPixel(bitsPerPixel);
    imgArea   = image.width() * image.height();
}

int Cipher::getMaxMsgLen() {
    return maxMsgLen;
}

int Cipher::setNewBitsUsedPerPixel(int bits) {
    this->bitsPerPixel = bits;
    if ( ( maxMsgLen = (
                 // Pixels                      // Bits used per pixel
                (image.height() * image.width() * bits)
                 // Bits reqiured to represent a character
            /    Cipher::bitsPerChar )
                 // Header
            -    2                   ) > maxHeadLen ) {
        maxMsgLen = maxHeadLen;
        return maxHeadLen;
    }
    else {
        return maxMsgLen;
    }
}

void Cipher::setMessage(QString msg) {
    message = msg;
}

int  Cipher::getMessageLength() {
    return message.length();
}

bool Cipher::isMessageTooLong() {
    return message.length() > maxMsgLen;
}

// Classic 1-Bit Steganography with 4 Byte Header
QImage* Cipher::encodeImage() {
    QImage* encodedImage = new QImage(image.copy(0, 0, image.width(), image.height()));
    int x = 0, y = 0, header = message.length();

    if (message == NULL || bitsPerPixel == 0) {
        return encodedImage;
    }
    for (int mask = FIRST_BIT_MASK; mask != 0x0000; mask <<= 1) {
        QRgb bit   = (bool) (header & mask);
        QRgb pixel = encodedImage->pixel(x, y) & ~FIRST_BIT_MASK;

        encodedImage->setPixel(x, y, pixel | bit);

        if (++x == image.width()) {
            x = 0;
            if (++y == image.height()) {
                break;
            }
        }
    }

    for (QChar* character = message.begin(); character != message.end(); ++character) {

        for (ushort mask = FIRST_BIT_MASK; mask != 0x0000; mask <<= 1) {
            QRgb bit   = (bool)  (character->unicode() &  mask);
            QRgb pixel = encodedImage->pixel(x, y) & ~FIRST_BIT_MASK;

            encodedImage->setPixel(x, y, pixel | bit);

            if (++x == image.width()) {
                x = 0;
                if (++y == image.height()) {
                    return encodedImage;
                }
            }
        }
    }
    return encodedImage;
}

QString Cipher::decodeImage() {
    ushort character = 0x0000;
    int msgLen = 0;

    for (int x = Cipher::last_header_index % image.width(), y = Cipher::last_header_index / image.width(),
         count = Cipher::last_header_index; count > -1; --count) {

        msgLen <<= 1;
        msgLen = msgLen | (((ushort) image.pixel(x, y)) & FIRST_BIT_MASK);

        if (--x == -1) {
            --y;
            x = image.width() - 1;
        }
    }
    if (msgLen <= 0) {
        return QString("");
    }
    int pixel_count = (msgLen + 2) * Cipher::bitsPerChar - 1;

    if (pixel_count > imgArea) {
        pixel_count = imgArea - (imgArea % Cipher::bitsPerChar) - 1;
        msgLen      = maxMsgLen;
    }
    QString decoded(msgLen);

    for (int x = pixel_count % image.width(), y = pixel_count / image.width();
         pixel_count > Cipher::last_header_index; --pixel_count) {

        character <<= 1;
        character = character | (((ushort) image.pixel(x, y)) & FIRST_BIT_MASK);

        if (pixel_count % Cipher::bitsPerChar == 0) {
            decoded[--msgLen] = character;
            character = 0x0000;
        }
        if (--x == -1) {
            --y;
            x = image.width() - 1;
        }
    }

    return decoded;
}

QImage* Cipher::noShameImage() {
    QImage* encodedImage = new QImage(image.copy(0, 0, image.width(), image.height()));

    for (int x = 0, y = 0, count = 0,
        limit = message.length() * Cipher::bitsPerChar + 2; count < limit; ++count) {

        encodedImage->setPixel(x, y, ~encodedImage->pixel(x, y));

        if (++x == image.width()) {
            x = 0;
            if (++y == image.height()) {
                return encodedImage;
            }
        }
    }
    return encodedImage;
}

/*int Cipher::findNullTerminator() {
    ushort character = 0x0000;

    for (int pixel_count = imgArea - (imgArea % Cipher::bitsPerChar) - 1;
         pixel_count > -1; --pixel_count) {

            character = character | ((*image.scanLine(pixel_count)) & FIRST_BIT_MASK);
            character <<= 1;

            if (pixel_count % Cipher::bitsPerChar == 0) {
                if (character == TERMINATOR) {
                    return pixel_count;
                }
                character = 0x0000;
            }
    }
    return 0;
}

QString Cipher::decodeImage() {
    QString decodedMessage;
    ushort character = 0x0000;

    for (int i = Cipher::bitsPerChar - 1; i > -1; --i) {
        character = character | (((ushort) *image.scanLine(i)) & FIRST_BIT_MASK);
        character <<= 1;
    }
    int length = (int) character;

    if (length > maxMsgLen) {
        length = maxMsgLen;
    }

    character = 0x0000;
    for (int i = length * Cipher::bitsPerChar - 1; i > Cipher::bitsPerChar - 1; --i) {
        character = character | (((ushort) *image.scanLine(i)) & FIRST_BIT_MASK);
        character <<= 1;

        if (i % Cipher::bitsPerChar == 0) {
            decodedMessage.prepend(QChar(character));
            character = 0x0000;
        }
    }

    return decodedMessage;
}

QString Cipher::lazyDecode() {
    QString decoded;
    ushort character;

    for (int pixel_count = Cipher::bitsPerChar - 1; pixel_count < imgArea; pixel_count += Cipher::bitsPerChar) {
        character = 0x0000;

        for (int i = pixel_count; i > pixel_count - Cipher::bitsPerChar; --i) {
            character <<= 1;
            character = character |
                    (((ushort) image.pixel(i % image.width(), i / image.width())) & FIRST_BIT_MASK);
        }
        decoded.append(QChar(character));
    }

    return decoded;
}*/
