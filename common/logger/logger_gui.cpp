/* ============================================================
 * 🧩 Logger GUI helpers – implementation
 * ============================================================ */
#include "logger_gui.h"
#include <QMessageBox>

namespace LoggerGui {
void show_critical_dialog(const QString& msg) {
    QMessageBox::critical(nullptr, QStringLiteral("Hiba"), msg);
}
}

