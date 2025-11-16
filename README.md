# CutCalculator

**CutCalculator** is a Qt/C++ application for calculating precise material requirements 
in manufacturing workflows (e.g. roller blinds, shading systems).  
It complements **CutPlanner** by focusing on deduction rules, measurement modes, 
and real-time material need calculation.

---

## ✨ Features

- **Global MaterialMaster**  
  Central database of all raw materials (tubes, rods, fabrics, accessories).

- **ProductDefinition**  
  Hierarchical product tree (Roletta, Sávroló, etc.) with per-type deduction rules.

- **Deduction Rules**  
  Measurement-mode based formulas (e.g. full width, fabric width) to compute cutting sizes.

- **Order Management**  
  - Customer autocomplete  
  - Auto-incrementing order line numbers  
  - Aggregation of identical products (same type, size, color)  

- **CalculationDomain**  
  Real-time preview of material needs during order entry.

- **AuditDomain**  
  Versioned rules and audit trail for changes.

---

## 📂 Project Structure

```code
src/
├── materials/ # MaterialMaster 
├── products/ # ProductDefinition + DeductionRules
├── customers/ # CustomerMaster 
├── orders/ # OrderDomain 
├── calculation/ # NeedCalculator + NeedResult 
├── audit/ # AuditDomain 
├── common/ # Shared enums, utils 
└── main.cpp # Entry point
```

---

## ⚙️ Build Setup

We use **out-of-source builds** for clarity:

- Source repos:  
/home/zoli/source/repos/CutCalculator

- Build output:  
/home/zoli/build/CutCalculator/Debug /home/zoli/build/CutCalculator/Release

---

### Qt Creator configuration
Set **Projects → Build & Run → Default build directory** to:

/home/zoli/build/%{ProjectName}/%{BuildConfig}


This ensures clean separation between source and build artifacts.

---

## 🚀 Getting Started

### With CMake
```bash
mkdir -p ~/build/CutCalculator
cd ~/build/CutCalculator
cmake ~/source/repos/CutCalculator
make -j8
With qmake
bash
mkdir -p ~/build/CutCalculator
cd ~/build/CutCalculator
qmake ~/source/repos/CutCalculator/CutCalculator.pro
make -j8
```

📜 License
MIT License (or specify your preferred license).
