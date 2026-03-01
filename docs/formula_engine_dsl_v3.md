# 📘 FormulaEngine DSL – *aktuális, kódból rekonstruált specifikáció*  
**Fájl javaslat:** `/docs/formula_engine_dsl_current.md`  
**Forrás:** teljes Expression modul (tokenizer, parser, AST, evalNode, function_registry)

---

## 1. A DSL célja és működési modellje

A FormulaEngine egy **általános, AST‑alapú mini‑nyelv**, amely:

- tokenizálja a bemenetet,
- RPN‑re alakítja,
- AST‑et épít,
- végrehajtja a kifejezést,
- a változókat a `VariableRepository`‑ban tárolja.

A NeedCalculator:

- feltölti az input változókat (`w`, `h`, `qty`, stb.),
- futtatja a FormulaEngine‑et,
- kiolvassa az output változókat (`requiredLength`, `qty`, `material`),
- ezekből építi a RawCut / RawKit modelleket.

A FormulaEngine **nem tud semmit** a cut/kit logikáról, csak számol és változókat ír.

---

## 2. A DSL lexikai elemei (Tokenek)

### Literálok
- **Számok**: `10`, `3.14`, `.5`  
  → `TokenType::Number`
- **Stringek**: `"text"`  
  → `TokenType::StringLiteral`
- **Nagybetűvel kezdődő azonosító is string**:  
  `MOTOR_A` → `"MOTOR_A"`

### Változók
- `w`, `h`, `qty`, `flag`, `x1`, `base_margin` stb.  
  → `TokenType::Variable`
- `$name` formátum is változó, de a jelenlegi engine nem használja külön.

### Kulcsszavak
- `choose` vagy `choose:` → `TokenType::Choose`
- `opt` vagy `opt:` → `TokenType::Opt`
- `return` → `TokenType::Return`

### Operátorok
- A tokenizer felismeri:
  - `+`, `-`, `*`, `/`
  - `>`, `<`, `>=`, `<=`, `==`
  - `=`
  - `?`, `:`, `,`, `(`, `)`
- A parser + operator_info alapján:
  - **infix**: `+`, `-`, `*`, `/`, `>`, `<`, `>=`, `<=`, `==`, `=`
  - **prefix**: `+`, `-`, `Function`, `Return`
  - **ternary**: `? :` → `Choose` node

---

## 3. A DSL grammatikája (Parser viselkedése)

### 3.1. Alap kifejezések
- Literálok: szám, string
- Változók
- Zárójelek: `( … )`

### 3.2. Prefix/infix `+` és `-`
- Ha operandust vár → prefix (`+x`, `-x`)
- Ha nem → infix (`a + b`)

### 3.3. Függvényhívás
- `name(expr1, expr2, ...)`
- A parser felismeri, az AST‑ben `Function` node lesz.
- A függvények a `FunctionRegistry`‑ben vannak regisztrálva.

### 3.4. Ternary operátor
```
cond ? expr1 : expr2
```
AST‑ben:
```
Choose(cond, expr1, expr2)
```

### 3.5. opt: prefix DSL
A parser speciális mintát kezel:

```
opt: flag ? expr
```

RPN:
```
flag expr Opt
```

AST:
```
Opt(flag, expr)
```

### 3.6. Assignment
```
x = expr
```

AST:
```
Assignment(name="x", child=expr)
```

### 3.7. Több soros script
Sorvége: `\n` → `StatementEnd`  
AST:
```
Sequence(
    Statement(expr1),
    Statement(expr2),
    ...
)
```

---

## 4. AST node‑típusok

- `Number`
- `StringLiteral`
- `Variable`
- `Operator`
- `Function`
- `Choose`
- `Opt`
- `Assignment`
- `Return`
- `Statement`
- `Sequence`

---

## 5. Runtime szemantika (evalNode)

### 5.1. Változók
- `VariableRepository` tárolja őket.
- Ha egy változó nincs definiálva → **hiba**:
  ```
  Undefined variable: X
  ```

### 5.2. Operátorok
- Gyerekek kiértékelése → `args`
- `Skip` kiszűrése
- Ha 0 arg → `null`
- Ha 1 arg → az az érték
- Ha 2+ arg → `FunctionRegistry::call(op, args)`

### 5.3. Függvények
- Ugyanaz, mint az operátorok
- Ha nincs ilyen függvény → hiba:
  ```
  Undefined function: NAME
  ```

### 5.4. Assignment
```
x = expr
```
- Kiértékeli `expr`‑t
- `vars.set("x", value)`
- Visszatérési érték: `null`

### 5.5. Choose
```
cond ? a : b
```
- `cond.toBool()` alapján választ

### 5.6. Opt
```
opt: flag ? expr
```
- Ha `flag.toBool()` igaz → `expr` értéke
- Ha hamis → `Value::skipValue()`

### 5.7. Skip érték
- Operátorok kiszűrik
- Függvények kiszűrik
- Ha minden operandus skip → `null`

### 5.8. Return
- Több assignmentet is visszaadhat
- A NeedCalculator nem használja

### 5.9. Sequence
- Sorban kiértékel minden statementet
- Utolsó értéket adja vissza

---

## 6. A DSL tényleges képességei

### 6.1. Numerikus kifejezések
```
w - 20
(w*h) / 1000
x = w - 10
x * 3
```

### 6.2. Feltételes logika
```
material = (w > 1000) ? "BIG" : "SMALL"
```

### 6.3. Opcionális ráhagyás
```
requiredLength = w - 20 + opt:paint ? 40
```

### 6.4. Több soros script
```
a = w - 10
b = a * 2
requiredLength = b
qty = qty
material = "M1"
```

---

## 7. Hibák, amiket a DSL dobhat

- `Undefined variable: X`
- `Undefined function: NAME`
- `Function NAME called with no arguments`
- `Division by zero`
- `AST build failed`
- `AST build failed in multi-line script`

Minden hiba `EvalResult.error` mezőbe kerül.

---

## 8. A DSL NEM tartalmazza

- `len:`, `qty:`, `fixed:`, `choose:` prefix DSL‑t  
- `qty_fixed`, `qty_perOrder`, `qty_perArea` függvényeket  
- bármilyen domain‑specifikus logikát  
- implicit qty‑szorzást  
- implicit material‑választást  

A DSL **általános nyelv**, nem domain‑DSL.

---

## 9. Példák (valós, működő DSL)

### 9.1. Cutting formula
```
requiredLength = w - 20
qty = qty
material = "M1"
```

### 9.2. Kitting formula
```
qty = qty * 2
material = (w > 1000) ? "BIG" : "SMALL"
```

### 9.3. Opt használata
```
requiredLength = w - 20 + opt:premium ? (w/10)
qty = qty
material = "M1"
```

### 9.4. Több soros script
```
a = w - 10
b = a * 2
requiredLength = b
qty = qty
material = "AXLE"
```

---

## 10. Tesztlefedettség (a kódból)

- literálok
- egyszerű kifejezések
- ternary
- nested ternary
- opt
- opt + expression
- undefined function
- division by zero
- choose true/false (AST‑szintű choose)

---

## Zárás

Ez a dokumentum **100%-ban a jelenlegi forráskód alapján készült**, nincs benne semmi legacy, semmi régi DSL, semmi olyan, amit a FormulaEngine nem tud.
