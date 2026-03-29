# 📘 **FormulaEngine Gyorstalpaló – Gyártási DSL 2026**

## 🎯 Mi ez a DSL?

A FormulaEngine egy **mini nyelv**, amelyet a BOM Workbench használ arra, hogy:

- vágási hosszakat számoljon (Cutting),
- darabszámokat számoljon (Kitting),
- és opcionálisan anyagokat válasszon.

A DSL **kifejezéseket** értékel, és **változókat állít be**, amelyeket a NeedCalculator olvas ki.

---

# 🧩 **1. Input változók (amit a rendszer ad)**

A NeedCalculator minden formula előtt feltölti:

| Változó | Jelentés | Típus |
|--------|----------|-------|
| `w` | szélesség (mm) | Number |
| `h` | magasság (mm) | Number |
| `handler` | „L” / „R” | String |
| `owner` | megrendelő neve | String |
| `color` | szín | String |
| `externalId` | rendelési azonosító | String |
| `product` | productId | String |

Ezeket a formula **olvashatja**, de nem írja.

---

# 🧩 **2. Output változók (amit a formula ad vissza)**

A rendszer **kétféle** formula‑sort ismer:

## **Cutting sor → vágandó elem**
A formula **kötelezően** beállítja:

```
len = <szám>        # vágási hossz (mm)
```

Opcionálisan:

```
mat = "<barcode>"   # anyag felülírása
```

## **Kitting sor → darabos elem**
A formula **kötelezően** beállítja:

```
qty = <szám>        # darabszám
```

Opcionálisan:

```
mat = "<barcode>"
```

---

# 🧩 **3. A DSL szintaxisa**

A FormulaEngine támogatja:

### ✔ Literálok
```
10
3.14
"TE-R-23"
```

### ✔ Változók
```
w
h
qty
len
mat
```

### ✔ Aritmetika
```
w - 30
(h / 2) + 15
```

### ✔ Feltételes (ternary)
```
w > 1000 ? w - 40 : w - 20
```

### ✔ Több soros script
```
a = w - 30
len = a
```

### ✔ Opcionális (opt)
```
len = w - 20 + opt:premium ? 40
```

---

# 🧩 **4. Cutting példák (valós gyártási logika)**

### Tengely (−3 cm)
```
len = w - 30
```

### Vászon (−4 cm)
```
len = w - 40
```

### Alsópálca (−2.7 cm)
```
len = w - 27
```

### Anyag felülírása
```
len = w - 30
mat = "TE-R-23"
```

---

# 🧩 **5. Kitting példák**

### 2 db dugó
```
qty = 2
```

### 1 db rugó
```
qty = 1
```

### Feltételes darabszám
```
qty = w > 1500 ? 2 : 1
```

---

# 🧩 **6. Teljes példa – Roletta Rugós**

### Tengely
```
len = w - 30
mat = "TE-R-23"
```

### Vászon
```
len = w - 40
```

### Alsópálca
```
len = w - 27
mat = "ROL-P"
```

### Alsópálca dugó
```
qty = 2
```

### Tengelyvég
```
qty = 1
```

### Rugós rugó
```
qty = 1
```

---

# 🧩 **7. Hibák, amiket a DSL dobhat**

- `Undefined variable: X`
- `Undefined function: NAME`
- `Division by zero`
- `Invalid choose expression`
- `len not set` (Cutting)
- `qty not set` (Kitting)

---

# 🧩 **8. Best practice-ek**

### ✔ Mindig mm-ben számolj  
A `w` és `h` mm-ben érkezik.

### ✔ Minden cutting sorban legyen `len = ...`  
Ha nincs → hiba.

### ✔ Minden kitting sorban legyen `qty = ...`  
Ha nincs → hiba.

### ✔ A `mat = "..."` csak override  
Ha nincs, a NeedRule‑ben megadott anyag érvényesül.

### ✔ Több soros formula csak akkor kell, ha tényleg számolsz
```
a = w - 30
len = a * 1.05
```

---
