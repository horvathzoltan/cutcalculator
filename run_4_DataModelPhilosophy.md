# 📘 **CutCalculator Data Model Philosophy**  
*A CSV‑alapú, emberi adatmodell és runtime entitásrendszer alapelvei*

---

# 🎯 **1. A rendszer természete: emberi adatmodell, nem adatbázis**

A CutCalculator **nem relációs adatbázisra épül**, hanem:

- **ember által szerkeszthető CSV fájlokra**,  
- amelyek a projekt *adatbázisát* jelentik,  
- és amelyekből a rendszer **futásidőben** épít fel egy memóriabeli, típusos, UUID‑alapú entitáshálót.

Ez a filozófia meghatározza:

- hogyan tárolunk adatot,  
- hogyan olvasunk be adatot,  
- hogyan kezeljük a relációkat,  
- hogyan garantáljuk a konzisztenciát,  
- és hogyan működik a teljes import/export architektúra.

---

# 🧩 **2. Emberi kulcsok vs. runtime kulcsok**

A CutCalculator kétféle azonosítót használ:

## 🔹 2.1. Emberi kulcsok (CSV-ben)
Ezek:

- stabilak,  
- ember által értelmezhetők,  
- nem változnak futások között,  
- a CSV‑ben szerepelnek,  
- a relációk alapját képezik.

Példák:

- `barcode` (Product, Material)  
- `name` (NeedCalculation modeName)  
- `colorStr` (NamedColor)  
- `machineId`  
- `typeStr`  

Ezek a **primer kulcsok** a CSV világában.

## 🔹 2.2. Runtime UUID-k (memóriában)
Minden domain entitás:

- futásidőben kap UUID‑t,  
- ez NEM szerepel a CSV‑ben,  
- nem stabil,  
- nem alkalmas CSV‑szintű relációkra,  
- csak a memóriabeli registry‑kben használjuk.

Ez a kettősség a rendszer egyik legfontosabb alapelve.

---

# 🧭 **3. A relációk feloldása: emberi kulcs → UUID**

A CSV‑ben lévő relációk **emberi kulcsokon** alapulnak.

Példa NeedCalculation esetén:

```
productBarcode;modeName
ROL-RR;mókus
```

A Build Phase feladata:

1. megkeresni a ProductRegistry-ben a barcode alapján:  
   `findByBarcode("ROL-RR")`
2. ha létezik → UUID hozzárendelése  
3. ha nem → hiba

Ez a mechanizmus:

- kikényszeríti a konzisztenciát,  
- kizárja az árva rekordokat,  
- nem igényel globális validációt,  
- nem igényel adatbázis‑szerű constraints rendszert.

---

# 🔁 **4. A Three‑Phase Import 2.0 szerepe a modellben**

A Data Model Philosophy és a Three‑Phase Import 2.0 együtt alkotják a teljes import‑architektúrát.

A három fázis:

### 1) Convert  
CSV sor → Row struct  
(emberi kulcsok még stringként)

### 2) Build  
Row → Domain object  
(emberi kulcs → UUID feloldás)

### 3) Assemble  
Domain object → Registry  
(runtime entitásháló összeállítása)

A relációs integritás **a Build Phase-ben** történik.

---

# 🧱 **5. Registry-k mint runtime adatbázis**

A registry-k:

- memóriabeli tárolók,  
- UUID‑alapú entitásokat tartalmaznak,  
- gyors keresést biztosítanak,  
- nem perzisztálnak,  
- nem töltenek be maguktól,  
- a StartupManager tölti fel őket.

A registry-k **nem adatbázisok**, hanem:

- runtime indexek,  
- gyors lookup struktúrák,  
- auditálható entitáslisták.

---

# 🏗️ **6. A StartupManager szerepe: a teljes adatbázis összeállítása**

A StartupManager:

- beolvassa az összes CSV-t,  
- minden repository-t meghív,  
- minden registry-t feltölt,  
- garantálja, hogy a runtime entitásháló konzisztens legyen.

Ez a rendszer „adatbázis inicializálása”.

---

# 🧩 **7. Nincsenek gyűrűk, nincsenek ciklusok**

A CutCalculator adatmodellje **irányított, aciklikus gráf**:

- Product → NeedCalculation  
- Product → Material  
- Material → NamedColor  
- NeedCalculation → NeedCalculationDetail  
- stb.

Ezért:

- nincs szükség topológiai rendezésre,  
- nincs szükség többkörös validációra,  
- nincs szükség cross‑table global validationra.

Minden reláció **lokálisan feloldható**.

---

# 🛡️ **8. Konzisztencia garanciák**

A rendszer konzisztenciáját az alábbi mechanizmusok biztosítják:

### ✔ Convert Phase – formátumellenőrzés  
### ✔ Build Phase – relációs feloldás  
### ✔ Build Phase – hibák gyűjtése  
### ✔ Assemble Phase – csak érvényes rekordok kerülnek registry-be  
### ✔ StartupManager – minden registry egyszer töltődik fel  
### ✔ FileContextCollector – teljes auditálhatóság

Ez a kombináció garantálja:

- nincs árva rekord,  
- nincs hibás reláció,  
- nincs csendes adatvesztés,  
- nincs „betölt, de nem jelenik meg” jelenség.

---

# 🧠 **9. Export filozófia**

Export esetén:

- a domain objektumokból **emberi kulcsokat** írunk vissza a CSV-be  
- soha nem UUID-t  
- mert a CSV emberi rendszer  
- és a következő futásban úgyis új UUID-k keletkeznek

Ez a körkörös modell:

```
CSV (emberi kulcsok)
    ↓ Convert
Row
    ↓ Build (feloldás)
Domain (UUID)
    ↓ Assemble
Registry
    ↓ Export
CSV (emberi kulcsok)
```

Ez a CutCalculator adatmodellének lényege.

---

# 🟩 **10. Összegzés – a CutCalculator adatmodell filozófiája**

A CutCalculator:

- **emberi kulcsokon alapuló CSV rendszer**,  
- amelyből **runtime UUID‑alapú entitásháló** épül,  
- a relációk **emberi kulcs → UUID** feloldással működnek,  
- a konzisztencia **lokálisan garantált**,  
- a registry-k **runtime adatbázisok**,  
- a StartupManager **állítja össze a teljes rendszert**,  
- a Three‑Phase Import 2.0 **egységesíti az importot**,  
- a FileContextCollector **auditálja a folyamatot**,  
- és az egész rendszer **aciklikus, determinisztikus, emberbarát**.

Ez a dokumentum a CutCalculator adatmodelljének **alapfilozófiája**.

---

Ha szeretnéd, készítek:

- egy **vizuális diagramot** az adatmodellről,  
- egy **repository‑szintű checklistet**,  
- vagy egy **konkrét patch‑csomagot** a NeedCalculation import 3‑fázisú átírásához.

Csak szólj, merre menjünk tovább.
