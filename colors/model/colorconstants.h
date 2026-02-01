#pragma once
#include <QColor>

namespace ColorConstants {

// ------------------------------------------------------------
// Sárga árnyalatok – figyelmeztetés, incomplete állapot
// ------------------------------------------------------------
constexpr int YellowR = 255, YellowG = 243, YellowB = 205;
inline const QColor ColorYellow{YellowR, YellowG, YellowB};

// ------------------------------------------------------------
// Zöld árnyalatok – success, OK állapot
// ------------------------------------------------------------
constexpr int GreenStandardR = 212, GreenStandardG = 237, GreenStandardB = 218;
constexpr int GreenSuperR    = 195, GreenSuperG    = 230, GreenSuperB    = 203;

inline const QColor ColorGreenStandard{GreenStandardR, GreenStandardG, GreenStandardB};
inline const QColor ColorGreenSuper{GreenSuperR, GreenSuperG, GreenSuperB};

// ------------------------------------------------------------
// Narancs – általános figyelmeztetés
// ------------------------------------------------------------
constexpr int OrangeR = 255, OrangeG = 165, OrangeB = 0;
inline const QColor ColorOrange{OrangeR, OrangeG, OrangeB};

// ------------------------------------------------------------
// LogViewAdapter specifikus színek
// ------------------------------------------------------------

// ERROR:
inline const QColor ColorError{Qt::red};

inline const QColor ColorRed{Qt::red};


// WARN: (#D98E00)
constexpr int WarnR = 217, WarnG = 142, WarnB = 0;
inline const QColor ColorWarn{WarnR, WarnG, WarnB};

// INFO:
inline const QColor ColorInfo{Qt::darkGreen};

// ------------------------------------------------------------
// Fekete – szöveg
// ------------------------------------------------------------
inline const QColor TextBlack{Qt::black};

// ------------------------------------------------------------
// Cián – információs jelzések
// ------------------------------------------------------------
constexpr int CyanR = 0, CyanG = 180, CyanB = 180;
inline const QColor ColorCyan{CyanR, CyanG, CyanB};

// ------------------------------------------------------------
// Szürke – semleges keretek, outline-ok
// ------------------------------------------------------------
constexpr int GrayR = 136, GrayG = 136, GrayB = 136;
inline const QColor ColorGray{GrayR, GrayG, GrayB};

} // namespace ColorConstants
