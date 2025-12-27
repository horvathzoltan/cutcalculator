#pragma once

#include <QObject>
#include <QPointer>
#include <QTimer>

/* ============================================================
 * 🧩 AttentionBlinker – finom figyelemfelhívó keret villogtatás
 * ============================================================ */

class AttentionBlinker : public QObject {
    Q_OBJECT

public:
    explicit AttentionBlinker(QObject* parent = nullptr);

    // A widget, amelyre a villogtatás hatni fog
    void attach(QWidget* target);

    enum StartMode{Dark, Light};
    enum BlinkModeMode{GrayMode, GreenMode};

    // Villogtatás indítása
    void start(StartMode mode, BlinkModeMode blinkMode);
    void flashOnce();

    // Villogtatás leállítása (pl. user interakció esetén)
    void stop();

private slots:
    void onBlinkTick();

private:
    /* ============================================================
     * 📦 Belső állapot
     * ============================================================ */
    QPointer<QWidget> _target;
    QTimer _timer;
    int _phase = 0;     // 0..4 ciklus, 0 = light, 1-4 = dark
    bool _running = false;
    BlinkModeMode _blinkMode = GreenMode;

    /* ============================================================
     * 🔧 Segédfüggvények
     * ============================================================ */
    void applyDark();
    void applyLight();
    void applyOriginal();
    void applyPhase();
};
