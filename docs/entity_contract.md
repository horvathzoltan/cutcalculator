```markdown
# Entity Contract v1  
**CutCalculator – Domain Entitások és Alapmodellek**

---

## 🎯 Cél

Ez a dokumentum a CutCalculator **domain entitásainak** és **alapmodelljeinek** szerződését írja le.

Célok:

- világosan definiálni, **milyen entitások léteznek**,
- megmutatni, **melyik registry melyik entitással dolgozik**,
- rögzíteni az **öröklési hierarchiát**, **invariánsokat** és **azonosító‑szerződéseket**,
- segíteni a fejlesztőket abban, hogy **következetesen bővítsék** a modellt.

Ez a dokumentum **párja** a `registry_contract.md`‑nek:  
ott a registryk szerződése, itt az entitásoké.

---

# 1️⃣ Alap entitás‑modell réteg

Ez a réteg adja az összes domain entitás közös alapját.

## 1.1 Entity

```cpp
class Entity {
public:
    virtual ~Entity() = default;
};
```

**Szerep:**

- üres, virtuális ős,
- minden domain entitás ebből indul,
- így a registryk `Entity*`‑ként is kezelhetik az objektumokat.

### Ábra – Entity mint gyökér

```
Entity
  ▲
  │
  ├─ IRegistryEntity
  └─ (egyéb specifikus típusok)
```

---

## 1.2 IRegistryEntity

```cpp
class IRegistryEntity {
public:
    virtual ~IRegistryEntity() = default;

    virtual QString displayName() const = 0;
    virtual QString shortId() const = 0;
    virtual QString toString() const = 0;
};
```

**Szerep:**

- egységes megjelenési API a GUI és logolás számára,
- minden „publikus” entitásnak meg kell mondania:
  - **displayName** – listákban, UI‑ban,
  - **shortId** – rövid azonosító (pl. első 8 char),
  - **toString** – teljes technikai reprezentáció.

---

## 1.3 IdentifiableEntity

```cpp
struct IdentifiableEntity : public Entity, public IRegistryEntity {
    using IdType = QUuid;
    QUuid id;
    QString name;

    QString displayName() const override {
        return name;
    }

    QString shortId() const override {
        return id.toString(QUuid::WithoutBraces).left(8);
    }

    QString toString() const override {
        return QString("IdentifiableEntity{id=%1, name=%2}")
            .arg(id.toString(), name);
    }
};
```

**Szerep:**

- minden olyan entitás, ami:
  - technikai azonosítóval (`id`) rendelkezik,
  - emberbarát névvel (`name`),
- egyértelmű szerződést ad:
  - `IdType = QUuid`
  - `id` mindig betöltve / generálva (registry hook gondoskodik róla).

### Ábra – IdentifiableEntity

```
          Entity
            ▲
            │
    IRegistryEntity
            ▲
            │
   IdentifiableEntity
       (id + name)
```

**Invariánsok:**

- `id` nem lehet null a persistált entitásoknál,
- `name` UI‑ban is megjeleníthető string.

---

# 2️⃣ Vonalkód‑ és hierarchia‑alapok

## 2.1 BarcodeBase

```cpp
struct BarcodeBase {
    QString barcode;
};
```

**Szerep:**

- opcionális fizikai azonosító (pl. nyomtatható barcode),
- nem kötelező minden entitásnál,
- a barcode workflow (BarcodeRegistry) ettől függetlenül működik.

---

## 2.2 BarcodeIdentifiableEntity

```cpp
struct BarcodeIdentifiableEntity : public IdentifiableEntity,
                                  public BarcodeBase
{
    QString displayName() const override {
        return barcode.isEmpty()
                   ? name
                   : QString("%1 [%2]").arg(name, barcode);
    }

    QString toString() const override {
        return QString("{id=%1, name=%2, barcode=%3}")
            .arg(id.toString(), name, barcode);
    }
};
```

**Szerep:**

- `id + name + barcode` kombinált modell,
- ideális törzsregiszterekhez:
  - MaterialMaster,
  - ProductMaster (közvetve),
- UI‑barát display: `Name [BARCODE]`, ha van barcode.

### Ábra – BarcodeIdentifiableEntity

```
          Entity
            ▲
            │
    IRegistryEntity
            ▲
            │
   IdentifiableEntity
            ▲
            │
      BarcodeBase
            ▲
            │
 BarcodeIdentifiableEntity
 (id + name + barcode)
```

---

## 2.3 HierarchicalBase

```cpp
struct HierarchicalBase {
    QUuid parentId;

    bool isRoot() const {
        return parentId.isNull();
    }

    virtual QString hierarchyPath() const = 0;
};
```

**Szerep:**

- fa‑szerkezetű entitások alapja (pl. ProductMaster),
- szülő‑gyerek viszony a `parentId` mezővel,
- `hierarchyPath()` UI‑barát szöveges útvonal.

---

## 2.4 BarcodeHierarchicalEntity

```cpp
struct BarcodeHierarchicalEntity : public BarcodeIdentifiableEntity,
                                   public HierarchicalBase
{
    QString hierarchyPath() const override {
        return isRoot()
            ? name
            : parentId.toString(QUuid::WithoutBraces) + " → " + name;
    }
};
```

**Szerep:**

- egyesíti:
  - `id + name + barcode` (BarcodeIdentifiableEntity),
  - `parentId + hierarchyPath` (HierarchicalBase),
- ideális hierarchikus, vonalkódos törzsadatokhoz:
  - ProductMaster.

### Ábra – BarcodeHierarchicalEntity

```
                Entity
                  ▲
                  │
          IRegistryEntity
                  ▲
                  │
         IdentifiableEntity
                  ▲
                  │
            BarcodeBase
                  ▲
                  │
    BarcodeIdentifiableEntity
                  ▲
                  │
          HierarchicalBase
                  ▲
                  │
      BarcodeHierarchicalEntity
 (id + name + barcode + parentId)
```

---

# 3️⃣ Domain entitások

## 3.1 MaterialMaster

```cpp
struct MaterialMaster : public BarcodeIdentifiableEntity {
    MaterialMaster(){};

    double stockLength_mm = 0.0;

    CrossSectionShape shape;
    double diameter_mm = 0.0;
    QSizeF size_mm;

    MaterialType type;
    double weightPerStock_kg = 0.0;
    QString defaultMachineId;

    NamedColor color;

    PaintingMode paintingMode = PaintingMode::Paintable;
    CuttingMode  cuttingMode  = CuttingMode::Length;

    QString comment;
};
```

**Szerep:**

- anyag törzsadat,
- MaterialRegistry tárolja,
- barcode + name + domain mezők.

**Invariáns jellegű elvárások (domain hookokban):**

- `stockLength_mm > 0`,
- forma + méret mezők konzisztenciája (shape/dimenziók),
- `type` nem Unknown,
- `color.isValid()`.

---

## 3.2 ProductMaster

```cpp
struct ProductMaster : public BarcodeHierarchicalEntity {
    // későbbi bővítéshez előkészítve
};
```

**Szerep:**

- hierarchikus terméktípus fa,
- ProductRegistry tárolja,
- örökli:
  - `id`, `name`, `barcode`,
  - `parentId`, `hierarchyPath`.

**Invariánsok:**

- gyökérelem: `parentId.isNull()`,
- nem gyökér: `parentId` érvényes ProductMaster ID.

---

## 3.3 NeedCalculation

```cpp
struct NeedCalculation : public IdentifiableEntity {
    QUuid productId;  // kapcsolat a ProductMaster-hez
};
```

**Szerep:**

- egy termékhez tartozó számítási mód definíció,
- NeedCalculationRegistry tárolja,
- `name` a mód neve (pl. „Teljes méret”, „Gyártási méret”).

**Invariánsok:**

- `!name.isEmpty()`,
- `productId` érvényes ProductMaster ID.

---

## 3.4 NeedCalculationDetail

```cpp
struct NeedCalculationDetail : public IdentifiableEntity {
    QUuid needCalculationId;
    QUuid materialId;
    QString formula;
};
```

**Szerep:**

- egy számítási módon belül:
  - melyik anyaghoz,
  - milyen formulával számolunk,
- NeedCalculationDetailRegistry tárolja.

**Invariánsok (domain hook enforcement):**

- `needCalculationId` érvényes NeedCalculation ID,
- `materialId` érvényes MaterialMaster ID,
- `formula` szintaxisa helyes (pl. `"w-15"`, `"fixed:2"`, `"h-10"`).

---

## 3.5 NeedRule (ConnectionEntity<ProductMaster, MaterialMaster>)

```cpp
using NeedRule = ConnectionEntity<ProductMaster, MaterialMaster>;
```

**Szerep:**

- Product ↔ Material kapcsolat,
- NeedRuleRegistry tárolja,
- bal oldal: ProductMaster,
- jobb oldal: MaterialMaster.

---

### ConnectionEntity alap

```cpp
template<typename LeftEntity, typename RightEntity>
struct ConnectionEntity {
    using IdType = QUuid;

    QUuid leftId{};
    QUuid rightId{};

    std::optional<LeftEntity> left() const { ... }
    std::optional<RightEntity> right() const { ... }

    QString leftBarcode() const { ... }
    QString rightBarcode() const { ... }

    QString describe() const {
        QString l = leftId.toString(QUuid::WithBraces);
        QString r = rightId.toString(QUuid::WithBraces);
        return QString("Connection{ left=%1, right=%2 }").arg(l, r);
    }
};
```

**Szerep:**

- általános kapcsolótábla sor,
- lookupot végez a megfelelő registrykben,
- NeedRule a konkrét példány (Product↔Material).

### Ábra – NeedRule

```
NeedRule
  = ConnectionEntity<ProductMaster, MaterialMaster>

 leftId  → ProductMaster.id
 rightId → MaterialMaster.id
```

---

## 3.6 BarcodeRecord

---

## 3.6.1 Barcode workflow szerződés (Validator + Ledger)

A BarcodeRecord önmagában csak az életút-modell.  
A barcode workflow teljes szerződése három komponensből áll:

---

## 3.6.2 Naming konzisztencia – CSV vs. belső mezőnevek

A barcode mező elnevezése rétegenként eltér, de a jelentése azonos.

### CSV szerződés (külső adatcsere)
- mező neve: **`barCode`**
- szerepe: a ledger export/import formális mezőneve
- fix, visszafelé kompatibilis formátum

### Belső modell (BarcodeRecord)
- mező neve: **`code`**
- szerepe: a barcode tényleges értéke a ledger‑modellben
- a `barCode` CSV mező erre a mezőre mapelődik

### Domain entitások
- mező neve: **`barcode`**
- szerepe: az entitás aktuális, ember által látható vonalkódja
- a BarcodeValidator gondoskodik a ledger‑szinkronról

### Összefoglaló
- **CSV → `barCode`**  
- **Ledger → `code`**  
- **Domain entitás → `barcode`**

A három név különböző rétegeket jelöl, de ugyanazt a fogalmat hordozza.

### 1) BarcodeValidator – a központi kapu
- minden új barcode ezen keresztül kerül regisztrálásra,
- üres kód tiltása,
- globális uniqueness ellenőrzése,
- CSV audit-policy: `ctx.addError` (sorhoz kötött hiba),
- UI audit-policy: `zEventERROR` (felhasználói audit esemény),
- Ledger audit-policy: `zWarning` (rendszerszintű audit figyelmeztetés).

### 2) BarcodeRegistry – globális életút ledger
- append-only ledger (insertInternal + updateInternal),
- nincs remove művelet,
- retiredAt időben monoton,
- domain-validáció NEM itt történik,
- CSV szerződés fix: `barCode;entityType;introducedAt;retiredAt`.

### 3) Domain entitások – csak a barcode mezőt hordozzák
- az entitások NEM írhatnak közvetlenül a BarcodeRegistry-be,
- minden módosítás a Validatoron keresztül történik,
- a barcode mező opcionális, de ha van → auditált életút tartozik hozzá.


```cpp
struct BarcodeRecord {
    using IdType = QUuid;
    QUuid id;
    QString code;
    QString entityType;
    std::optional<QUuid> entityId;
    QDateTime introducedAt;
    std::optional<QDateTime> retiredAt;

    enum class Status { Active, Retired };

    Status status() const {
        return retiredAt.has_value() ? Status::Retired : Status::Active;
    }

    bool isActive() const { return status() == Status::Active; }
};
```

**Szerep:**

- auditbarát életút modell egy barcode‑hoz,
- BarcodeRegistry tárolja,
export/import szerződés (ledger CSV):
  - `barCode;entityType;introducedAt;retiredAt`
  - `entityId` NEM része a CSV-nek (csak runtime trace)

**Invariánsok:**

- `code` nem üres,
- `introducedAt` érvényes időpont,
- ha `retiredAt` jelen van → `status() == Retired`.

---

## 3.7 NamedColor

```cpp
class NamedColor {
public:
    using IdType = QString;
    IdType id;

    NamedColor() = default;
    NamedColor(const QColor& color, const QString& name);
    NamedColor(const QString& code);
    NamedColor(const QColor& color, const QString& name,
               const QString& code, RalSystem system);

    QColor color() const;
    QString name() const;
    QString code() const;
    RalSystem system() const;

    static NamedColor fromRal(const QString& ralCode);
    static NamedColor fromHex(const QString& hexCode);

    QString toString() const;
    bool isValid() const;

    struct NormalizedRal {
        QString key;
        RalSystem system;

        bool isValid() {
            if (key.isEmpty()) return false;
            if (system == RalSystem::Unknown) return false;
            return true;
        }
    };

    static NormalizedRal normalizeRalExtended(const QString& raw);

    bool operator==(const NamedColor& other) const {
        return m_code == other.m_code
               && m_system == other.m_system;
    }

private:
    QColor m_color;
    QString m_name;
    QString m_code;
    RalSystem m_system = RalSystem::Unknown;
};
```

**Szerep:**

- színmodell a ColorRegistry számára,
- `id` technikai azonosító (pl. code),
- domain logika:
  - HEX/RAL normalizálás,
  - `NormalizedRal` típus,
  - `isValid()` ellenőrzés.

---

# 4️⃣ Entitások és registryk kapcsolata

| Entitás                  | Registry                    | Típus      |
|--------------------------|-----------------------------|-----------|
| MaterialMaster           | MaterialRegistry            | Domain / Technikai keverék |
| ProductMaster            | ProductRegistry             | Domain + Hierarchia |
| NeedRule                 | NeedRuleRegistry            | Kapcsolat |
| NeedCalculation          | NeedCalculationRegistry     | Domain |
| NeedCalculationDetail    | NeedCalculationDetailRegistry | Domain |
| BarcodeRecord            | BarcodeRegistry             | Audit |
| NamedColor               | ColorRegistry               | Technikai |

---

# 5️⃣ Entitás invariánsok összefoglalása

- **IdentifiableEntity**:  
  - `id` nem null persistált állapotban  
  - `name` nem üres domain szinten  

- **BarcodeIdentifiableEntity**:  
  - `barcode` opcionális, de ha van, egyedi vagy auditált  

- **ProductMaster**:  
  - `parentId` vagy null (gyökér), vagy érvényes ProductMaster ID  

- **MaterialMaster**:  
  - `stockLength_mm > 0`  
  - forma/dimenziók konzisztenciája  
  - `color.isValid()`  

- **NeedCalculation**:  
  - `!name.isEmpty()`  
  - `productId` érvényes  

- **NeedCalculationDetail**:  
  - `needCalculationId` érvényes  
  - `materialId` érvényes  
  - `formula` szintaxisa helyes  

- **NeedRule**:  
  - `(leftId, rightId)` pár egyedi adott registryben  

- **BarcodeRecord**:  
  - `code` globálisan auditált  
  - `introducedAt` mindig kitöltött  
  - `retiredAt` → status = Retired  

---

# 6️⃣ Zárás

Ez a dokumentum:

- lefedi a CutCalculator **összes domain entitását**,  
- megmutatja az **öröklési viszonyokat**,  
- rögzíti az **invariánsokat**,  
- és egyértelművé teszi, hogy **melyik registry milyen entitással dolgozik**.

Ez a kép együtt a `registry_contract.md`‑vel:

- a rendszeredet **modell‑szinten is ipari minőségűvé** teszi,
- és kiváló alap lesz bármilyen további refaktor, migráció vagy onboarding számára.
```
