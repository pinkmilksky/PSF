#include "overlay.h"
#include <QDebug>
#include <algorithm>

Overlay::Overlay()
{

}

// функция для создания изображения без центрирования
Parser Overlay::makeSimple(const QSet<Parser*>& parsers)
{
    qDebug() << "Search aggregate matrix size";


    int height = 0, width = 0;

    // цикл для поиска максимальной ширины и длины изображения
    for (Parser parser: parsers) {

        height = std::max(height, parser.height());
        width = std::max(width, parser.width());

        qDebug() << "Current size: ("
                 << height << ','
                 << width << ')';
    }

    qDebug() << "Build aggregate matrix";

    Parser p = Parser(width, height);

    for (Parser* parser: parsers) {
        for (int x = 0; x < parser->width(); x++) {

            // перенос
            for (int y = 0; y < parser->height(); y++) {
                int nValue = parser->Get_Value(x, y) + p.Get_Value(x, y);
                p.setValue(x, y, std::min(nValue, 255));
            }
        }
    }

    QString numbers = "(";
    for (auto it = parsers.rbegin(); it != parsers.rend(); it++) {
        numbers += QString::number((*it)->absolute_position + 1);
        if (it != parsers.rend() - 1) numbers += ", ";
    }
    numbers += ")";

    p.m_sFileName = "Наложение без центрирования " + numbers;
    p.PreProcessing();
    return p;
}

Parser Overlay::makeCentered(const QSet<Parser*>& parsers)
{
    qDebug() << "Search aggregate mass center";

    // центр масс
    double massCenterX = 0, massCenterY = 0;

    // цикл для поиска максимальной x-координаты и
    // максимальной y-координаты среди всех центров масс
    for (Parser parser: parsers) {
        qDebug() << "Mass center: ("
                 << parser.m_CenterXCircle << ','
                 << parser.m_CenterYCircle << ')';

        massCenterX = std::max(massCenterX, parser.m_CenterXCircle);
        massCenterY = std::max(massCenterY, parser.m_CenterYCircle);

        qDebug() << "Current mass center: ("
                 << massCenterX << ','
                 << massCenterY << ')';
    }

    qDebug() << "Search aggregate matrix size";

    // количества строк и столбцов
    int height = 0, width = 0;

    // цикл для определения количества строк и столбцов
    for (Parser parser: parsers) {

        // определение смещения
        int offsetX = static_cast<int>(ceil(massCenterX - parser.m_CenterXCircle)),
            offsetY = static_cast<int>(ceil(massCenterY - parser.m_CenterYCircle));

        qDebug() << "Offset: ("
                 << offsetX << ','
                 << offsetY << ')';

        qDebug() << "Size: " << parser.width() << ',' << parser.height();

        height = std::max(height, parser.height() + offsetY);
        width = std::max(width, parser.width() + offsetX);

        qDebug() << "Current size: ("
                 << width << ','
                 << height << ')';
    }

    qDebug() << "Build aggregate matrix";


    Parser p = Parser(width, height);

    for (Parser parser: parsers) {

        // определение смещения
        int offsetX = static_cast<int>(ceil(massCenterX - parser.m_CenterXCircle)),
            offsetY = static_cast<int>(ceil(massCenterY - parser.m_CenterYCircle));

        qDebug() << "Offset: ("
                 << offsetX << ','
                 << offsetY << ')';

        for (int x = 0; x < parser.width(); x++) {

            // перенос
            for (int y = 0; y < parser.height(); y++) {
                int nValue = parser.Get_Value(x, y) + p.Get_Value(x + offsetX, y + offsetY);
                p.setValue(x + offsetX, y + offsetY, std::min(nValue, 255));
            }
        }
    }

    QString numbers = "(";
    for (auto it = parsers.rbegin(); it != parsers.rend(); it++) {
        numbers += QString::number((*it)->absolute_position + 1);
        if (it != parsers.rend() - 1) numbers += ", ";
    }
    numbers += ")";

    p.m_sFileName = "Наложение с центрированием " + numbers;
    p.PreProcessing();
    return p;
}
