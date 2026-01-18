# 📘 **CutCalculator – README.md (2026‑01‑18, frissített verzió)**

# CutCalculator

A **CutCalculator** egy Qt/C++ alapú, registry‑vezérelt alkalmazás, amely
gyártási folyamatok (pl. roletta, sávroló, árnyékolástechnika) 
anyagigényét számítja ki valós időben.  
A rendszer a **ProductDefinition**, **NeedRule**, **NeedCalculation** és 
**MaterialMaster** adatok alapján határozza meg a vágási méreteket, 
darabszámokat és szerelési elemeket.

A CutCalculator a **CutPlanner** kiegészítője:  
a Planner a gyártási optimalizálást végzi,  
a Calculator pedig a **szabályalapú anyagszükséglet számítást**.

---

# ✨ Fő funkciók

- **Globális MaterialMaster**  
  Minden alapanyag (tok, tengely, pálca, szövet, rugó, motor, szerelék).

- **ProductDefinition + NeedRule**  
  Terméktípusonkénti levonási szabályok és szerelékek.

- **Calculation Modes**  
  Szélesség‑levonás, fix darabszám, szövetfelület, gyártási módok.

- **Order kezelés**  
  - Megrendelő autocomplete  
  - Automatikus tételszám  
  - Termék + méret + szín + darabszám  
  - Aggregálás azonos ügyfél + termék + szín esetén

- **Real‑time anyagszükséglet preview**  
  Azonnal látszik, milyen anyag kell a megadott méretekhez.

- **Audit rendszer**  
  Jelzi a hiányzó szabályt, hibás formulát, hiányzó anyagot.

- **Snapshot‑alapú UI**  
  A Workbench minden splitter/header állapotot megjegyez.

- **Overlay‑alapú státuszjelzés**  
  Minden torony (Tree, Needs, Modes, Details) saját overlay‑ikonon mutatja:
  - ❌ nincs adat  
  - 🟡 részleges adat  
  - 🟢 teljes adat  

---

# 📂 Projektstruktúra

```text
src/
├── materials/          # MaterialMaster + views
├── products/           # ProductDefinition + ProductTree
├── needs/              # NeedRule + MaterialRequirements
├── calcmodes/          # CalculationModes
├── calculation/        # NeedCalculationDetail + NeedCalculator
├── colors/             # NamedColor registry
├── barcodes/           # Barcode registry + collision detection
├── common/             # Registry, CSV, logger, utils, snapshot
├── workbench/          # BOMWorkbench (Tree + Needs + Modes + Details)
├── ui/                 # Widgets (overlay, adapters)
└── main.cpp            # Entry point
```

---

# 🧩 Domain modellek

### **MaterialMaster**
- id, name, type, size, color, weight, RAL/HEX
- keresztmetszet, vágási mód, festési mód

### **ProductDefinition**
- terméktípusok (Roletta, Sávroló, Tetőtéri, Rugós…)
- levonási szabályok
- szerelékek
- verziózás

### **NeedRule**
Kapcsolótábla ProductDefinition ↔ MaterialMaster között.

### **NeedCalculation**
Számítási mód egy NeedRule‑hoz (pl. `w-15`, `fixed:2`).

### **NeedCalculationDetail**
Részletes számítási lépések.

### **OrderHeader / OrderLine**
Megrendelések, méretek, színek, darabszámok.

### **NeedResult**
A számítás eredménye (materialId, length, pieces, mode).

### **AuditEntry**
Hiányzó szabály, hibás formula, hiányzó anyag.

---

# 🧮 Számítási logika

- **Méretre vágott elemek:**  
  `cut = width - deduction`

- **Fix darabos elemek:**  
  `pieces = quantity * fixed_amount`

- **Szövetfelület:**  
  `area = width * height`

- **Aggregálás:**  
  azonos ügyfél + termék + szín → összevonás

- **Audit:**  
  minden hiányzó szabály külön AuditEntry‑t kap

---

# 🖥️ UI architektúra (MVP + Overlay + Snapshot)

A CutCalculator UI‑ja **MVP‑alapú**, domain-ikertornyok, ahol minden torony külön Presenter + Manager + View réteggel.

### **Tornyok:**
- ProductTree  
- MaterialRequirements  
- CalculationModes  
- CalculationModeDetail  

Mindegyik torony:

- saját toolbar  
- saját overlay‑ikon  
- registry‑vezérelt frissítés  
- snapshot‑kompatibilis layout  

### **Overlay rendszer**
Az `OverlayIconWidget` domain‑vezérelt:

- repoCount  
- visibleRows  
- domainCount  
- uiCount  

→ automatikusan választja a ❌ / 🟡 / 🟢 állapotot.

---

# 🧱 Architektúra áttekintés

### **Registry‑vezérelt domain modell**
- RegistryEngineBase  
- RegistryCore  
- RegistryManager  
- Traits + Mixins  
- Lookup + subscription  

### **Háromfázisú import pipeline**
1. Convert  
2. Build  
3. Assemble  

### **Snapshot rendszer**
- GeometryHelper  
- SnapshotManager  
- WorkbenchSnapshot  

### **Logger rendszer**
- Logger  
- LogManager  
- EventLogger  
- ErrorBucketizer  
- LogViewAdapter  

---

# 🧭 Komponens‑atlasz (ikon‑nyelvvel)

A teljes, frissített ikon‑tábla külön fájlba is kitehető:

`docs/architecture_components.md`

(Az előző üzenetben már elkészült a teljes, naprakész verzió.)

---

# ⚙️ Build & Run

### CMake
```bash
mkdir -p ~/build/CutCalculator
cd ~/build/CutCalculator
cmake ~/source/repos/CutCalculator
make -j8
```

### qmake
```bash
mkdir -p ~/build/CutCalculator
cd ~/build/CutCalculator
qmake ~/source/repos/CutCalculator/CutCalculator.pro
make -j8
```

---

# 🧪 Tesztadatok

- `testdata/materials.csv`
- `testdata/products.csv`
- `testdata/needrules.csv`
- `testdata/need_calculations.csv`


---

# 🟢 KÉSZ A FRISSÍTETT README.md

Ez most már:

- a teljes architektúrát lefedi,  
- a domain‑modellt bemutatja,  
- a projekt mélységét tükrözi,  
- a modul‑atlaszt integrálja,  
- és fejlesztőbarát, modern dokumentum.

Ha szeretnéd, készítek:

- **README‑light** verziót (rövidebb, GitHub‑barát),  
- vagy **README‑extended** verziót (diagramokkal, Mermaid‑del).  

Csak mondd ki, melyik irányba menjünk tovább.
