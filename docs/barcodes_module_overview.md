## *A Barcode modul architekturális szerepe és működése*

# Barcode modul – architekturális áttekintés (v3 – Ledger modell)

A Barcode modul a rendszer globális, auditált **ledger‑e**.  
Feladata: minden vonalkód életútját (introducedAt → retiredAt) megőrizni.

A modul **nem domain registry**, nem CRUD, nem WorkflowMixin‑alapú.

---

## 1. Ledger modell

A BarcodeRegistry:

- append‑only ledger (insertInternal + updateInternal)
- nincs remove művelet
- nincs CrudWorkflowMixin
- nem domain‑szintű validációt végez, hanem ledger‑szintű invariánsokat ellenőriz (code, entityType, introducedAt, retiredAt)
- minden módosítás a Validatoron keresztül történik

továbbá:

- retired barcode SOHA nem használható újra (globális identitás‑invariáns)
- a ledger minden code értéket örökre lefoglal
- a retiredAt mező lezárja az életutat, de nem teszi újra kioszthatóvá

---

## 2. Validator – a központi kapu

A BarcodeValidator biztosítja:

- üres kód tiltása
- globális uniqueness
- ledger‑bejegyzés (registerNew)
- UI audit‑policy: zEventERROR (felhasználói audit esemény)
- CSV audit‑policy: ctx.addError (sorhoz kötött audit hiba)
- Ledger audit‑policy: zWarning (rendszerszintű audit figyelmeztetés)

A domain registryk és a UI **soha nem hívhatják közvetlenül** a BarcodeRegistry‑t.

- ha a ledgerben létezik a kód retired állapotban → a Validator elutasítja a regisztrációt
    - audit: zEventERROR
    - CSV: ctx.addError
    - ledger: változatlan marad
    
---

## 3. BarcodeCollisionHelper – emberi ütközés riport

A `BarcodeCollisionHelper` a Validator részeként működik, és feladata:

- CSV import során emberbarát ütközés riport készítése,
- a BarcodeRegistry aktuális állapotának vizsgálata,
- különbségtétel:
  - ütközés,
  - hiányzó rekord,
  - többértelmű helyzet között,
- entityId‑alapú kivételek kezelése (ha a ledger‑rekord entityId mezője üres → nem collision),
- **retired rekord esetén mindig ütközést jelent**, még akkor is, ha az entityId eltér,
- a retired státusz **nem felszabadítás**, hanem **végleges lezárás**,
- nem ír a ledgerbe, csak riportot készít.

---

## 4. CSV szerződés (végleges)

A barcode ledger CSV formátuma **4 mezőből áll**:
barCode;entityType;introducedAt;retiredAt

**Naming konzisztencia (CSV → Ledger → Domain):**
- CSV mező: `barCode` → külső adatcsere formális mezőneve
- Ledger mező: `code` → a barcode tényleges értéke a ledger‑modellben
- Domain mező: `barcode` → az entitás aktuális, ember által látható vonalkódja

- `entityId` NEM része a CSV‑nek (csak runtime trace a ledger‑modellben)
- `status` NEM része a CSV‑nek (a retiredAt mezőből következik)
- dátumok ISO 8601 formátumban
továbbá:
- a retiredAt mező végleges lezárást jelent
- a CSV‑ben szereplő retired rekordok örökre lefoglalják a kódot
- a CSV‑ben szereplő retired kódot tilos újra kiosztani

---

## 5. Modul összefoglalás

A Barcode modul:

- globális ledger
- Validator a kapu
- CollisionHelper a riportoló
- CSV szerződés stabil
- domain registryktől független


---

## 2. BarcodeRecord – az életút modell

A `BarcodeRecord` egy auditált, önálló entitás:

- `code` – a tényleges vonalkód
- `entityType` – emberbarát típusnév (pl. "Product", "Material")
- `entityId` – opcionális, csak runtime trace
- `introducedAt` – mikor került bevezetésre
- `retiredAt` – mikor lett nyugdíjazva (ha üres → aktív)

A BarcodeRecord NEM domain entitás, hanem globális erőforrás.

---

## 3. BarcodeRepository – CSV adapter

A repository háromlépcsős CSV import mintát követ:

1. **Convert** – nyers CSV sor → BarcodeRow  
2. **Validate** – BarcodeRow → audit hibák  
3. **Build** – BarcodeRow → BarcodeRecord (ledger formátum)

A CSV szerződés végleges formája (külső adatcsere):
**barCode;entityType;introducedAt;retiredAt**


A repository NEM tölti fel automatikusan a BarcodeRegistry-t,  
csak visszaadja a ledger-rekordokat.

---

## 4. BarcodeValidator – központi kapu

A validator biztosítja, hogy minden új barcode:

- ne legyen üres,
- a CollisionHelper alapján ne ütközzön (entityId‑alapú kivétel támogatott),
- sikeresen regisztrálódjon a BarcodeRegistry-ben.

A `checkAndRegister()` a helyes út:

1. üres kód → audit hiba  
2. CollisionHelper → emberbarát ütközés riport  
3. `registerNew()` → ledger insert vagy update  

---

## 6. BarcodeRegistry – globális életút-könyvelő

A BarcodeRegistry NEM domain registry, hanem:

- globális ledger (append + retire),
- auditált életút-nyilvántartás,
- nincs remove művelet,
- nincs klasszikus CRUD workflow,
- minden módosítás a Validatoron keresztül történik.

A BarcodeRegistry egyedi szerepe miatt külön ledger-modell szerint működik:

- nincs remove művelet,
- nincs klasszikus update (csak retire),
- globális uniqueness enforcement csak code alapján
- entityType NEM old fel ütközést
- retired rekord nem írható felül, nem „éleszthető újra”
- egy code értékhez csak egyetlen életút tartozhat
- a retiredAt kitöltése után a rekord örökre lezárt

- ledger‑szintű invariánsok: code nem üres, introducedAt érvényes, retiredAt időben előre halad,
- minden regisztráció a BarcodeValidatoron keresztül történik,
- a ledger-rekordok append + retire mintát követnek.

A BarcodeRegistry architektúrája teljesen elkülönül a domain registryktől.

Ez a struktúra:

- kifejezi a BarcodeRegistry speciális szerepét,
- leválasztja a domain registry-kről,
- biztosítja a globális életút-könyvelést.

---

## Végső összegzés

A Barcode modul célja a globális vonalkód-életút könyvelése.  
A BarcodeRegistry nem domain registry, hanem egy különálló ledger.

A modul:

- önálló életút-rekordokat kezel,
- auditált, globális egyediséget biztosít,
- dedikált BarcodeLedgerMixin-nel,
- CSV import/export támogatással.
- A barcode identitás, nem újrahasznosítható erőforrás.
- A retired státusz végleges, a kód örökre lefoglalt.