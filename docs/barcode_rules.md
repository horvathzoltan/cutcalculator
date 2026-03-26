# 📘 `docs/barcode_rules.md`  
## *Barcode Format Specification – Karakterkészlet és Formátumszabályok*

# Barcode Format Specification  
**Globális formátumszabályok a rendszerben használt vonalkódokhoz**  
Verzió: 1.0

---

## 1. Cél és hatókör

Ez a dokumentum a rendszerben használt **barcode** értékek formátumát, karakterkészletét és validációs szabályait rögzíti.  
A szabályok minden olyan komponensre kötelezőek, amely:

- barcode‑ot generál,  
- barcode‑ot validál,  
- barcode‑ot importál CSV‑ből,  
- barcode‑ot regisztrál a BarcodeRegistry‑ben.

A specifikáció **nem** a ledger‑modellről szól — azt a `barcodes_module_overview.md` tartalmazza.  
Ez a dokumentum kizárólag a **barcode formátumára** és **karakterkészletére** vonatkozó követelményeket írja le.

---

## 2. Alapelvek

A barcode a rendszerben **globális, stabil, vizuálisan egységes azonosító**.  
Ezért:

- **nem tartalmazhat vizuális törést**,  
- **nem tartalmazhat whitespace‑et**,  
- **nem tartalmazhat ékezetet**,  
- **nem tartalmazhat speciális karaktereket**,  
- **nem tartalmazhat elválasztó jeleket**, amelyek több kódnak látszanak.

A barcode célja, hogy:

- **egy egységként jelenjen meg**,  
- **szemmel és géppel is egyetlen kódnak tűnjön**,  
- **szkennerrel megbízhatóan olvasható legyen**,  
- **CSV‑ben, logokban, fájlnevekben és API‑kban is stabil maradjon**.

---

## 3. Megengedett karakterkészlet

A barcode kizárólag az alábbi karaktereket tartalmazhatja:

```
A–Z
0–9
-
```

### 3.1. Tiltott karakterek

A következő karakterek **szigorúan tiltottak**:

- whitespace: ` `, `\t`, `\n`, `\r`, NBSP  
- ékezetes betűk: `ÁÉÍÓÖŐÚÜŰ` stb.  
- speciális karakterek: `# @ ! ? % * + = ~ ^`  
- elválasztók: `/ \ | : ; , .`  
- idézőjelek: `' "`  
- bármilyen Unicode kombinációs jel

Indoklás:

- whitespace vizuálisan **szétszedi** a kódot,  
- speciális karakterek CSV‑ben és API‑ban **szétesést** okoznak,  
- ékezetek normalize után **nem determinisztikusak**,  
- a barcode‑nak **kompaktnak és egyértelműnek** kell maradnia.

---

## 4. Barcode formátum

A rendszerben minden barcode az alábbi formátumot követi:

```
PREFIX + TOKEN + "-" + SLUG
```

### 4.1. Prefix

- Ember által definiált előtag (pl. `MAT-`, `PRD-`).
- Csak `[A-Z0-9-]` karaktereket tartalmazhat.

### 4.2. Token

- Base36 karakterekből áll (`A–Z`, `0–9`).
- Hossza konfigurálható (alapértelmezés: 6).
- Véletlenszerű, ütközésmentes.

### 4.3. Slug

- A névből képzett kompakt azonosító.
- Normalize → ékezetmentes, nagybetűs.
- Minden szó első két magánhangzója előtt vágunk.
- A szótövek kötőjellel vannak elválasztva.
- Csak `[A-Z0-9-]` karaktereket tartalmazhat.

Példák:

```
"Simple Name" → SIMPL-NAM
"Árvíztűrő tükörfúrógép" → ARV-TUK
"Üveg Űrhajó" → UV-URH
```

---

## 5. Validációs szabályok

A BarcodeValidator minden új barcode‑ra az alábbi szabályokat alkalmazza:

1. **Nem lehet üres.**
2. **Csak megengedett karaktereket tartalmazhat.**
3. **Nem tartalmazhat whitespace‑et.**
4. **Nem ütközhet a ledgerben lévő aktív vagy retired kódokkal.**
5. **Sikeres validáció után a BarcodeRegistry‑be kerül.**

CSV módban a hibák:

- sorhoz kötött audit hibaként (`ctx->addError`) jelennek meg.

UI módban:

- `zEventERROR` audit esemény keletkezik.

---

## 6. Normalize szabályok

A normalize művelet:

1. Unicode Normalization Form D (NFD)
2. Minden diakritikus jel eltávolítása (`\p{Mn}`)
3. Nagybetűsítés (`toUpper()`)

Példa:

```
"ŐűÚő" → "OUUO"
```

---

## 7. Példák

### Érvényes:

```
MAT-ABC123-SIMPL-NAM
PRD-9FZ01A-ARV-TUK
X-000001-A
```

### Érvénytelen:

```
"ABC 123"   (whitespace)
"ABC#123"   (# tiltott)
"ÁBC123"    (ékezet)
"ABC/123"   (slash tiltott)
"ABC\123"   (backslash tiltott)
```

---

## 8. Fenntarthatóság és jövőbeli bővítés

Ez a specifikáció a rendszerben használt vonalkódok **kötelező formátumát és karakterkészletét** rögzíti.  
A dokumentum **élő szabvány**, amely a rendszer fejlődésével együtt frissülhet, különösen az alábbi területeken:

- új prefix‑szabályok vagy domain‑specifikus előtagok bevezetése,  
- tokenhossz vagy token‑generálási stratégia módosítása,  
- slug‑képzési logika finomítása (pl. többnyelvű támogatás),  
- további validációs rétegek vagy audit‑szabályok beépítése,  
- új adatcsere‑formátumok (pl. API payloadok) megjelenése.

A BarcodeValidator és a BarcodeRegistry minden módosítás esetén **ennek a specifikációnak megfelelően** kerül frissítésre.  
A cél változatlan: a barcode a rendszerben továbbra is **globális, stabil, egyértelmű és auditálható azonosító** maradjon.

