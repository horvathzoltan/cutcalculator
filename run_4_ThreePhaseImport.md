# 📘 **Three‑Phase Import 2.0 – CutCalculator fejlesztői dokumentáció**

## 🎯 **Cél**
A CSV‑alapú adatbetöltés egységes, auditálható, moduláris és hibabiztos architektúrájának meghatározása a CutCalculator rendszerben.

A Three‑Phase Import 2.0 célja:

- egységesíteni minden repository működését,  
- biztosítani a relációs konzisztenciát emberi kulcsok alapján,  
- garantálni, hogy minden CSV import auditálható legyen,  
- kiküszöbölni az árva rekordokat és a csendes hibákat,  
- és a MaterialRepository mintájára egységes, tiszta struktúrát adni.

---

# 🧩 **Alapelvek – CSV‑alapú emberi adatmodell**

A CutCalculator **nem adatbázis**, hanem **ember által szerkeszthető CSV‑készlet**.  
Ez meghatározza az adatmodell filozófiáját:

### 🔹 1. A CSV‑ben **emberi kulcsok** szerepelnek  
Tipikusan:

- barcode  
- name  
- rövid kódok  
- logikai jelölések  

Ezek stabilak, érthetők, és nem változnak futások között.

### 🔹 2. A rendszerben **runtime UUID** az entitások azonosítója  
Minden domain objektum:

- futásidőben kap UUID‑t,  
- ez nem szerepel a CSV‑ben,  
- nem stabil,  
- nem alkalmas CSV‑szintű relációkra.

### 🔹 3. A relációk mindig **emberi kulcs → UUID** feloldással működnek  
Ez a Build Phase része.

Példa NeedCalculation esetén:

```
CSV: productBarcode;modeName
      ROL-RR;         mókus
```

A Build Phase:

```cpp
auto* p = ProductRegistry::instance().findByBarcode(row.productBarcode);
if (!p) ctx.addError(...);
c.productId = p->id;
```

Ez garantálja a konzisztenciát.

### 🔹 4. A rendszerben **nincs gyűrű**, nincs ciklikus függés  
Ezért nincs szükség globális topológiai rendezésre.

### 🔹 5. A relációs integritás **lokálisan validálható**  
Minden rekord a Build Phase-ben ellenőrizhető.

---

# 🔁 **Három fázis – Three‑Phase Import 2.0**

## 1) **Convert Phase**  
*Nyers CSV sor → típusos Row struktúra*

Feladatok:

- mezők számának ellenőrzése  
- üres mezők vizsgálata  
- alapvető formátumellenőrzés  
- soronkénti auditálás (FileContext)

Példa:

```cpp
struct NeedCalculationRow {
    QString productBarcode;
    QString modeName;
};
```

A Convert függvény:

```cpp
auto convertRowToNeedCalculationRow(const QVector<QString>& parts, FileContext& ctx)
```

---

## 2) **Build Phase**  
*Row → domain objektum*

Feladatok:

- mélyebb validáció (számok, enumok, formátumok)  
- **relációk feloldása emberi kulcs alapján**  
- domain objektum összeállítása  
- hibák gyűjtése FileContext-ben

Ez a legfontosabb újítás:

### 🔥 **A relációs validáció a Build Phase része.**

Példa:

```cpp
auto* p = ProductRegistry::instance().findByBarcode(row.productBarcode);
if (!p) {
    ctx.addError(...);
    return std::nullopt;
}

NeedCalculation c;
c.id = QUuid::createUuid();
c.productId = p->id;
c.name = row.modeName;
```

---

## 3) **Assemble Phase**  
*Domain objektumok → registry*

Feladatok:

- domain objektumok összegyűjtése  
- registry.setAll() hívása  
- kapcsolatok véglegesítése  
- audit logolás

Példa:

```cpp
NeedCalculationRegistry::instance().setAll(calcs);
```

---

# 🧭 **StartupManager szerepe**

A registry-k **nem töltik be magukat**.  
A repository-k **nem töltik be a registry-t**.

Ezért a StartupManager feladata:

### 🔹 1. Minden CSV beolvasása  
### 🔹 2. Minden registry feltöltése  
### 🔹 3. A teljes rendszer konzisztenciájának biztosítása

Példa:

```cpp
QVector<NeedCalculation> calcs;
NeedCalculationRepository::load(calcs);
NeedCalculationRegistry::instance().setAll(calcs);
```

Ez minden repository-ra egységesen alkalmazandó.

---

# 📦 **FileContext és FileContextCollector – audit rendszer**

A Three‑Phase Import 2.0 része a teljes auditálhatóság:

- minden import művelethez FileContext tartozik  
- a destruktor automatikusan regisztrál  
- a CSV összefoglaló a FileContextCollector alapján készül  
- minden sor auditálható  
- minden hiba visszakövethető

A Three‑Phase Import 2.0 előírja:

### 🔹 Minden repository pontosan 1 FileContext-et használ  
### 🔹 A Convert/Build/Assemble fázisok ugyanazt a contextet frissítik  
### 🔹 A read() helyett readAndConvert() használata kötelező  
### 🔹 A duplikált FileContext-ek tiltottak

---

# 🧱 **Egységes repository minta (MaterialRepository alapján)**

Minden repository így épül fel:

1. `Row` struct  
2. `convertRowToXxxRow()`  
3. `validateXxxRow()` (opcionális)  
4. `buildXxxFromRow()`  
5. `loadXxxRows()` → `readAndConvert()`  
6. `load()` → buildAll + registry.setAll

Ez a minta kötelező:

```cpp
CsvImporter::FileContext ctx("NeedCalculation import", path);

auto rows = CsvImporter::readAndConvert<Row>(ctx, convertRowToRow);
auto domainObjects = CsvImporter::buildAll<Row, Domain>(rows, buildDomainFromRow, ctx);

out = domainObjects;
```

---

# 🧩 **Relációs integritás – Three‑Phase Import 2.0 szabályai**

### ✔ A CSV-ben emberi kulcs szerepel  
(pl. barcode)

### ✔ A Build Phase feloldja a relációt  
(barcode → UUID)

### ✔ Ha a reláció nem létezik → hiba  
A rekord nem kerül be a registry-be.

### ✔ Nincs szükség globális cross‑table validationra  
Mert:

- nincs gyűrű  
- minden reláció lokális  
- minden reláció egyirányú  
- minden reláció egy mező alapján feloldható

---

# 🟩 **Összegzés – Three‑Phase Import 2.0 lényege**

- Egységes, auditálható import architektúra  
- CSV‑alapú emberi kulcsok használata  
- Runtime UUID-k automatikus generálása  
- Relációk feloldása a Build Phase-ben  
- Registry-k feltöltése a StartupManager-ben  
- FileContext-alapú audit minden fázisban  
- MaterialRepository mintájára egységesített repository-k  
- Árva rekordok kizárása  
- Nincs szükség globális konzisztencia vizsgálatra  

---

Ha szeretnéd, készítek:

- **diff‑szerű patch‑csomagot** a NeedCalculationRepository 3‑fázisú átírásához,  
- vagy a teljes **StartupManager registry‑betöltő blokkot**,  
- vagy egy **külön dokumentumot a CSV‑alapú adatmodell filozófiájáról**.

Csak mondd, melyik legyen a következő lépés.
