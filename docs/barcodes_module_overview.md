## *A Barcode modul architekturális szerepe és működése*

```md
# Barcode modul – architekturális áttekintés

## 1. Bevezetés
A Barcode modul a rendszer globális vonalkód-életút könyvelője. 
Nem domain entitások (Material, Product) attribútumait kezeli, hanem 
önálló, auditált életút-rekordokat (`BarcodeRecord`). 
Ez a modul biztosítja a globális uniqueness-t, az életút követést 
(introducedAt, retiredAt), valamint a CSV-alapú import/export stabil szerződését.

A modul NEM domain registry, hanem egy különálló, globális könyvelő.

---

## 2. BarcodeRecord – az életút modell
A `BarcodeRecord` egy auditált, önálló entitás:

- `code` – a vonalkód (globális kulcs)
- `entityType` – melyik domain entitáshoz tartozik
- `entityId` – opcionális, runtime trace
- `introducedAt` – kiadás időpontja
- `retiredAt` – nyugdíjazás időpontja (opcionális)
- `status()` – Active/Retired

A BarcodeRecord NEM domain entitás, hanem globális erőforrás.

---

## 3. BarcodeRepository – CSV adapter
A repository háromlépcsős importot valósít meg:

1. **Convert** – nyers CSV sor → BarcodeRow
2. **Validate** – auditált mezőellenőrzés
3. **Build** – BarcodeRow → BarcodeRecord
4. **Assemble** – rekordok átadása a registrynek

Export:
- stabil CSV szerződés
- ISO dátumok
- auditbarát header

A repository tiszta adatadapter, nincs benne domain logika.

---

## 4. BarcodeValidator – központi kapu
A validator biztosítja, hogy minden új barcode:

- átmegy a globális uniqueness ellenőrzésen,
- auditált hibát generál, ha ütközik,
- csak valid esetben kerül a registrybe.

A validator vékony, domain-agnosztikus kapu.

---

## 5. BarcodeCollisionHelper – emberi ütközés riport
Feladata:

- ütközés esetén előkeresi a másik entitást (RegistryManager),
- emberbarát üzenetet épít,
- auditált CSV hibát ad vissza.

Ez a modul biztosítja a domain-szintű érthetőséget.

---

## 6. BarcodeRegistry – globális életút-könyvelő
A BarcodeRegistry NEM domain registry, hanem:

- globális könyvelő,
- append-jellegű tároló,
- életút-kezelő (introducedAt, retiredAt),
- globális uniqueness biztosító,
- CSV persist pont.

### Jelenlegi felépítés:
- `RegistryEngineBase<BarcodeRecord>`
- `CrudMixin<BarcodeRegistry, BarcodeRecord>`
- `RegisterMe<BarcodeRegistry>`

### Problémák:
- A CrudMixin nem illeszkedik a szerepéhez (remove nem kell, update speciális).
- Nem használ mixint az életút workflow-ra.
- Nem illeszkedik a Material/Product registryk architektúrájába.
- A hook-lánc nem teljes (nincs mixin-szintű hook).

---

## 7. Javasolt architekturális felépítés
A BarcodeRegistry egyedi szerepe miatt külön mixinre van szükség:

### `BarcodeLedgerMixin<Host, BarcodeRecord>`
Feladatai:
- validateDomain
- validateDuplicate
- beforeInsert
- beforeUpdate
- onInsertLog
- onUpdateLog
- persistRegistry
- retire workflow
- notifyItemsChanged

A BarcodeRegistry így épülne fel:

- `RegistryEngineBase<BarcodeRecord>`
- `CollectorMixin<BarcodeRegistry, BarcodeRecord>` (update-t támogató verzió)
- `BarcodeLedgerMixin<BarcodeRegistry>`
- `RegisterMe<BarcodeRegistry>`

Ez az architektúra:
- kifejezi a BarcodeRegistry speciális szerepét,
- egységesíti a registry-rendszert,
- tiszta, auditálható hook-láncot ad,
- megszünteti a kivétel-jelleget.

---

## 8. Összegzés
A Barcode modul célja a globális vonalkód-életút könyvelése. 
A BarcodeRegistry nem domain registry, hanem egy különálló ledger. 
A modernizálás célja, hogy ezt a szerepet az architektúra is tükrözze:
- CollectorMixin update-támogatással,
- dedikált BarcodeLedgerMixin-nel,
- egységes hook-lánccal,
- tiszta auditálhatósággal.

Ez a dokumentum a modul teljes mentális modelljét adja.
```
