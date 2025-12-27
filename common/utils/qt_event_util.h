// #pragma once

// #include <QObject>
// #include <QCoreApplication>
// #include <QEvent>
// #include <functional>
// #include "common/logger/logger.h"
// //
// // 🚀 Minimál lambda-esemény osztály – csak egy QEvent és egy futtatandó lambda
// //
// class LambdaEvent : public QEvent {
// public:
//     using Func = std::function<void()>;

//     explicit LambdaEvent(QObject* receiver, Func func)
//         : QEvent(QEvent::User), m_func(std::move(func)), m_receiver(receiver)  {} // QEvent::User: egyszerű típus

//     void execute() { m_func(); m_executed = true; }

//     ~LambdaEvent() {
//         if (!m_executed) {
//             QString targetClass = m_receiver ? m_receiver->metaObject()->className() : "ismeretlen";
//             QString targetName  = m_receiver ? m_receiver->objectName() : "(nincs objectName)";
//             const char* baseClass = m_receiver && m_receiver->metaObject()->superClass()
//                                         ? m_receiver->metaObject()->superClass()->className()
//                                         : "QObject";

//             zWarning() << "⚠️ LambdaEvent sosem futott le!"
//                        << "\n📋 Megoldási terv:"
//                        << "\n   → Nyisd meg: " << targetClass <<".cpp (objectName=" << targetName << ")"
//                        << "\n   → Írd bele az event(QEvent*) override-ot:"
//                        << "\n=== kód kezdete ==="
//                        << "\n   bool "<< targetClass << "::event(QEvent* e)"
//                        << "\n   {"
//                        << "\n     //🎯 Ha ez egy LambdaEvent, akkor futtatjuk a benne levő lambdát"
//                        << "\n     if (e->type() == QEvent::User) {"
//                        << "\n         auto* le = static_cast<LambdaEvent*>(e);"
//                        << "\n         le->execute();"
//                        << "\n         return true; // jelezzük, hogy kezeltük"
//                        << "\n     }"
//                        << "\n     // 🔄 Egyéb események átadása az alapkezelésnek"
//                        << "\n     return "<<baseClass<<"::event(e); // minden más esemény átadva az alapnak"
//                        << "\n   }"
//                        << "\n=== /kód vége ==="
//                        << "\n   → Nyisd meg: "<< targetClass << ".h"
//                        << "\n   → Deklaráld:"
//                        << "\n=== kód kezdete ==="
//                        << "\n   bool event(QEvent* e) override;"
//                        << "\n=== /kód vége ==="
//                        << "\nEzzel biztosítod, hogy a lambda mindig lefusson."
//                        << "\n⚠️ Ha több osztályban használsz LambdaEvent‑et, mindegyikben implementáld az event() override‑ot.";
//         }
//     }

// private:
//     Func m_func;
//     QObject* m_receiver;
//     bool m_executed = false;
// };

// //
// // 🔧 Használatra kész util namespace, ami lambdákat postol az event queue-ba
// //
// namespace QtEventUtil {

// // 📬 Lambda postolása az objektumnak
// inline void post(QObject* receiver, std::function<void()> func) {
//     QCoreApplication::postEvent(receiver, new LambdaEvent(receiver, std::move(func)));
// }
// }
