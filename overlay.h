#ifndef OVERLAY_H
#define OVERLAY_H

#include "parser.h"
#include "algorithm"

#include <QSet>

//класс для наложения картинок
class Overlay
{
public:
    Parser* makeSimple(const QSet<Parser*>& parsers);
    Parser* makeCentered(const QSet<Parser*>& parsers);
    //конструктор
    Overlay();
};

#endif // OVERLAY_H
