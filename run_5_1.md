---
title: "CutCalculator Prompt & Architecture Handbook v1.0"
subtitle: "Architektúra • Prompt DSL • UI/UX • Workflow"
author: "Horváth Zoltán"
date: "2025"
lang: "hu"
keywords:
  - fejlesztői kultúra
  - architektúra
  - naming convention
  - terminológia
  - best practice
  - CutCalculator
  - Qt
  - C++
  - engineering
geometry: margin=2.5cm
papersize: a4
pagestyle: plain
toc: true
toc-depth: 3
colorlinks: true
linkcolor: blue
---


# 0. Előszó

A CutCalculator nem egyszerűen egy alkalmazás — ez egy fejlesztési kultúra, egy gondolkodásmód, egy mérnöki ritmus.
Ebben a projektben a kód nem csak működik: **öröm ránézni, öröm karbantartani, öröm továbbfejleszteni**.

A projekt különlegessége, hogy a fejlesztés nem klasszikus módon történik.
Itt a fejlesztő és az AI **közös nyelvet beszél** — egy olyan nyelvet, amelyet mi magunk hoztunk létre:
a **CutCalculator Prompt DSL‑t**.

Ez a DSL nem egyszerű szabályrendszer.
Ez egy domain‑specific language, amely:

- meghatározza a kód stílusát
- meghatározza az architektúrát
- meghatározza a workflow‑t
- meghatározza a minőséget 
- meghatározza a vizuális identitást
- meghatározza a fejlesztői ritmust

A CutCalculator fejlesztése így válik:

- gyorssá
- konzisztenssé
- audit‑baráttá
- örömtelivé
- és hosszú távon is fenntarthatóvá

Ez a kézikönyv azért született, hogy:

- rögzítse a projekt nyelvét
- dokumentálja a fejlesztési kultúrát
- egységesítse a kódgenerálást
- megkönnyítse az onboardingot
- biztosítsa a minőséget
- és megőrizze azt a stílust, amit együtt kialakítottunk

A könyv nem csak szabályokat tartalmaz.
Ez egy élő dokumentum, amely:

- példákat ad
- sablonokat ad
- anti‑patternöket mutat
- workflow‑kat ír le
- architektúra‑mintákat ad
- és egy teljes terminológiai szótárt is tartalmaz

**A cél egyszerű:**
A CutCalculator legyen olyan projekt, amelyet öröm fejleszteni, öröm olvasni, és öröm továbbadni.

***

# 1. A CutCalculator fejlesztési kultúrája

A CutCalculator nem csupán egy alkalmazás, hanem egy fejlesztési ökoszisztéma, amelyben a kód, a dokumentáció, a UI és a workflow **egyetlen egységes nyelvet beszél**.
Ez a nyelv a **CutCalculator Prompt DSL**, és ez a fejezet írja le azt a kultúrát, amelyben ez a nyelv megszületett és működik.

A fejlesztési kultúra három pillére:

1. **Alapelvek** – hogyan gondolkodunk a kódról
2. **Vizuális identitás** – hogyan néz ki a kód
3. **Fejlesztői ritmus** – hogyan dolgozunk együtt

Mindhárom nélkülözhetetlen ahhoz, hogy a projekt hosszú távon is átlátható, örömteli és fenntartható maradjon.

***

## 1.1 A projekt alapelvei

A CutCalculator fejlesztése során minden döntés — legyen az architektúra, UI, naming vagy prompt — ezekre az alapelvekre épül.

### 1.1.1 Egyszerűség

A kód legyen:

- rövid
- tiszta
- érthető
- felesleges rétegek nélkül

Az egyszerűség nem szegénység — az egyszerűség a minőség jele.

### 1.1.2 Átláthatóság

A kódot úgy írjuk, hogy:

- egy pillantással érthető legyen
- a logikai blokkok jól elkülönüljenek
- a kommentek valódi értéket adjanak
- a naming egyértelmű legyen

A fejlesztő ne keressen — a kód beszéljen.

### 1.1.3 Auditálhatóság

A CutCalculator egyik legfontosabb értéke az audit‑barát működés:

- minden CRUD művelet logolva van
- a snapshot‑kezelés átlátható
- a UI állapotai rekonstruálhatók
- a registry‑k tisztán kezelik az adatot

Az audit nem teher — az audit a biztonság és a minőség garanciája.

### 1.1.4 Konzisztencia

A projekt minden része ugyanazt a stílust követi:

- ugyanazok a blokkfejlécek
- ugyanaz a naming
- ugyanaz a validációs minta
- ugyanaz a UI‑ritmus
- ugyanaz a prompt‑nyelv

A konzisztencia nem szabály — a konzisztencia a fejlesztői öröm forrása.

### 1.1.5 Emberi olvashatóság

A kódot emberek olvassák, nem gépek.

Ezért:

- a kommentek magyarul vannak
- a technikai terminusok angolul
- a kód vizuálisan tagolt
- a változónevek nem túl hosszúak
- a metódusnevek nem eposzok

A kód legyen olyan, mint egy jó szöveg: ritmusos, érthető, élvezetes.

### 1.1.6 Snapshot‑kompatibilitás

A UI‑k tervezésekor alapelv:

- minden dialógus pozíciója visszaállítható
- minden méret menthető
- minden layout stabil
- a felhasználói élmény reprodukálható

Ez a CutCalculator egyik védjegye.

### 1.1.7 Prompt‑vezérelt workflow

A fejlesztés nem „kódolás”, hanem:

- tervezés
- promptolás
- generálás
- refaktor
- teszt
- review

A prompt nem kérés — a prompt specifikáció.

***

## 1.2 A projekt vizuális identitása

A CutCalculator kódja felismerhető.
Nem csak működik — jól is néz ki.

A vizuális identitás három fő eleme:

### 1.2.1 Blokkfejlécek

Minden logikai fejezetet blokkfejléc választ el:

```
/* ============================================================
 * 🧩 Initialization
 * ============================================================ */
```

Ez:

- tagol
- fókuszt ad
- audit‑barát
- vizuálisan ritmust teremt

### 1.2.2 Emoji‑fókuszpontok

A kód kulcspontjait emojik jelölik:

- 🧩 komponens
- 🎨 UI
- 🧪 validáció
- ⚙️ inicializálás
- 🔧 helper
- 📦 adatkezelés
- 🖱️ pozicionálás

Ez nem játék — ez vizuális navigáció.

### 1.2.3 Magyar kommentek angol terminológiával

A kommentek:

- magyarul vannak
- de a technikai terminusok angolul
- nincs Hunglish
- nincs felesleges magyarázkodás

Ez a projekt nyelve.

***

## 1.3 A fejlesztői ritmus

A CutCalculator fejlesztése nem ad‑hoc.
Ez egy ritmus, amely minden modulnál ugyanúgy működik.

### 1.3.1 A fejlesztési ciklus

1. Tervezés
2. Prompt
3. Generálás
4. Refaktor
5. Teszt
6. Review

Ez a ciklus garantálja a minőséget.

### 1.3.2 A „checkpoint” kultúra

Minden nagyobb lépés előtt:

- megállunk
- átbeszéljük
- jóváhagyjuk

Ezért nincs káosz.
Ezért nincs félregenerált modul.
Ezért nincs stílustörés.

### 1.3.3 A közös nyelv szerepe

A CutCalculator Prompt DSL:

- egységesíti a kommunikációt
- csökkenti a félreértéseket
- gyorsítja a fejlesztést
- biztosítja a minőséget

Ez a projekt „lingua franca”-ja.

***

# 2. A CutCalculator Prompt DSL teljes nyelvtana

A CutCalculator Prompt DSL egy domain‑specific language, amelyet kifejezetten a projekt fejlesztési folyamataihoz terveztünk.
A DSL célja:

- egységesíteni a promptokat
- biztosítani a generált kód minőségét
- definiálni a projekt stílusát
- csökkenteni a félreértéseket
- gyorsítani a fejlesztést
- auditálhatóvá tenni a workflow‑t

A DSL három nagy rétegből áll:

1. **Szerkezeti nyelvtan** – hogyan épül fel egy prompt
2. **Stílusnyelvtan** – hogyan írjuk le a kód stílusát
3. **Viselkedési nyelvtan** – hogyan irányítjuk a generálást

Ez a fejezet mindhárom réteget részletesen definiálja.

***

## 2.1 A DSL filozófiája

A CutCalculator Prompt DSL alapelve:

**A prompt nem kérés, hanem specifikáció.**

A prompt:

- meghatározza a kontextust
- meghatározza a követelményeket
- meghatározza a workflow‑t
- meghatározza a stílust
- meghatározza a minőséget

A prompt tehát nem egy „utasítás”, hanem egy fejlesztési dokumentum, amelyből a kód automatikusan generálható.

A DSL célja, hogy:

- a promptok konzisztenssé váljanak
- a generált kód egységes legyen
- a projekt stílusa hosszú távon fennmaradjon
- a fejlesztés gyors és örömteli legyen

***

## 2.2 A DSL szerkezeti nyelvtana

A CutCalculator Prompt DSL három fő blokkot használ:

1. **[Kontextus]**
2. **[Követelmények]**
3. **[Workflow]**

Mindhárom blokk opcionális, de a jó prompt mindhármat tartalmazza.

***

### 2.2.1 A [Kontextus] blokk

Ez definiálja a világot, amelyben a kód létezik.

Szerepe:

- technológiai környezet
- architektúra
- projekt‑sajátosságok
- stílus‑ és nyelvi szabályok

**Szintaxis:**

```
[Kontextus]
- <elem>
- <elem>
- <elem>
```

**Példák:**

```
[Kontextus]
- C++20, Qt 6.10
- registry/repository/manager architektúra
- CSV perzisztencia
- magyar kommentek angol terminológiával
```

***

### 2.2.2 A [Követelmények] blokk

Ez definiálja, milyen legyen a generált kód.

Szerepe:

- stílus
- naming
- validáció
- UI‑szabályok
- audit‑szabályok
- snapshot‑szabályok

**Szintaxis:**

```
[Követelmények]
- <követelmény>
- <követelmény>
- <követelmény>
```

**Példák:**

```
[Követelmények]
- blokkfejlécek logikai fejezeteknél
- inline validation + accent frame
- snake_case fájlnevek
- audit log minden CRUD-nál
```

***

### 2.2.3 A [Workflow] blokk

Ez definiálja a generálás folyamatát.

Szerepe:

- lépések sorrendje
- tiltások
- jóváhagyási pontok
- moduláris bontás

**Szintaxis:**

```
[Workflow]
1. <lépés>
2. <lépés>
3. <lépés>
```

**Példák:**

```
[Workflow]
1. először csak fájllistát adj
2. ne generálj még kódot
3. kódgenerálás csak jóváhagyás után
```

***

## 2.3 A DSL stílusnyelvtana

A CutCalculator kódja felismerhető.
A stílusnyelvtan definiálja a vizuális és szerkezeti szabályokat.

***

### 2.3.1 Blokkfejlécek

A kód logikai fejezeteit blokkfejlécek választják el:

```
/* ============================================================
 * 🧩 <Blokk neve>
 * ============================================================ */
```

Szabályok:

- kötelező minden nagyobb logikai egység előtt
- emoji a blokk típusát jelzi
- vizuális ritmust ad
- audit‑barát

***

### 2.3.2 Emoji‑fókuszpontok

A kód kulcspontjait emojik jelölik:

- 🧩 komponens
- 🎨 UI
- 🧪 validáció
- ⚙️ inicializálás
- 🔧 helper
- 📦 adatkezelés
- 🖱️ pozicionálás

Szabályok:

- nem dekoráció
- vizuális navigáció
- konzisztens használat

***

### 2.3.3 Kommentnyelv

A kommentek:

- magyar mondatok
- angol technikai terminusok
- nincs Hunglish
- nincs felesleges magyarázkodás

**Példa:**

```cpp
// A dialógus pozicionálása az egér alá (cursor-based positioning)
```

***

### 2.3.4 Naming convention

**Fájlok:**
- snake_case

**Osztályok:**
- PascalCase

**Metódusok:**
- camelCase

**Registry API:**

- `find(id)` → alias a `findById()`‑re
- `findByX()` → minden más keresés
- `exists()`
- `insert()`
- `remove()`
- `rename()`

***

### 2.3.5 UI‑szabályok

A CutCalculator UI‑k saját stílusnyelvet követnek:

- inline validation
- accent frame
- shake animáció hibánál
- selectAll + focus
- snapshot‑kompatibilis layout
- egér alá pozicionálás

***

## 2.4 A DSL viselkedési nyelvtana

Ez határozza meg, hogyan viselkedik a generálás.

***

### 2.4.1 Tiltások

A tiltások a DSL egyik legfontosabb részei.

Példák:

- ne generálj még kódot
- ne írj snapshot‑mentést a dialogba
- ne írj túl hosszú változóneveket
- ne keverd a tervezést és a generálást
- ne írj imperatív promptot

***

### 2.4.2 Pozitív minták

A DSL ajánlott mintákat is tartalmaz.

Példák:

- javasolj moduláris bontást
- tartsd tiszteletben a projektstruktúrát
- audit log minden CRUD-nál
- UI legyen snapshot‑kompatibilis

***

### 2.4.3 Generálási pipeline

A CutCalculator generálási folyamata:

1. Fájllista
2. API‑terv
3. Kódgenerálás
4. Refaktor
5. Teszt
6. Review

Ez a pipeline garantálja a minőséget.

***

# 3. Példatár

A CutCalculator Prompt DSL akkor válik igazán erőssé, amikor a szabályok mellé **konkrét példák** is társulnak.
Ez a fejezet bemutatja:

- hogyan néz ki egy jó kontextusblokk
- hogyan néz ki egy jó követelményblokk
- hogyan néz ki egy jó workflow‑blokk
- hogyan néz ki egy komplett prompt
- hogyan néz ki egy rossz prompt (anti‑pattern)
- hogyan néz ki egy javított verzió

A példák valós CutCalculator‑helyzetekből indulnak ki: UI, registry, manager, refaktor, audit, snapshot.

***

## 3.1 Kontextusblokk példák

A kontextusblokk a prompt „világát” definiálja.
Ez a blokk adja meg, hogy milyen technológiában, milyen architektúrában, milyen stílusban dolgozunk.

### Példa 1 – UI komponens generálása

```
[Kontextus]
- Qt 6.10, C++20
- snapshot-kompatibilis UI
- magyar kommentek angol terminológiával
```

### Példa 2 – backend modul generálása

```
[Kontextus]
- C++20
- registry/repository/manager architektúra
- CSV perzisztencia
```

### Példa 3 – dialog refaktor

```
[Kontextus]
- meglévő Qt6 dialog
- inline validation már részben implementálva
- snapshot-kezelés még hiányzik
```

### Példa 4 – audit log bővítés

```
[Kontextus]
- registry CRUD műveletek
- audit log jelenleg INFO szintig implementálva
```

***

## 3.2 Követelményblokk példák

A követelményblokk mondja meg, milyen legyen a generált kód.

### Példa 1 – dialog követelmények

```
[Követelmények]
- inline validation + accent frame
- shake animáció hibánál
- selectAll + focus
- egér alá pozicionálás
```

### Példa 2 – registry követelmények

```
[Követelmények]
- find(id) alias findById()
- findByX() minden más kereséshez
- audit log minden CRUD-nál
- snake_case fájlnevek
```

### Példa 3 – manager követelmények

```
[Követelmények]
- tiszta signal-slot kapcsolatok
- blokkfejlécek logikai fejezeteknél
- inline validation támogatása
```

### Példa 4 – snapshot követelmények

```
[Követelmények]
- minden dialógus mérete menthető
- pozíció visszaállítása snapshot alapján
- fallback: egér alá pozicionálás
```

***

## 3.3 Workflow‑blokk példák

A workflow‑blokk a generálás folyamatát definiálja.

### Példa 1 – modul generálás

```
[Workflow]
1. először csak fájllistát adj
2. ne generálj még kódot
3. kódgenerálás csak jóváhagyás után
```

### Példa 2 – dialog generálás

```
[Workflow]
1. először csak layout-tervet adj
2. ne generálj még kódot
```

### Példa 3 – refaktor

```
[Workflow]
1. először csak problémalistát adj
2. ne írj még kódot
3. javasolj moduláris bontást
```

### Példa 4 – audit log bővítés

```
[Workflow]
1. először csak log-szabályokat adj
2. ne generálj még kódot
```

***

## 3.4 Komplett prompt példák

Itt látszik igazán, hogyan áll össze a DSL.

### Példa 1 – Új dialog generálása

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kompatibilis UI

[Követelmények]
- inline validation + accent frame
- shake animáció hibánál
- selectAll + focus
- egér alá pozicionálás

[Workflow]
1. először csak layout-tervet adj
2. ne generálj még kódot
```

***

### Példa 2 – Új registry generálása

```
[Kontextus]
- C++20
- registry/repository/manager architektúra

[Követelmények]
- find(id) alias findById()
- findByX() minden más kereséshez
- audit log minden CRUD-nál

[Workflow]
1. először csak API-tervet adj
2. ne generálj még kódot
```

***

### Példa 3 – Manager generálása

```
[Kontextus]
- Qt 6.10
- registry + view + dialog integráció

[Követelmények]
- blokkfejlécek
- tiszta signal-slot kapcsolatok
- inline validation támogatása

[Workflow]
1. először csak osztályvázlatot adj
2. ne generálj még kódot
```

***

### Példa 4 – Refaktor prompt

```
[Kontextus]
- meglévő Qt6 kód

[Követelmények]
- SRP
- blokkfejlécek
- magyar kommentek angol terminológiával

[Workflow]
1. először csak problémalistát adj
2. ne írj még kódot
3. javasolj moduláris bontást
```

***

## 3.5 Anti‑pattern példák (rossz → jó)

Ez a rész mutatja, mit **NE** csináljunk.

***

### ❌ Rossz példa 1 – túl rövid prompt

```
írj egy dialogot
```

**Miért rossz?**

- nincs kontextus
- nincs követelmény
- nincs workflow
- nincs stílus
- nincs projekt‑nyelv

**✔️ Javított verzió**

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kompatibilis UI

[Követelmények]
- inline validation
- accent frame
- shake animáció hibánál

[Workflow]
1. először csak layout-tervet adj
```

***

### ❌ Rossz példa 2 – imperatív prompt

```
csinálj egy registryt ami tud findot meg insertet
```

**Miért rossz?**

- nem deklaratív
- nem projekt‑nyelvű
- nem konzisztens

**✔️ Javított verzió**

```
[Kontextus]
- C++20
- registry/repository/manager architektúra

[Követelmények]
- find(id) alias findById()
- findByX() minden más kereséshez
- audit log minden CRUD-nál

[Workflow]
1. először csak API-tervet adj
```

***

### ❌ Rossz példa 3 – túl hosszú, kevert prompt

```
írj egy dialogot ami validál, legyen shake, legyen snapshot, legyen audit log, legyen registry is, meg manager is, meg UI is, meg minden
```

**Miért rossz?**

- keveri a modulokat
- túl sok felelősség
- nincs struktúra

**✔️ Javított verzió**

```
[Kontextus]
- Qt 6.10 dialog

[Követelmények]
- inline validation
- accent frame
- shake animáció hibánál

[Workflow]
1. először csak layout-tervet adj
```

***

# 4. Sablonkönyvtár
*A CutCalculator Prompt DSL standard library‑ja*

A sablonkönyvtár a CutCalculator Prompt DSL „standard library‑ja”.
Ezek a sablonok:

- gyorsítják a fejlesztést
- biztosítják a konzisztenciát
- egységesítik a generált kódot
- csökkentik a hibalehetőséget
- fenntartják a projekt vizuális identitását

A sablonok minden modulhoz és minden fejlesztési helyzethez adnak egy **alap prompt‑mintát**, amelyet csak testre kell szabni.

A fejezet 10+ sablont tartalmaz.

***

## 4.1 Új modul generálása

```
[Kontextus]
- C++20
- moduláris projektstruktúra
- registry/repository/manager architektúra

[Követelmények]
- snake_case fájlnevek
- blokkfejlécek logikai fejezeteknél
- magyar kommentek angol terminológiával

[Workflow]
1. először csak fájllistát adj
2. ne generálj még kódot
```

***

## 4.2 Új dialog generálása

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kompatibilis UI

[Követelmények]
- inline validation + accent frame
- shake animáció hibánál
- selectAll + focus
- egér alá pozicionálás

[Workflow]
1. először csak layout-tervet adj
2. ne generálj még kódot
```

***

## 4.3 Registry generálása

```
[Kontextus]
- C++20
- registry/repository/manager architektúra
- CSV perzisztencia

[Követelmények]
- find(id) alias findById()
- findByX() minden más kereséshez
- audit log minden CRUD-nál
- snake_case fájlnevek

[Workflow]
1. először csak API-tervet adj
2. ne generálj még kódot
```

***

## 4.4 Manager generálása

```
[Kontextus]
- Qt 6.10
- registry + view + dialog integráció

[Követelmények]
- tiszta signal-slot kapcsolatok
- blokkfejlécek
- inline validation támogatása

[Workflow]
1. először csak osztályvázlatot adj
2. ne generálj még kódot
```

***

## 4.5 View generálása

```
[Kontextus]
- Qt 6.10
- táblanézet (QTableView) + model

[Követelmények]
- snapshot-kompatibilis oszlopszélességek
- konzisztens header naming
- double-click → edit workflow

[Workflow]
1. először csak layout-tervet adj
2. ne generálj még kódot
```

***

## 4.6 Helper generálása

```
[Kontextus]
- C++20
- utility modul

[Követelmények]
- statikus metódusok
- magyar kommentek angol terminológiával
- blokkfejlécek

[Workflow]
1. először csak API-tervet adj
2. ne generálj még kódot
```

***

## 4.7 Refaktor sablon

```
[Kontextus]
- meglévő Qt6/C++20 kód

[Követelmények]
- SRP
- blokkfejlécek
- naming tisztítás
- inline validation egységesítése

[Workflow]
1. először csak problémalistát adj
2. ne írj még kódot
3. javasolj moduláris bontást
```

***

## 4.8 Audit log sablon

```
[Kontextus]
- registry CRUD műveletek
- audit log részben implementálva

[Követelmények]
- INFO/WARN/ERROR szintek
- minden CRUD művelet logolása
- emberi olvashatóságú üzenetek

[Workflow]
1. először csak log-szabályokat adj
2. ne generálj még kódot
```

***

## 4.9 Snapshot sablon

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kezelés hiányos

[Követelmények]
- méret mentése
- pozíció mentése
- fallback: egér alá pozicionálás

[Workflow]
1. először csak snapshot-tervet adj
2. ne generálj még kódot
```

***

## 4.10 UI‑pozicionálási sablon

```
[Kontextus]
- Qt 6.10 dialog
- cursor-based positioning

[Követelmények]
- egér alá pozicionálás
- képernyőn belüli korrekció
- snapshot fallback

[Workflow]
1. először csak pozicionálási logikát adj
2. ne generálj még kódot
```

***

# 5. Prompt Cookbook
*A CutCalculator fejlesztő „svájci bicskája”*

A Prompt Cookbook a CutCalculator Prompt DSL gyakorlati alkalmazását mutatja be.
Ez a fejezet válaszol a kérdésre:

**„Ha ezt akarod, így kérd.”**

A Cookbook minden fontos fejlesztési helyzetre ad egy **kész, bevált prompt‑mintát**, amelyet csak testre kell szabni.

A receptek lefedik:

- dialog generálás
- registry generálás
- manager generálás
- view generálás
- helper generálás
- refaktor
- audit log
- snapshot
- naming tisztítás
- workflow újratervezés

Ez a fejezet a CutCalculator fejlesztői eszköztárának egyik legfontosabb része.

***

## 5.1 „Ha új dialogot akarsz, így kérd”

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kompatibilis UI

[Követelmények]
- inline validation + accent frame
- shake animáció hibánál
- selectAll + focus
- egér alá pozicionálás

[Workflow]
1. először csak layout-tervet adj
2. ne generálj még kódot
```

***

## 5.2 „Ha registryt akarsz, így kérd”

```
[Kontextus]
- C++20
- registry/repository/manager architektúra
- CSV perzisztencia

[Követelmények]
- find(id) alias findById()
- findByX() minden más kereséshez
- audit log minden CRUD-nál
- snake_case fájlnevek

[Workflow]
1. először csak API-tervet adj
2. ne generálj még kódot
```

***

## 5.3 „Ha manager kell, így kérd”

```
[Kontextus]
- Qt 6.10
- registry + view + dialog integráció

[Követelmények]
- tiszta signal-slot kapcsolatok
- blokkfejlécek
- inline validation támogatása

[Workflow]
1. először csak osztályvázlatot adj
2. ne generálj még kódot
```

***

## 5.4 „Ha refaktort akarsz, így kérd”

```
[Kontextus]
- meglévő Qt6/C++20 kód

[Követelmények]
- SRP
- blokkfejlécek
- naming tisztítás
- inline validation egységesítése

[Workflow]
1. először csak problémalistát adj
2. ne írj még kódot
3. javasolj moduláris bontást
```

***

## 5.5 „Ha audit logot akarsz, így kérd”

```
[Kontextus]
- registry CRUD műveletek
- audit log részben implementálva

[Követelmények]
- INFO/WARN/ERROR szintek
- minden CRUD művelet logolása
- emberi olvashatóságú üzenetek

[Workflow]
1. először csak log-szabályokat adj
2. ne generálj még kódot
```

***

## 5.6 „Ha snapshot‑kompatibilis UI kell, így kérd”

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kezelés hiányos

[Követelmények]
- méret mentése
- pozíció mentése
- fallback: egér alá pozicionálás

[Workflow]
1. először csak snapshot-tervet adj
2. ne generálj még kódot
```

***

## 5.7 „Ha naming conventiont akarsz tisztítani, így kérd”

```
[Kontextus]
- meglévő Qt6/C++20 modul

[Követelmények]
- snake_case fájlnevek
- PascalCase osztályok
- camelCase metódusok
- konzisztens registry API

[Workflow]
1. először csak problémalistát adj
2. ne írj még kódot
```

***

## 5.8 „Ha workflow‑t akarsz újratervezni, így kérd”

```
[Kontextus]
- meglévő generálási folyamat
- több modul érintett

[Követelmények]
- moduláris pipeline
- jóváhagyási pontok
- tiltások tisztázása
- auditálható lépések

[Workflow]
1. először csak workflow-diagramot adj
2. ne generálj még kódot
```

***

# 6. Anti‑pattern gyűjtemény
*Mit NE csinálj a CutCalculator fejlesztése során*

Az anti‑pattern gyűjtemény a CutCalculator egyik legfontosabb fejezete.
A jó minták önmagukban nem elegendők — **tudni kell felismerni a rossz mintákat is**, hogy elkerüljük őket.

Ez a fejezet bemutatja:

- a leggyakoribb hibákat
- miért problémásak
- hogyan javíthatók
- hogyan írjuk át őket a CutCalculator Prompt DSL szerint

A cél:
**a projekt hosszú távú minőségének és konzisztenciájának megőrzése.**

***

## 6.1 Rossz promptok

### ❌ Anti‑pattern: túl rövid prompt

```
írj egy dialogot
```

**Miért rossz?**

- nincs kontextus
- nincs követelmény
- nincs workflow
- nem projekt‑nyelvű
- nem deklaratív

**✔️ Javított verzió**

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kompatibilis UI

[Követelmények]
- inline validation
- accent frame
- shake animáció hibánál

[Workflow]
1. először csak layout-tervet adj
```

***

### ❌ Anti‑pattern: imperatív prompt

```
csinálj egy registryt ami tud findot meg insertet
```

**Miért rossz?**

- imperatív, nem deklaratív
- nem használja a DSL‑t
- nem konzisztens a projekt nyelvével

**✔️ Javított verzió**

```
[Kontextus]
- C++20
- registry/repository/manager architektúra

[Követelmények]
- find(id) alias findById()
- findByX() minden más kereséshez
- audit log minden CRUD-nál

[Workflow]
1. először csak API-tervet adj
```

***

### ❌ Anti‑pattern: túl hosszú, kevert prompt

```
írj egy dialogot ami validál, legyen shake, legyen snapshot, legyen audit log, legyen registry is, meg manager is, meg UI is, meg minden
```

**Miért rossz?**

- keveri a modulokat
- túl sok felelősség
- nincs struktúra
- nem követi a projekt ritmusát

**✔️ Javított verzió**

```
[Kontextus]
- Qt 6.10 dialog

[Követelmények]
- inline validation
- accent frame
- shake animáció hibánál

[Workflow]
1. először csak layout-tervet adj
```

***

## 6.2 Rossz workflow‑k

### ❌ Anti‑pattern: kódgenerálás workflow nélkül

```
írj egy manager osztályt
```

**Miért rossz?**

- nincs pipeline
- nincs jóváhagyási pont
- nincs modularizálás
- nagy az esély a félregenerálásra

**✔️ Javított verzió**

```
[Workflow]
1. először csak osztályvázlatot adj
2. ne generálj még kódot
```

***

### ❌ Anti‑pattern: kevert lépések

```
először adj API-t, aztán generálj is kódot, de még ne mindent
```

**Miért rossz?**

- nem egyértelmű
- nem auditálható
- nem követi a projekt ritmusát

**✔️ Javított verzió**

```
[Workflow]
1. először csak API-tervet adj
2. ne generálj még kódot
```

***

## 6.3 Rossz UI‑minták

### ❌ Anti‑pattern: snapshot nélküli UI

- a dialógus nem jegyzi meg a méretét
- nem jegyzi meg a pozícióját
- minden megnyitáskor máshol jelenik meg

**Miért rossz?**

- instabil felhasználói élmény
- auditálhatatlan állapot

**✔️ Javított verzió**

- snapshot‑kompatibilis layout
- méret mentése
- pozíció mentése
- fallback: egér alá pozicionálás

***

### ❌ Anti‑pattern: inline validation hiánya

- a felhasználó nem kap azonnali visszajelzést
- hibák csak később derülnek ki

**✔️ Javított verzió**

- inline validation
- accent frame
- shake animáció hibánál

***

## 6.4 Rossz naming

### ❌ Anti‑pattern: inconsistent naming

```
class usermanager
void DoInsertUser()
int finduser()
```

**Miért rossz?**

- nem PascalCase
- nem camelCase
- nem konzisztens
- nehezen olvasható

**✔️ Javított verzió**

```
class UserManager
void insertUser()
int findUser()
```

***

## 6.5 Rossz architektúra

### ❌ Anti‑pattern: registry + UI keverése

```
registry->insertUser();
ui->refresh();
registry->removeUser();
```

**Miért rossz?**

- megsérti az SRP‑t
- nincs manager réteg
- UI és adatkezelés összekeveredik

**✔️ Javított verzió**

- Registry → adat
- Repository → perzisztencia
- Manager → UI integráció

***

## 6.6 Rossz validáció

### ❌ Anti‑pattern: késői validáció

- hibák csak mentéskor derülnek ki
- nincs vizuális visszajelzés

**✔️ Javított verzió**

- inline validation
- accent frame
- shake animáció

***

## 6.7 Rossz snapshot‑kezelés

### ❌ Anti‑pattern: csak méret mentése

**Miért rossz?**

- a pozíció elveszik
- a dialógus „ugrál”

**✔️ Javított verzió**

- méret + pozíció mentése
- fallback: egér alá pozicionálás

***

## 6.8 Rossz audit log minták

### ❌ Anti‑pattern: túl technikai log

```
INFO: insert ok
```

**Miért rossz?**

- nem emberi olvasható
- nem audit‑barát

**✔️ Javított verzió**

```
INFO: User inserted (id=42, name="Kiss Béla")
```

***

# 7. Architektúra‑minták
*A CutCalculator projekt gerince*

A CutCalculator architektúrája tudatosan egyszerű, mégis rendkívül stabil.
A projekt minden modulja ugyanazokra az alapelvekre épül, így:

- könnyen érthető
- könnyen bővíthető
- audit‑barát
- konzisztens
- hosszú távon fenntartható

Ez a fejezet bemutatja a projekt legfontosabb architektúra‑mintáit, amelyek a teljes kódbázis alapját adják.

***

## 7.1 Registry / Repository / Manager triád

A CutCalculator architektúrájának központi eleme a **Registry / Repository / Manager** triád.

Ez a három komponens együtt biztosítja:

- az adatkezelés tisztaságát
- a UI és az adat szétválasztását
- a perzisztencia stabilitását
- az auditálhatóságot
- a moduláris felépítést

### Registry
**In‑memory adatkezelés + audit log**

Feladatai:

- entitások tárolása memóriában
- keresések (`find`, `findByX`)
- beszúrás, törlés, átnevezés
- audit log minden CRUD műveletnél
- emberi olvashatóságú log üzenetek

A registry NEM:

- nem ment fájlba
- nem kommunikál a UI‑val
- nem validál UI‑szinten

### Repository
**CSV‑alapú perzisztencia**

Feladatai:

- CSV beolvasása
- CSV mentése
- hibakezelés
- fájlformátum stabilitás

A repository NEM:

- nem tart adatot memóriában
- nem validál
- nem kommunikál a UI‑val

### Manager
**UI ↔ adat integráció + validáció + workflow**

Feladatai:

- registry + view + dialog összekötése
- inline validation támogatása
- workflow lépések kezelése
- snapshot‑kompatibilis viselkedés
- UI frissítések
- hibakezelés

A manager NEM:

- nem tárol adatot
- nem ment fájlba
- nem tartalmaz üzleti logikát

***

## 7.2 Dialog‑tervezési minták

A CutCalculator dialogjai egységes mintát követnek.

### Kötelező elemek:

- inline validation
- accent frame
- shake animáció hibánál
- selectAll + focus
- snapshot‑kompatibilis méret és pozíció
- egér alá pozicionálás fallbackként

### Ajánlott szerkezet:

```
/* ============================================================
 * 🧩 Initialization
 * ============================================================ */

...

/* ============================================================
 * 🎨 UI Setup
 * ============================================================ */

...

/* ============================================================
 * 🧪 Validation
 * ============================================================ */

...

/* ============================================================
 * 🖱️ Positioning
 * ============================================================ */

...

/* ============================================================
 * 📦 Snapshot
 * ============================================================ */

...
```

Ez a vizuális ritmus a projekt egyik védjegye.

***

## 7.3 Snapshot‑kompatibilis UI minták

A snapshot‑kezelés a CutCalculator egyik legfontosabb UX‑eleme.

### Kötelező:

- méret mentése
- pozíció mentése
- fallback: egér alá pozicionálás
- stabil layout
- konzisztens widget‑hierarchia

### Példa snapshot workflow:

1. dialógus megnyitásakor:
   - ha van snapshot → alkalmazzuk
   - ha nincs → egér alá pozicionáljuk

2. dialógus bezárásakor:
   - méret mentése
   - pozíció mentése

***

## 7.4 Audit‑barát log minták

Az audit log célja:

- emberi olvashatóság
- visszakövethetőség
- hibakeresés támogatása

### Jó log minta:

```
INFO: User inserted (id=42, name="Kiss Béla")
INFO: User renamed (old="Kiss Béla", new="Kiss Béla Jr.")
WARN: Attempted delete of non-existing user (id=99)
```

### Rossz log minta:

```
INFO: insert ok
ERROR: fail
```

***

## 7.5 Naming convention minták

A projekt naming szabályai:

- **snake_case** → fájlok
- **PascalCase** → osztályok
- **camelCase** → metódusok
- **registry API** → `find`, `findByX`, `exists`, `insert`, `remove`, `rename`

### Jó példa:

```
user_registry.cpp
UserRegistry
findUser()
insertUser()
```

### Rossz példa:

```
UserRegistry.cpp
usermanager
DoInsertUser()
```

***

## 7.6 Blokkfejléces kódstílus minták

A blokkfejlécek a projekt vizuális identitásának alapjai.

### Példa:

```
/* ============================================================
 * 🧩 Initialization
 * ============================================================ */
```

### Szabályok:

- minden logikai fejezet előtt kötelező
- emoji jelzi a blokk típusát
- audit‑barát
- vizuális ritmust ad

***

# 8. Terminológiai szótár (Glossary)
*A CutCalculator projekt hivatalos nyelve*

A Terminológiai szótár célja, hogy minden fejlesztő **ugyanazt a nyelvet beszélje**.
A projektben használt fogalmaknak pontos, egyértelmű jelentése van — ez biztosítja a konzisztenciát, a gyors kommunikációt és az auditálhatóságot.

A Glossary hat nagy kategóriára oszlik:

1. Alapfogalmak
2. UI fogalmak
3. Architektúra fogalmak
4. Prompt DSL fogalmak
5. Audit fogalmak
6. Snapshot fogalmak
7. Domain fogalmak

***

## 8.1 Alapfogalmak

### **Projekt**
A CutCalculator teljes kódbázisa, dokumentációja, workflow‑ja és vizuális identitása.

### **Modul**
A projekt egy logikailag elkülönülő része (pl. registry, manager, dialog).

### **Entitás**
Egy adatstruktúra, amelyet a registry kezel (pl. User, Product, Rule).

### **Workflow**
A fejlesztési folyamat lépéseinek sorrendje (Tervezés → Prompt → Generálás → Refaktor → Teszt → Review).

***

## 8.2 UI fogalmak

### **Inline validation**
Valós idejű mezőellenőrzés, amely azonnali visszajelzést ad a felhasználónak.

### **Accent frame**
Vékony, színes keret, amely hibás mezőt jelöl.

### **Shake animáció**
Rövid rázkódás hibás input esetén.

### **Cursor‑based positioning**
A dialógus az egér pozíciója alá kerül megnyitáskor.

### **Snapshot‑kompatibilis UI**
A dialógus megjegyzi méretét és pozícióját.

***

## 8.3 Architektúra fogalmak

### **Registry**
In‑memory adatkezelő, audit loggal.

### **Repository**
CSV‑alapú perzisztencia réteg.

### **Manager**
UI ↔ adat integráció, validáció, workflow.

### **Triád**
A Registry / Repository / Manager hármas együttese.

### **SRP (Single Responsibility Principle)**
Egy modulnak csak egy felelőssége lehet.

***

## 8.4 Prompt DSL fogalmak

### **[Kontextus] blokk**
A technológiai és architekturális környezet leírása.

### **[Követelmények] blokk**
A stílus, minőség, UI‑szabályok, naming és audit elvárások.

### **[Workflow] blokk**
A generálás lépései, tiltások, jóváhagyási pontok.

### **Tiltás**
Olyan szabály, amely megakadályozza a félregenerálást (pl. „ne generálj még kódot”).

### **Pozitív minta**
Ajánlott viselkedés vagy szerkezet.

***

## 8.5 Audit fogalmak

### **Audit log**
Minden CRUD művelet emberi olvasható naplózása.

### **INFO**
Normál működés.

### **WARN**
Gyanús vagy nem várt helyzet.

### **ERROR**
Hiba, amely beavatkozást igényel.

### **Audit‑barát kód**
Olyan kód, amelyből a műveletek visszakövethetők.

***

## 8.6 Snapshot fogalmak

### **Snapshot**
A dialógus állapotának (méret, pozíció) mentése.

### **Snapshot restore**
A dialógus visszaállítása a korábbi állapotba.

### **Fallback positioning**
Ha nincs snapshot, a dialógus az egér alá kerül.

***

## 8.7 Domain fogalmak

### **Mode**
A CutCalculator egyik fő kategóriája (pl. Cutting Mode).

### **Rule**
Egy szabály, amely meghatározza a működést.

### **Detail**
Egy szabályhoz tartozó részadat.

### **Product**
A projekt egyik alapentitása.

***

# 9. Függelék
*Kiegészítő anyagok, példák, diagramok, vizuális szabályok*

A Függelék a CutCalculator kézikönyv „hátsó zsebében” hordott eszköztára.
Ez a fejezet tartalmaz minden olyan kiegészítő anyagot, amely:

- segíti a fejlesztést
- tisztázza a stílust
- példákat ad a helyes megoldásokra
- dokumentálja a projekt vizuális identitását
- összegyűjti a teljes prompt‑mintakészletet

Ez a fejezet nem kötelező olvasmány, de **nagyon hasznos referencia**.

***

## 9.1 Kódstílus‑példák

A CutCalculator kódstílusa vizuálisan tagolt, audit‑barát és konzisztens.

### Blokkfejlécek

```
/* ============================================================
 * 🧩 Initialization
 * ============================================================ */
```

### UI Setup blokk

```
/* ============================================================
 * 🎨 UI Setup
 * ============================================================ */
```

### Validation blokk

```
/* ============================================================
 * 🧪 Validation
 * ============================================================ */
```

### Snapshot blokk

```
/* ============================================================
 * 📦 Snapshot
 * ============================================================ */
```

***

## 9.2 Prompt‑minták teljes listája

Ez a szakasz összegyűjti a könyvben szereplő összes prompt‑mintát.

### Új dialog

```
[Kontextus]
- Qt 6.10 dialog
- snapshot-kompatibilis UI

[Követelmények]
- inline validation + accent frame
- shake animáció hibánál
- selectAll + focus
- egér alá pozicionálás

[Workflow]
1. először csak layout-tervet adj
2. ne generálj még kódot
```

### Új registry

```
[Kontextus]
- C++20
- registry/repository/manager architektúra
- CSV perzisztencia

[Követelmények]
- find(id) alias findById()
- findByX() minden más kereséshez
- audit log minden CRUD-nál

[Workflow]
1. először csak API-tervet adj
2. ne generálj még kódot
```

### Új manager

```
[Kontextus]
- Qt 6.10
- registry + view + dialog integráció

[Követelmények]
- tiszta signal-slot kapcsolatok
- blokkfejlécek
- inline validation támogatása

[Workflow]
1. először csak osztályvázlatot adj
2. ne generálj még kódot
```

### Refaktor

```
[Kontextus]
- meglévő Qt6/C++20 kód

[Követelmények]
- SRP
- blokkfejlécek
- naming tisztítás

[Workflow]
1. először csak problémalistát adj
2. ne írj még kódot
3. javasolj moduláris bontást
```

### Audit log

```
[Kontextus]
- registry CRUD műveletek

[Követelmények]
- INFO/WARN/ERROR szintek
- emberi olvashatóságú üzenetek

[Workflow]
1. először csak log-szabályokat adj
2. ne generálj még kódot
```

### Snapshot

```
[Kontextus]
- Qt 6.10 dialog

[Követelmények]
- méret mentése
- pozíció mentése
- fallback: egér alá pozicionálás

[Workflow]
1. először csak snapshot-tervet adj
2. ne generálj még kódot
```

***

## 9.3 Ajánlott workflow‑diagramok

### Generálási pipeline

```
Tervezés
   ↓
Prompt
   ↓
Generálás
   ↓
Refaktor
   ↓
Teszt
   ↓
Review
```

### Registry / Repository / Manager triád

```
UI
 ↓
Manager
 ↓
Registry
 ↓
Repository
```

### Snapshot workflow

```
Megnyitás:
  ha van snapshot → alkalmaz
  ha nincs → egér alá pozicionál

Bezárás:
  méret mentése
  pozíció mentése
```

***

## 9.4 A CutCalculator vizuális identitásának szabályai

A projekt vizuális identitása a következő elemekből áll:

### 1. Blokkfejlécek

- kötelezők
- emoji jelzi a blokk típusát
- audit‑barát
- vizuális ritmust adnak

### 2. Emoji‑fókuszpontok

- 🧩 komponens
- 🎨 UI
- 🧪 validáció
- ⚙️ inicializálás
- 🔧 helper
- 📦 adatkezelés
- 🖱️ pozicionálás

### 3. Kommentnyelv

- magyar mondatok
- angol technikai terminusok
- nincs Hunglish

### 4. Naming convention

- snake_case → fájlok
- PascalCase → osztályok
- camelCase → metódusok

### 5. Snapshot‑kompatibilis UI

- méret mentése
- pozíció mentése
- fallback: egér alá pozicionálás

***
