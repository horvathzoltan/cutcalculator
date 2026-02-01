# Registry Matrix — v2 Specification

## 1. Fogalmi modell
A mátrix három dimenzióból áll:
- ProductMaster
- NeedCalculation (mode)
- MaterialMaster (NeedRule.rightId)

Egy cella = NeedCalculationDetail rekord.

## 2. Teljesség definíció
A mátrix akkor teljes, ha:
minden product × mode × material kombinációhoz létezik legalább egy detail.

## 3. Validáció
A MatrixValidator v2 három szinten validál:
- validateMode(modeId)
- validateProduct(productId)
- validateAll()

A validáció MissingDetail struktúrákat ad vissza.

## 4. Formula validáció
Három állapot:
- "" vagy "unknown" → unknown (valid, de jelölendő)
- hibás DSL → invalid
- helyes DSL → valid

## 5. Generálás
A MatrixGenerator v2 automatikusan létrehozza a hiányzó detail cellákat:
- formula = ""
- kind = Cutting
- determinisztikus sorrend
- audit log

## 6. UX integráció
A View réteg ikonokkal jelzi a mátrix állapotát.
A Presenter automatikusan a hibás sorra ugrik.
