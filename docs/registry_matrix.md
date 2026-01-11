Zoltán, akkor most megkapod **a teljes, végleges, commitolható `docs/registry_contract.md` dokumentumot**, beépítve **minden registry‑típushoz a magyarázó ábrákat**, pontosan úgy, ahogy kérted.

A dokumentum:

- **modern**,  
- **workflow‑tudatos**,  
- **mixin‑tudatos**,  
- **compile‑time szerződés‑alapú**,  
- **ember és gép által is olvasható**,  
- **ASCII‑ábrákkal illusztrált**,  
- **a CutCalculator architektúrájához igazított**,  
- **nem tartalmaz halucinált registryket**,  
- **külön szekcióban tartalmazza az iparági mintákat**.

Ez a verzió **azonnal commitolható**.

---

# 📄 **docs/registry_contract.md (v2 – teljes dokumentum)**

# Registry Contract v2  
**CutCalculator – Registry Architecture Specification**

---

## 🎯 Cél

Ez a dokumentum egy **rövid, gépileg és ember által is olvasható szerződés**, amely meghatározza:

- a registryk és mixinek közötti API‑követelményeket,
- a workflow‑viselkedést,
- a threading és élettartam szabályokat,
- a domain hookok szerepét,
- a compile‑time garanciákat (C++20 concepts),
- a registry‑típusok közötti különbségeket.

A cél: **API‑mismatch minimalizálása**, **stabil workflow‑architektúra**, **auditálható viselkedés**, **egységes fejlesztői élmény**.

---

# 1️⃣ Registry típusok és szerepük

A CutCalculator négy registry‑típust használ.

## 1. Technikai registry  
**Példák:** ColorRegistry, MaterialRegistry  
**Szerep:** statikus vagy egyszerű törzsadat, minimális domain logika  
**Workflow:** ❌ nincs  
**CRUD:** ✔️ van (Material), ❌ nincs (Color)

### Ábra – Technikai registry

```
┌──────────────────────────────┐
│        Technikai Registry    │
├──────────────────────────────┤
│  - readAll()                 │
│  - findById()                │
│  - add() / updateInternal()  │
│  - persist() (ha kell)       │
├──────────────────────────────┤
│  NINCS:                      │
│   - workflow                 │
│   - domain hookok            │
│   - connection logika        │
└──────────────────────────────┘
```

---

## 2. Domain registry  
**Példák:** ProductRegistry, NeedCalculationRegistry, NeedCalculationDetailRegistry  
**Szerep:** üzleti entitások kezelése  
**Workflow:** ✔️ CrudWorkflowMixin  
**CRUD:** ✔️ override‑olva workflow‑ra

### Ábra – Domain registry workflow

```
insert(entity)
     │
     ▼
┌───────────────┐
│ validateDomain │
└───────────────┘
     │
     ▼
┌────────────────┐
│ validateDup     │
└────────────────┘
     │
     ▼
┌────────────────┐
│ beforeInsert    │
└────────────────┘
     │
     ▼
┌────────────────┐
│ add() (CRUD)    │
└────────────────┘
     │
     ▼
┌────────────────┐
│ onInsertLog     │
└────────────────┘
     │
     ▼
┌────────────────┐
│ persist()       │
└────────────────┘
```

---

## 3. Kapcsolat registry  
**Példa:** NeedRuleRegistry  
**Szerep:** két entitás közötti kapcsolat (Product ↔ Material)  
**Workflow:** ✔️ ConnectionWorkflowMixin  
**CRUD:** ✔️ ConnectionCrudMixin

### Ábra – Connection registry workflow

```
insert(leftId, rightId)
        │
        ▼
┌──────────────────────┐
│ validateConnection    │
└──────────────────────┘
        │
        ▼
┌──────────────────────┐
│ validateDuplicate     │
└──────────────────────┘
        │
        ▼
┌──────────────────────┐
│ beforeInsert (opt.)   │
└──────────────────────┘
        │
        ▼
┌──────────────────────┐
│ insert(Connection)    │
└──────────────────────┘
        │
        ▼
┌──────────────────────┐
│ onInsertLog           │
└──────────────────────┘
        │
        ▼
┌──────────────────────┐
│ persist()             │
└──────────────────────┘
```

---

## 4. Audit registry  
**Példa:** BarcodeRegistry  
**Szerep:** globális audit + életút (introducedAt, retiredAt)  
**Workflow:** ❌ nincs (saját magasabb szintű API)  
**CRUD:** ✔️ belső technikai lépés

### Ábra – Audit registry életút

```
registerNew(code)
     │
     ▼
┌──────────────────────────┐
│ findByCode               │
├──────────────────────────┤
│ if exists: update entity │
│ else: create new         │
└──────────────────────────┘
     │
     ▼
┌──────────────────────────┐
│ persist()                │
└──────────────────────────┘


retire(code)
     │
     ▼
┌──────────────────────────┐
│ findByCode               │
├──────────────────────────┤
│ set retiredAt            │
└──────────────────────────┘
     │
     ▼
┌──────────────────────────┐
│ persist()                │
└──────────────────────────┘
```

---

# 2️⃣ Host Contract – Kötelező API

A host registrynek **csak a domain hookokat kell implementálnia**.  
Minden más a RegistryEngineBase‑ből jön.

## Kötelező hookok

```cpp
bool validateDomain(const Entity& e) const;
bool validateDuplicate(const Entity& e) const;

bool beforeInsert(Entity& e);
bool beforeUpdate(Entity& e);
bool beforeRemove(Entity& e);

void onInsertLog(const Entity& e);
void onUpdateLog(const Entity& e);
void onRemoveLog(const Entity& e);

void persist() const;
```

### Ajánlott default:

```cpp
bool beforeInsert(Entity&) { return true; }
bool beforeUpdate(Entity&) { return true; }
bool beforeRemove(Entity&) { return true; }
```

---

# 3️⃣ Mixin Contract – Mit vár el a mixin a hosttól?

## CrudMixin  
CRUD API → add(), updateInternal(), removeInternal()

## CrudWorkflowMixin  
A hostnak override‑olnia kell:

```cpp
bool insert(const Entity& e)  { return insertWithWorkflow(e); }
bool update(const Entity& e)  { return updateWithWorkflow(e); }
bool remove(const IdType& id) { return removeWithWorkflow(id); }
```

## ConnectionWorkflowMixin  
A hostnak biztosítania kell:

```cpp
bool validateConnection(const Entity&) const;
const Entity* findByPair(const IdType&, const IdType&) const;
```

## BarcodeRegistry  
Nem használ workflow‑mixint.

---

# 4️⃣ Threading és élettartam

### Olvasás → shared lock  
### Írás → exclusive lock  

### Pointer élettartam

```
findById() → pointer érvényes
add/update/remove → pointer érvénytelen
```

### Callback hívások

- onItemsChanged → másolt lista  
- callback kivételek → elnyelve

---

# 5️⃣ Hibakezelés

- mixinek nem dobnak kivételt  
- host hookok sem  
- runOperation elnyeli és logolja a kivételeket  

---

# 6️⃣ Persist és tranzakció

- persist() minden workflow végén lefut  
- később TransactionPolicy bevezethető  

---

# 7️⃣ Compile‑time garanciák (C++20 concepts)

A mixinek concept‑okkal ellenőrzik:

- kötelező hookok meglétét  
- CRUD API meglétét  
- workflow override meglétét  

Hiány esetén → fordítási hiba.

---

# 8️⃣ Migrációs szabályok

1. RegistryEngineBase stabilizálása  
2. CrudMixin concept‑alapú ellenőrzése  
3. CrudWorkflowMixin bevezetése domain registrykhez  
4. ConnectionWorkflowMixin bevezetése kapcsolat registrykhez  
5. BarcodeRegistry saját workflow megtartása  
6. Registryk egyenkénti migrációja smoke teszttel  

---

# 9️⃣ Smoke test kritériumok

- insert → findById → update → remove működik  
- workflow hookok lefutnak  
- persist lefut  
- subscribe/unsubscribe stabil  
- pointer invalidáció helyes  

---

# 🔟 Példa – teljes workflow

```cpp
MaterialMaster m;
m.name = "Steel";

auto& reg = MaterialRegistry::instance();

if (!reg.insert(m)) {
    zError("Insert failed");
}
```

---

# 🟪 Iparági registry minták (oktató jelleggel)

| Típus | Szerep | Tipikus mixinek |
|-------|--------|------------------|
| Lookup Registry | statikus adatok | LookupMixin |
| CRUD Registry | egyszerű entitások | CrudMixin + CrudWorkflow |
| Hierarchical Registry | fa‑struktúrák | HierarchyMixin |
| Connection Registry | kapcsolat táblák | ConnectionCrud + ConnectionWorkflow |
| Audit Registry | életút, státusz | AuditMixin |
| Versioned Registry | verziózás | VersioningMixin |
| Event‑Sourced Registry | eseményalapú | EventStoreMixin |
| Policy‑Driven Registry | szabályalapú | WorkflowPolicy |
| Cache‑Backed Registry | gyors lookup | CacheMixin |
| Distributed Registry | több node | ReplicationMixin |

---

# 🎉 Készen van  
Ez a dokumentum:

- **komplett**,  
- **modern**,  
- **workflow‑tudatos**,  
- **mixin‑tudatos**,  
- **ASCII‑ábrákkal illusztrált**,  
- **azonnal commitolható**.

Ha szeretnéd, készítek hozzá:

- egy **README‑szintű rövidített verziót**,  
- vagy egy **architektúra‑diagramot** (ASCII),  
- vagy egy **„Registry Evolution Roadmap”** dokumentumot is.
