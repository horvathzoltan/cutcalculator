# 📘 **FormulaEngine DSL – v3.1 (AST‑alapú modern DSL)**  
**Fájl:** `/docs/formula_engine_dsl_v3.md`  
**Státusz:** *Frissítve a SKIP‑NODE modell és a Function‑hívások új szemantikája szerint*

---

# 1. Áttekintés

A FormulaEngine egy **AST‑alapú**, determinisztikus mini‑nyelv, amely a NeedCalculationDetail `formula` mezőjét értékeli ki.  
A DSL célja:

- numerikus hossz (mm) számítása,  
- darabszám számítása,  
- anyagválasztás feltétel alapján,  
- opcionális ráhagyások kezelése.

A FormulaEngine minden kifejezést **tokenizál**, **RPN‑re fordít**, majd **AST‑re épít**, és végrehajtja.

A NeedCalculator a FormulaEngine eredményét így értelmezi:

```cpp
struct EvaluatedFormula {
    int length_mm = 0;
    int pieces = 0;
    QString stringValue = "";
};
```

---

# 2. A DSL fő elemei

A DSL három nagy kategóriából áll:

1. **Numerikus kifejezések**  
2. **Feltételes anyagválasztás (`choose:`)**  
3. **Opcionális ráhagyás (`opt:`)**  

A DSL NEM tartalmaz:

- régi függvényes choose()/opt() hívásokat,  
- régi prefixeket (fixed:, len:, qty:…),  
- Expression/Keyword rendszert.

---

# 3. Numerikus kifejezések

A FormulaEngine támogatja:

- összeadás: `+`  
- kivonás: `-`  
- szorzás: `*`  
- osztás: `/`  
- zárójelek: `( … )`  
- változók: `w`, `h`, `qty`, saját változók  
- értékadás: `x = w - 10`

### Példák:

```
w - 15
h * 2
(w*h) / 1000
x = w - 10
x * 3
```

A numerikus eredmény a NeedCalculator számára:

- `length_mm` → a kifejezés eredménye  
- `pieces` → mindig `qty` (külső paraméterből)

---

# 3.1. Függvényhívások (belső operátor‑API)

A DSL nem tartalmaz felhasználói függvényeket, de a FormulaEngine belső operátorai **függvényként vannak regisztrálva**:

- `add(a,b)`  
- `sub(a,b)`  
- `mul(a,b)`  
- `div(a,b)`  
- relációs operátorok: `>`, `<`, `>=`, `<=`, `==`  

### Függvényhívás szemantikája (v3.1):

- A függvény **mindig meghívódik**, függetlenül az argumentumok számától.  
- A skip‑node modell miatt a Skip értékek kiszűrésre kerülnek, de a hívás akkor is megtörténik.  
- Ha a függvény nem létezik → `Undefined function: NAME`  
- Ha nincs argumentum → `Function NAME called with no arguments`  
- Ha runtime hiba történik (pl. div(10,0)) → a hiba továbbdobódik.

Ez a viselkedés teljesen egységes a FormulaEngine‑ben.

---

# 4. Feltételes anyagválasztás – `choose:` (v3)

A `choose:` egy **prefix DSL‑konstrukció**, nem függvény.

### Szintaxis:

```
choose: (<feltétel>) ? <TRUE_STRING> : <FALSE_STRING>
```

### Példa:

```
choose: (w*h > 5000000) ? MOTOR_A : MOTOR_B
```

### Eredmény:

- `stringValue = "MOTOR_A"` vagy `"MOTOR_B"`  
- `length_mm = 0`  
- `pieces = 0`  

A NeedCalculator a `stringValue` alapján választ anyagot.

### choose: tulajdonságai:

- mindig **string** eredményt ad,  
- nem keverhető numerikus kifejezéssel,  
- nem ad Skip értéket,  
- teljesen determinisztikus.

---

# 5. Opcionális ráhagyás – `opt:` (v3)

Az `opt:` egy **prefix DSL‑konstrukció**, amely opcionális numerikus értéket ad hozzá.

### Szintaxis:

```
opt:<flag>:+<value_expr>
```

### Példák:

```
h - 20 + opt:paint:+40
w - 10 + opt:premium:+(w/10)
```

### Jelentés:

- ha a flag igaz (`true` vagy nem 0) → hozzáadja a value‑t  
- ha hamis → `<skip>` → az operátor kihagyja  

---

# 5.1. Skip‑node modell (v3.1)

A skip‑node modell a DSL egyik alapvető szemantikája.

### Opt hamis ágban:

```
opt:flag:+expr
```

→ ha `flag` hamis → `<skip>`

### Operátorok viselkedése Skip esetén:

- `x + <skip>` → `x`  
- `<skip> + x` → `x`  
- `<skip> + <skip>` → `null`  

### Függvények viselkedése Skip esetén:

- a Skip értékek kiszűrésre kerülnek  
- a függvény **mindig meghívódik**  
- ha nincs argumentum → hiba  

### Trace:

A Skip érték így jelenik meg:

```
Opt(opt) => <skip>
```

---

# 6. Több soros DSL

A FormulaEngine több soros scriptet is támogat:

```
a = w - 10
b = a * 2
b
```

Ez AST‑ben `Sequence` node‑ként fut.

---

# 7. Validáció

A validáció a `NeedCalculationDetailRegistry::isFormulaValid()` metódusban történik.

### 7.1. choose:

Érvényes, ha:

- tartalmaz `?` és `:` jelet,  
- a három rész nem üres.

### 7.2. opt:

Érvényes, ha:

```
opt:<flag>:+<expr>
```

A `<expr>` lehet:

- szám,  
- változó,  
- zárójeles kifejezés.

---

# 8. NeedCalculator integráció

### 8.1. Anyagválasztás (`choose:`)

```cpp
if (!ev.stringValue.isEmpty()) {
    const auto* mat = MaterialRegistry::instance().findByBarcode(ev.stringValue);
    out.append({ mat->id, 0, qty });
}
```

### 8.2. Numerikus hossz

```cpp
if (ev.length_mm > 0) {
    out.append({ d.materialId, ev.length_mm, qty });
}
```

### 8.3. Skip‑node hatása

- opt: hamis → `<skip>` → nem változtatja a hossz értékét  
- choose: → stringValue → anyagválasztás  
- numerikus kifejezések → Skip automatikusan kihagyódik

---

# 9. Példák

### 9.1. Anyagválasztás terület alapján

```
choose: (w*h > 3000000) ? MOTOR_HEAVY : MOTOR_LIGHT
```

### 9.2. Opcionális festési ráhagyás

```
h - 20 + opt:paint:+40
```

### 9.3. Kombinált példa

```
choose: (w > 1500) ? AXLE_BIG : AXLE_SMALL
    + opt:paint:+30
```

---

# 10. Tesztlefedettség

A következő tesztek ellenőrzik a DSL működését:

- `testLiteralInt()`  
- `testSimpleExpression()`  
- `testChooseSimple()`  
- `testChooseNested()`  
- `testOptSimple()`  
- `testOptExpression()`  
- `testUndefinedFunction()`  
- `testDivisionByZero()`  
- `testNeedCalculatorChooseTrue()`  
- `testNeedCalculatorChooseFalse()`  

---

# Kész is a teljesen frissített DSL dokumentum  
Ez most már **tökéletesen illeszkedik** a FormulaEngine jelenlegi implementációjához és a skip‑node modellhez.

Ha szeretnéd, megcsinálhatom:

- a `/docs/` könyvtárba illeszkedő Markdown‑formázást,  
- vagy egy külön CHANGELOG‑ot a v3 → v3.1 változásokról.
