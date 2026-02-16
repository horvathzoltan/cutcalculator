# 📘 **FormulaEngine DSL – v3 Dokumentáció**

## 1. Áttekintés

A FormulaEngine egy egyszerű, determinisztikus DSL, amely a NeedCalculationDetail `formula` mezőjét értékeli ki.  
A DSL háromféle eredményt adhat:

| Típus | Mező | Jelentés |
|------|------|----------|
| numerikus hossz | `length_mm` | vágási hossz (mm) |
| darabszám | `pieces` | darabszám |
| string | `stringValue` | anyag barcode (choose:) |

A visszatérési típus:

```cpp
struct EvaluatedFormula {
    int length_mm = 0;
    int pieces = 0;
    QString stringValue = "";
};
```

---

## 2. DSL elemek

### 2.1. Alap numerikus operátorok (v1)

| Szintaxis | Jelentés |
|-----------|----------|
| `w-15` | szélesség mínusz 15 mm |
| `h-10` | magasság mínusz 10 mm |
| `fixed:2` | 2 darab |
| `w*h` | terület |

---

### 2.2. Új numerikus operátorok (v2)

| Szintaxis | Jelentés |
|-----------|----------|
| `len:w-10` | hossz = w-10 |
| `len:h-20` | hossz = h-20 |
| `qty:fixed:3` | darabszám = 3 * qty |
| `qty:perOrder:2` | darabszám = 2 |
| `qty:perArea:1000` | darabszám = (w*h)/1000 |

---

### 2.3. Új: `choose:` operátor (v3)

A choose: operátor feltételes anyagválasztást tesz lehetővé.

#### Szintaxis:

```
choose: <feltétel> ? <barcode_true> : <barcode_false>
```

#### Példa:

```
choose: (w*h > 5000000) ? MOTOR_A : MOTOR_B
```

#### Eredmény:

- `stringValue = "MOTOR_A"` vagy `"MOTOR_B"`
- `length_mm = 0`
- `pieces = 0`

A NeedCalculator a `stringValue` alapján választ anyagot.

---

### 2.4. Új: `opt:` operátor (v3)

Az opt: operátor opcionális hossz‑kiegészítést ad.

#### Szintaxis:

```
<base_formula> + opt:<flag>:+<value>
```

#### Példa:

```
h-15 + opt:paint:+40
```

#### Jelentés:

- ha a `paint` flag aktív → +40 mm  
- ha nem → +0 mm  

A flag‑logika később kerül bevezetésre.

---

## 3. Validáció

A validáció helye:

```
NeedCalculationDetailRegistry::isFormulaValid()
```

### 3.1. choose: validáció

Érvényes, ha:

- tartalmaz `?` és `:` jelet
- a három rész nem üres

### 3.2. opt: validáció

Érvényes, ha minden opt token:

```
opt:<flag>:+<integer>
```

például:

```
opt:paint:+40
opt:foo:-5
```

---

## 4. NeedCalculator integráció

A NeedCalculator a FormulaEngine eredményét így kezeli:

### 4.1. choose: esetén

```cpp
if (!ev.stringValue.isEmpty()) {
    // stringValue = anyag barcode
    const auto* mat = MaterialRegistry::instance().findByBarcode(ev.stringValue);
    out.append({ mat->id, 0, qty });
}
```

### 4.2. numerikus esetben

```cpp
if (ev.length_mm > 0 && ev.pieces > 0) {
    out.append({ d.materialId, ev.length_mm, ev.pieces });
}
```

---

## 5. Példák

### 5.1. Anyagválasztás terület alapján

```
choose: (w*h > 3000000) ? MOTOR_HEAVY : MOTOR_LIGHT
```

### 5.2. Opcionális festési ráhagyás

```
h-20 + opt:paint:+40
```

### 5.3. Kombinált példa

```
choose: (w > 1500) ? AXLE_BIG : AXLE_SMALL
    + opt:paint:+30
```

---

## 6. Tesztlefedettség

A következő tesztek ellenőrzik a DSL működését:

- `testChooseSimple()`
- `testChooseFalseBranch()`
- `testOptSimple()`
- `testNeedCalculatorChooseTrue()`
- `testNeedCalculatorChooseFalse()`

---

