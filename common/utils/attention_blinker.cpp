#include "attention_blinker.h"
#include <QRandomGenerator>
#include <QWidget>

/* ============================================================
 * ⚙️ Konstruktor
 * ============================================================ */
AttentionBlinker::AttentionBlinker(QObject* parent)
    : QObject(parent)
{
    _timer.setInterval(300);      // 300 ms fázisidő
    _timer.setSingleShot(false);  // folyamatos ciklus
    connect(&_timer, &QTimer::timeout, this, &AttentionBlinker::onBlinkTick);
}

/* ============================================================
 * 🧩 Widget hozzárendelése
 * ============================================================ */
void AttentionBlinker::attach(QWidget* target) {
    _target = target;
}

/* ============================================================
 * 🧩 Villogtatás indítása
 * ============================================================ */
void AttentionBlinker::start(StartMode mode, BlinkModeMode blinkMode) {
    if (!_target || _running)
        return;

    if(mode == Dark){
         _phase =0;//QRandomGenerator::global()->bounded(0, 4);
    //     applyDark();
    } else {
         _phase = 1; // következő tick már dark lesz
    //     applyLight(); // induljon világossal
    }

    _running = true;
    _blinkMode = blinkMode;

    applyPhase();
    _timer.start();
}

/* ============================================================
 * 🧩 Egy villantás végrehajtása
 * ============================================================ */
void AttentionBlinker::flashOnce() {
    if (!_target)
        return;

    applyLight(); // világos border
    QTimer::singleShot(200, this, [this]() {
        applyOriginal();
    });
}

/* ============================================================
 * 🧩 Villogtatás leállítása
 * ============================================================ */
void AttentionBlinker::stop() {
    if (!_running)
        return;

    _running = false;
    _timer.stop();

    // Keret eltávolítása
    applyOriginal();
}

/* ============================================================
 * 🧩 Fázislogika – 300 ms-onként fut
 * ============================================================ */
void AttentionBlinker::onBlinkTick() {
    if (!_running || !_target)
        return;

    applyPhase();
    // Következő fázis
    _phase++;

    // 5 fázis után új ciklus
    if (_phase >= 5)
        _phase = 0;
}

void AttentionBlinker::applyPhase() {
    // Light phase: minden 4. tick
    if (_phase % 4 == 0) {
        applyLight();
    } else {
        if(_blinkMode == GrayMode)
            applyOriginal();
        else
            applyDark();
    }

}


/* ============================================================
 * 🎨 Sötét keret
 * ============================================================ */
void AttentionBlinker::applyDark() {
    if (_target) {
        _target->setStyleSheet(
            "QDialog { border: 6px solid #007A33; }"
            );
    }
}

/* ============================================================
 * 🎨 Világos keret
 * ============================================================ */
void AttentionBlinker::applyLight() {
    if (_target) {
        _target->setStyleSheet(
            "QDialog { border: 6px solid #00CC55; }"
            );
    }
}

/* ============================================================
 * 🎨 Eredeti keret
 * ============================================================ */
void AttentionBlinker::applyOriginal() {
    if (_target) {
        _target->setStyleSheet("");   // visszaáll a natív keret
    }
}




