// #pragma once
// #include <QUuid>
// #include <QVector>
// #include <algorithm>

// /**
//  * 📚 ConnectionRegistry<T> – generikus in-memory tároló kapcsolatokhoz
//  *
//  * Hunglish: minimál CRUD. Nem perzisztál, nem IO-zik – csak memóriában él.
//  * Nem singleton! A típusos registry (NeedRuleRegistry, ColorRegistry, stb.)
//  * példányosítja és menedzseli.
//  */
// template<typename ConnectionType>
// class ConnectionRegistry {
//     QVector<ConnectionType> _data;

// public:
//     ConnectionRegistry() = default;

//     // Hunglish: teljes tartalom beállítás – repo load után
//     void setData(const QVector<ConnectionType>& v) { _data = v; }

//     // Hunglish: beolvasás – presenter és audit összefoglaló számára
//     const QVector<ConnectionType>& readAll() const { return _data; }

//     // Hunglish: beszúrás – duplikátumot nem tiltjuk, de érdemes ellenőrizni
//     void insert(const ConnectionType& c) { _data.append(c); }

//     // Hunglish: törlés – left/right GUID párral
//     bool remove(const QUuid& left, const QUuid& right) {
//         auto it = std::remove_if(_data.begin(), _data.end(),
//                                  [&](const ConnectionType& c){ return c.leftId==left && c.rightId==right; });
//         if (it != _data.end()) {
//             _data.erase(it, _data.end());
//             return true;
//         }
//         return false;
//     }

//     // Hunglish: szűrés bal szerint – a presenter tipikusan ezt hívja
//     QVector<ConnectionType> findByLeft(const QUuid& left) const {
//         QVector<ConnectionType> res;
//         for (const auto& c : _data) if (c.leftId == left) res.append(c);
//         return res;
//     }

//     // Hunglish: szűrés jobb szerint – ritkább use-case, de legyen meg
//     QVector<ConnectionType> findByRight(const QUuid& right) const {
//         QVector<ConnectionType> res;
//         for (const auto& c : _data) if (c.rightId == right) res.append(c);
//         return res;
//     }

//     // Hunglish: audit convenience – hány kapcsolat van?
//     int size() const { return _data.size(); }
// };
