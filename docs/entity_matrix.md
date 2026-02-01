# Entity Matrix — v2

## 1. Entitások
- ProductMaster
- NeedRule (Product → Material)
- NeedCalculation (Product → Mode)
- NeedCalculationDetail (Mode → Material)

## 2. Kapcsolatok
A mátrix anyaghalmaza:
NeedRule.rightId halmaz.

A mátrix módhalmaza:
NeedCalculation.productId == ProductMaster.id

A mátrix cellái:
NeedCalculationDetail.needCalculationId × NeedCalculationDetail.materialId

## 3. Követelmények
Minden materialhoz minden módhoz kell lennie detail rekordnak.

## 4. Formula mező
A formula mező DSL alapú:
- len:w-10
- len:h-10
- qty:fixed:3
- qty:perOrder:2
- qty:perArea:1000

## 5. Validációs pipeline
1. MissingDetail felismerése  
2. Formula validáció  
3. Material validáció  
4. UX jelzés  
5. Automatikus generálás (opcionális)  
