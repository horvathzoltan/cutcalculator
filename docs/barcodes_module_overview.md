## *A Barcode modul architekturális szerepe és működése*

# Barcode modul – architekturális áttekintés

A Barcode modul a rendszer globális vonalkód-életút könyvelője.  
Feladata: minden olyan vonalkód, amely bármely domain entitáshoz tartozik (Product, Material, stb.),  
önálló, auditált életút-rekordokat (`BarcodeRecord`) kapjon.

A modul NEM domain registry, hanem egy különálló, globális ledger.

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
3. **Build** – BarcodeRow → BarcodeRecord

A repository NEM tölti fel automatikusan a BarcodeRegistry-t,  
csak visszaadja a rekordokat.

---

## 4. BarcodeValidator – központi kapu

A validator biztosítja, hogy minden új barcode:

- ne legyen üres,
- legyen globálisan egyedi,
- sikeresen regisztrálódjon a BarcodeRegistry-ben.

A `checkAndRegister()` a helyes út:

1. üres kód → audit hiba  
2. `isBarcodeUnique()`  
3. `registerNew()` → audit log

---

## 5. BarcodeCollisionHelper – emberi ütközés riport

Feladata:

- CSV import során emberbarát ütközés riport készítése,
- a BarcodeRegistry állapotát használja,
- különbséget tesz:
  - ütközés,
  - hiányzó rekord,
  - többértelmű helyzet között.

---

## 6. BarcodeRegistry – globális életút-könyvelő

A BarcodeRegistry NEM domain registry, hanem:

- globális ledger,
- auditált életút-nyilvántartás,
- egyedi workflow.

A jelenlegi felépítés:

- `RegistryEngineBase<BarcodeRecord>`
- `CrudMixin<BarcodeRegistry, BarcodeRecord>`
- `RegisterMe<BarcodeRegistry>`

A BarcodeRegistry egyedi szerepe miatt külön mixinre van szükség:

### `BarcodeLedgerMixin<Host, BarcodeRecord>`

Ez biztosítja:

- életút-invariánsok érvényesítését,
- introducedAt / retiredAt kezelését,
- globális uniqueness enforcement-et,
- audit logokat.

A BarcodeRegistry így épülne fel:

- `RegistryEngineBase<BarcodeRecord>`
- `CollectorMixin<BarcodeRegistry, BarcodeRecord>` (update-t támogató verzió)
- `BarcodeLedgerMixin<BarcodeRegistry>`
- `RegisterMe<BarcodeRegistry>`

Ez a struktúra:

- kifejezi a BarcodeRegistry speciális szerepét,
- leválasztja a domain registry-kről,
- biztosítja a globális életút-könyvelést.

---

## Összegzés

A Barcode modul célja a globális vonalkód-életút könyvelése.  
A BarcodeRegistry nem domain registry, hanem egy különálló ledger.

A modul:

- önálló életút-rekordokat kezel,
- auditált, globális egyediséget biztosít,
- dedikált BarcodeLedgerMixin-nel,
- CSV import/export támogatással.

