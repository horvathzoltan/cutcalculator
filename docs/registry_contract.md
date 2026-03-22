### Cél

Rövid, gépileg olvasható és ember által is olvasható szerződés a registry mixinek és hostok közötti API‑ról, valamint a viselkedési elvárásokról. A dokumentum célja, hogy a `CrudMixin`, `RegistryEngineBase` és a host registryk (például `MaterialRegistry`) közötti integrációt egyértelműen definiálja, minimalizálja az API mismatch kockázatot, és meghatározza a threading, hibakezelés és migráció szabályait.

---

### Host Contract metódusok és signatúrák

**Kötelező metódusok** — a hostnak ezeknek a signatúráknak pontosan meg kell felelniük:

```cpp
// RegistryEngineBase által biztosított
SubscriptionId subscribeItemsChanged(ItemsChangedEvent cb);
void unsubscribeItemsChanged(SubscriptionId id);
SubscriptionToken subscribeItemsChangedToken(ItemsChangedEvent cb);

// Store és read API
bool add(const Entity& e);
bool updateInternal(const Entity& e);
bool removeInternal(const IdType& id);
const Entity* findById(const IdType& id) const;
QVector<Entity> readAll() const;
int size() const;

// Persist és lifecycle
void persistRegistry() const; // constness kötelező
void initialize();
bool isInitialized() const;
bool isRegistered() const;

// Domain hookok
bool validateDomain(const Entity& e) const;
bool validateDuplicate(const Entity& e) const;
bool beforeInsert(Entity& e);
void afterInsert(const Entity& e);
bool beforeUpdate(Entity& e);
void afterUpdate(const Entity& e);
bool beforeRemove(Entity& e);
void afterRemove(const Entity& e);
void onInsertLog(const Entity& e);
void onUpdateLog(const Entity& e);
void onRemoveLog(const Entity& e);
```

**Ajánlott, opcionális metódusok**  
- `bool validateConnection(const Entity& e) const` — connection táblákhoz.  
- `const Entity* findByPair(const QUuid& left, const QUuid& right) const` — pair lookup.  
- `void onLoadLog()` — import log hook.

---

### Viselkedési elvárások és invariánsok

- **Élettartam és ownership**
  - A host garantálja, hogy a `findById` által visszaadott `const Entity*` érvényes marad, amíg a registry nem módosul. Minden író művelet (add, removeInternal, setAll, append) potenciálisan invalidálja a pointereket.
  - Ha tokenek RAII módon unsubscribe‑olnak, a token destruktora csak akkor hívja `unsubscribe`‑t, ha a registry még él. Ha a registry nem singleton, használjatok `subscribeItemsChangedTokenShared`-et shared ownership mellett.

- **Threading**
  - Olvasó metódusok (`readAll`, `findById`, `existsBy`, `size`) **shared/read lock** alatt futnak.
  - Író metódusok (`add`, `updateInternal`, `removeInternal`, `setAll`) **exclusive/write lock** alatt futnak.
  - `onItemsChanged` készít zárolt másolatot a feliratkozó listáról, majd hívja a callbackeket zárolás nélkül. Callbackek kivételét el kell nyelni, hogy a registry stabil maradjon.

- **Hook viselkedés**
  - `beforeInsert` és `beforeUpdate` kapjon `Entity&`‑t; a mixin mindig lokális másolatot ad át, így a hook módosíthatja az entitást.
  - `validateDomain` és `validateDuplicate` legyen `const` és gyors; ha komplex validáció szükséges, aszinkron ellenőrzést vagy előfeldolgozást használjatok.

- **Persist és tranzakció**
  - `persistRegistry()` egy egyszerű, végrehajtandó hook; ha a persist kritikus, vezessetek be `TransactionPolicy` mixint később, amely `beginTransaction`, `commit`, `rollback` metódusokat ad.

---

### Hibakezelés és visszavonás

- **Kivételkezelés**
  - Mixin `runOperation` elnyeli a kivételeket, logolja őket és `false`‑t ad vissza. Host metódusok ne dobjanak kivételt rutin hibák esetén; ha dobniuk kell, a mixin kezelje a logolást és a biztonságos visszatérést.
- **Rollback**
  - Jelenlegi pilot nem követeli meg automatikus rollbacket. Ha szükséges, a következő lépés egy `TransactionPolicy` bevezetése, amely a `CrudMixin` által használt műveleteket tranzakciós kontextusba helyezi.
- **Idempotencia**
  - `remove` és `add` műveletek legyenek determinisztikusak: ismételt hívás ugyanarra az inputra ne okozzon nem várt állapotot.

---

### Static checks és fordítási idői garanciák

- **C++20 concept alapú ellenőrzés**
  - A `CrudMixin` használjon `concept`‑okat a host‑metódusok meglétének ellenőrzésére. A hiányzó metódusok fordítási hibát adjanak, egyértelmű üzenettel.
- **CI szabály**
  - Minden PR‑ben fusson le a build C++20 beállítással; a `CrudMixin` concept ellenőrzései legyenek részei a buildnek.

---

### Migrációs lépések és elfogadási kritériumok

**Lépések**
1. Commit és PR a `subscription_token.h` és `RegistryEngineBase` módosításokkal.  
2. Commit és PR a `crud_mixin.h` C++20 verzióval.  
3. Pilot migráció: `MaterialRegistry` frissítése és smoke test futtatása.  
4. Unit tesztek a domain hookokra és subscribe/unsubscribe viselkedésre.  
5. CI integráció és PR review.  
6. Iteratív migráció további registrykre, egyenként: implementáció, unit+smoke teszt, PR.

**Elfogadási kritériumok**
- `CrudMixin` compile‑time ellenőrzései zöldek a hoston.  
- MaterialRegistry smoke test zöld: insert → findById → update → remove.  
- Subscribe/unsubscribe concurrency smoke teszt nem dob crash‑t.  
- CI pipeline lefut minden PR‑re és a smoke teszt zöld.

---

### Fájlok és változtatások listája

| Fájl | Művelet | Prioritás |
|---|---|---:|
| common/registry/subscription_token.h | új fájl | Magas |
| common/registry/base/registry_engine_base.h | mutex/lock és token API módosítás | Magas |
| common/registry/mixins/crud_mixin.h | C++20 concept verzió | Magas |
| materials/registry/material_registry.h | migrációs példa | Közepes |
| materials/registry/material_registry.cpp | migrációs példa | Közepes |
| tests/material_registry_smoke_test.cpp | smoke test | Magas |
| docs/registry_contract.md | ez a dokumentum | Magas |

---

### Rövid példa kód és használati minták

**Feliratkozás RAII tokennel**
```cpp
auto token = reg.subscribeItemsChangedToken([](){ /* callback */ });
// token scope végekor automatikus unsubscribe
```

**Insert használat**
```cpp
MaterialMaster m = /* kitöltés */;
if (!MaterialRegistry::instance().insert(m)) {
    // log vagy hiba kezelés
}
```

**Hook implementáció példa**
```cpp
bool MaterialRegistry::beforeInsert(MaterialMaster& m) {
    if (m.id.isNull()) m.id = QUuid::createUuid();
    m.name = m.name.trimmed();
    return true;
}
```

---

## BarcodeRegistry – különálló ledger (nem domain registry)

A BarcodeRegistry NEM domain registry és NEM használ CrudWorkflowMixin‑t.

Jellemzői:

- append‑only ledger (insertInternal + updateInternal)
- nincs remove művelet
- nincs domain‑szintű validateDomain / validateDuplicate
- nincs CrudWorkflowMixin
- minden módosítás a BarcodeValidatoron keresztül történik
- CSV szerződés 4 mezővel:  
  `barCode;entityType;introducedAt;retiredAt`

A BarcodeRegistry architektúrája teljesen elkülönül a domain registryktől.

---

## Ledger-specifikus registryk (BarcodeRegistry)

A BarcodeRegistry NEM követi a CrudMixin / RegistryEngineBase host contractot.
A BarcodeRegistry külön ledger-modell szerint működik, saját invariánsokkal:

- append-only ledger (insertInternal + updateInternal)
- nincs remove művelet
- retiredAt időben monoton (csak előre haladhat)
- globális uniqueness (code + entityType)
- domain-validáció NEM itt történik, hanem a BarcodeValidatorban
- minden módosítás a Validatoron keresztül történik
- CSV szerződés fix: barCode;entityType;introducedAt;retiredAt

A BarcodeRegistry architektúrája teljesen elkülönül a domain registryktől.

