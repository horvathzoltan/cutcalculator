---
title: "CutCalculator Engineering Culture Handbook"
subtitle: "Terminológia • Naming • Architektúra • Workflow • Best Practice"
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

# **0. Előszó**

A fejlesztői kultúra a projekt láthatatlan alaprétege: a gondolkodásmód, a szokások és a közös nyelv rendszere.
Ez a fejezet megágyaz a teljes kézikönyvnek, és bemutatja, miért nélkülözhetetlen a tudatos kultúra a hosszú távon fenntartható fejlesztéshez.

A modern szoftverfejlesztés nem csupán technikai tevékenység.
A kód mögött **gondolkodásmód**, **szokások**, **ritmus**, **közös nyelv**, és **kultúra** áll.
Egy projekt akkor lesz hosszú távon fenntartható, ha nem csak működik — hanem **érthető**, **átlátható**, **auditálható**, és **örömmel fejleszthető**.

Ez a dokumentum ennek a kultúrának az alapköve.

***

## **A fejlesztői kultúra szerepe**

A fejlesztői kultúra az a láthatatlan infrastruktúra, amely:

- meghatározza, hogyan gondolkodunk a kódról
- hogyan kommunikálunk egymással
- hogyan építünk modulokat
- hogyan kezeljük a hibákat
- hogyan tartjuk fenn a minőséget
- hogyan fejlődünk együtt

A jó kultúra:

- csökkenti a hibákat
- gyorsítja a fejlesztést
- stabilizálja az architektúrát
- csökkenti a belépési küszöböt
- növeli a projekt élettartamát

A rossz kultúra pedig — még jó kód mellett is — lassan szétesik.

Ez a kézikönyv azt a célt szolgálja, hogy a fejlesztői kultúra **láthatóvá, tanulhatóvá és átadhatóvá** váljon.

***

## **Miért fontos a közös nyelv**

A közös nyelv nem esztétikai kérdés — hanem **architektúra**.
Ha a fejlesztők ugyanazokat a fogalmakat használják:

- a domain tisztább
- a kód érthetőbb
- a modulok konzisztenssé válnak
- a hibák száma csökken
- a kommunikáció gyorsul

A közös nyelv a projekt „DNS‑e”.
Ezért a dokumentum egyik legfontosabb része a **Terminológiai Szótár**, amely minden fogalmat:

- definiál
- példával illusztrál
- anti‑patternnel tisztít
- naminggel rögzít

Ez a szótár a fejlesztői kultúra alapja.

***

## **A CutCalculator mint példaprojekt**

A CutCalculator nem csak egy alkalmazás — hanem **egy működő, élő példaprojekt**, amely:

- tiszta domain‑szétválasztást alkalmaz
- moduláris architektúrát valósít meg
- snapshot‑tudatos UI‑t használ
- audit‑barát logolást épít
- konzisztens naminget követ
- prompt‑vezérelt fejlesztési workflow‑t használ
- fejlesztői kultúrát teremt

A CutCalculator nem a dokumentum tárgya — hanem **példája**.
A cél nem az, hogy a projektet dokumentáljuk, hanem hogy a projektből tanult mintákat **általánosítsuk**, és **projektfüggetlen best practice‑ként** rögzítsük.

Ez a kézikönyv tehát nem a CutCalculator dokumentációja — hanem **egy fejlesztői kultúra kézikönyve**, amelyet a CutCalculator inspirált.

***

## **A dokumentum célja és hatóköre**

Ez a kézikönyv:

- egységes fejlesztői kultúrát rögzít
- projektfüggetlen best practice‑t ad
- közös nyelvet definiál
- architektúrát és workflow‑t tanít
- UI/UX irányelveket ad
- anti‑patternöket gyűjt
- sablonokat és mintákat biztosít
- formalizálja a Prompt DSL‑t
- fejlesztői gondolkodást formál

A dokumentum hatóköre:

- C++/Qt alapú desktop alkalmazások
- domain‑vezérelt architektúrák
- snapshot‑kompatibilis UI‑k
- audit‑barát rendszerek
- moduláris workflow‑k
- AI‑támogatott fejlesztés

A cél:
**egy olyan fejlesztői kultúra megalapozása, amely hosszú távon is fenntartható, örömmel fejleszthető, és új fejlesztők számára is gyorsan átadható.**

***

## **1. Alapelvek és filozófia**

Ez a fejezet rögzíti a CutCalculator fejlesztői kultúrájának alapvető gondolkodási mintáit: konzisztencia, modularitás, domain‑vezérelt szemlélet és snapshot‑tudatosság.
A cél, hogy a technikai döntések mögött álló filozófia minden fejlesztő számára érthető és követhető legyen.

### 1.1 A fejlesztői kultúra jelentősége
### 1.2 A konzisztencia mint elsődleges érték
### 1.3 A domain‑vezérelt gondolkodás
### 1.4 A moduláris architektúra előnyei
### 1.5 A snapshot‑kompatibilis UI mint ipari követelmény
### 1.6 A fejlesztő–AI együttműködés alapelvei

***

## **2. Terminológiai Szótár (Developer Lexicon)**
*(fogalom → definíció → példa → anti‑pattern → naming)*

A közös nyelv a fejlesztői kultúra alapja.
Ez a fejezet definiálja a projektben használt fogalmakat, példákkal, anti‑patternökkel és naming‑szabályokkal, hogy minden fejlesztő ugyanazokat a kifejezéseket ugyanabban az értelemben használja.

### 2.1 Domain fogalmak
- Need
- NeedCalculation
- NeedCalculationDetail
- Mode
- Material
- Rule
- FormulaStep
- Pipeline
- Snapshot
- LayoutDefault
- GeometryGuard
- EventLog
- SystemLog

### 2.2 Architektúra fogalmak
- Repository
- Registry
- Manager
- Presenter
- View
- Dialog
- Picker
- Editor
- Preview

### 2.3 UI fogalmak
- inline validation
- accent frame
- shake animation
- cursor‑based positioning
- deferred restore
- percent‑based layout

### 2.4 Logolási fogalmak
- audit log
- verbose routing
- system log
- event log

***

## **3. Naming Convention Guide**

A név nem dekoráció, hanem architektúra.
Ez a fejezet rögzíti a projektfüggetlen névadási szabályokat, amelyek biztosítják a kód olvashatóságát, kereshetőségét és hosszú távú stabilitását.

### 3.1 Általános naming elvek
- rövid, de beszédes
- domain‑orientált
- konzisztens
- kerülendő minták

### 3.2 Fájlnév konvenciók
### 3.3 Osztálynév konvenciók
### 3.4 Metódusnév konvenciók
### 3.5 Registry/Repository/Manager naming
### 3.6 UI komponensek naming
### 3.7 Dialogok és Editorok naming
### 3.8 Pipeline elemek naming
### 3.9 Validációs komponensek naming
### 3.10 Anti‑pattern naming példák

***

## **4. Architektúra és rendszertervezés**

Ez a fejezet bemutatja a CutCalculator architektúráját mint referenciamodellt: domain‑szétválasztás, Registry/Repository/Manager triád, snapshot‑tudatos UI és audit‑barát logolás.
A cél egy olyan architektúra bemutatása, amely stabil, moduláris és projektfüggetlenül is alkalmazható.

### 4.1 A CutCalculator architektúra mint referenciamodell
### 4.2 Domain‑szétválasztás mint ipari best practice
### 4.3 Registry/Repository/Manager triád
### 4.4 Snapshot‑architektúra
- SnapshotManager
- LayoutDefaultStore
- GeometryHelper
- ScreenChangeDetector

### 4.5 UI‑architektúra
- Presenter/Manager
- View
- Dialog
- Picker
- Editor

## **4.6 Logolási architektúra**
### 4.6.1 A logolás szerepe az architektúrában
### 4.6.2 A Logger háromrétegű modellje
### 4.6.3 DebugLog szerepe (TRACE, DEBUG)
### 4.6.4 SystemLog szerepe (INFO, WARNING, ERROR)
### 4.6.5 EventLog szerepe (UI workflow)
### 4.6.6 Időbélyegzés és ipari szabványok
### 4.6.7 Audit‑barát logolás
### 4.6.8 Logger API szereptérkép
### 4.6.9 Logolási döntési fa
### 4.6.10 A logikonok színének ergonomiai szerepe

### 4.7 Validációs architektúra
- inline validation
- domain validation
- UI validation

***

## **5. Workflow és fejlesztési ritmus**

A fejlesztés nem csak kódolás, hanem ritmus, szokás és gondolkodásmód.
Ez a fejezet rögzíti a CutCalculator alatt kialakult workflow‑t: prompt‑vezérelt fejlesztés, moduláris ciklusok, snapshot‑tudatosság és napzáró rituálék.

### 5.1 A fejlesztői workflow alapelvei
### 5.2 Prompt‑vezérelt fejlesztés
### 5.3 Moduláris fejlesztési ciklus
### 5.4 Snapshot‑tudatos UI fejlesztés
### 5.5 Audit‑barát fejlesztés
### 5.6 Refaktorálási protokoll
### 5.7 Napzáró és állapottábla mint rituálé

***

## **6. Best Practice gyűjtemény**

Ez a fejezet összegyűjti a projekt során bizonyított mintákat: architektúra, naming, UI, logolás, validáció és snapshot‑kezelés.
A cél egy olyan best practice készlet, amely bármely C++/Qt projektben alkalmazható.

### 6.1 Architektúra best practice
### 6.2 Naming best practice
### 6.3 UI best practice
### 6.4 Logolási best practice
### 6.5 Validációs best practice
### 6.6 Snapshot‑kezelési best practice
### 6.7 Prompt‑vezérelt fejlesztési best practice

***

## **7. Anti‑pattern gyűjtemény**

A jó kultúra nem csak azt mondja meg, mit kell csinálni — hanem azt is, mit kell elkerülni.
Ez a fejezet összegyűjti a leggyakoribb anti‑patternöket, amelyek rontják a kód minőségét, a UI élményt vagy az architektúra stabilitását.

### 7.1 Architektúra anti‑pattern
### 7.2 Naming anti‑pattern
### 7.3 UI anti‑pattern
### 7.4 Logolási anti‑pattern
### 7.5 Validációs anti‑pattern
### 7.6 Snapshot anti‑pattern
### 7.7 Prompt anti‑pattern

***

## **8. Sablonok és boilerplate minták**

A sablonok gyorsítják a fejlesztést és biztosítják a konzisztenciát.
Ez a fejezet olyan boilerplate mintákat tartalmaz, amelyek a Registry/Repository/Manager triádra, dialogokra, editorokra, pickerekre és snapshot‑kompatibilis UI‑kra épülnek.

### 8.1 Registry sablon
### 8.2 Repository sablon
### 8.3 Manager sablon
### 8.4 Dialog sablon
### 8.5 Picker sablon
### 8.6 Editor sablon
### 8.7 Snapshot‑kompatibilis UI sablon
### 8.8 Logolási sablonok

***

## **9. Prompt DSL és Prompt Engineering**

A prompt nem kérés — hanem specifikáció.
Ez a fejezet bemutatja a CutCalculator Prompt DSL‑t, amely egységesíti az AI‑val való együttműködést, és biztosítja, hogy a generált kód konzisztens, auditálható és domain‑orientált legyen.

### 9.1 A prompt mint specifikáció
### 9.2 A prompt szerkezete (DSL)
### 9.3 Prompt standard library
### 9.4 Prompt anti‑patternök
### 9.5 Prompt workflow
### 9.6 Prompt mint fejlesztői kultúra

***

## **10. UI/UX Guidelines**

A felhasználói élmény a rendszer működésének része.
Ez a fejezet összefoglalja a modern, ergonomikus, snapshot‑tudatos UI kialakításának alapelveit: layout, interakciók, vizuális ritmus, hibakezelés és UX anti‑patternök.

### 10.1 Alapelvek
### 10.2 Layout és elrendezés
### 10.3 Interakciós minták
### 10.4 Snapshot‑tudatos UI
### 10.5 Vizuális ritmus és hierarchia
### 10.6 Hibakezelés és visszajelzés
### 10.7 UX anti‑patternök

***

## **11. Ipari minták és inspirációk**

Ez a fejezet bemutatja, hogyan kapcsolódik a CutCalculator fejlesztői kultúrája a világ vezető desktop alkalmazásainak gyakorlatához.
A snapshot‑tudatos UI‑tól a moduláris architektúrán át a non‑blocking UX‑ig összegyűjti azokat az ipari mintákat, amelyek a modern szoftverfejlesztés alapját adják.

### 11.1 Snapshot‑tudatos UI mint ipari alapelv
### 11.2 Moduláris architektúra és komponens‑szétválasztás
### 11.3 Auditálható workflow és visszavonhatóság
### 11.4 Non‑blocking UX és modern interakciós minták
### 11.5 Percent‑based layout és DPI‑független megjelenítés
### 11.6 Pipeline‑alapú feldolgozás és lépésrendszer
### 11.7 Naming és konzisztencia mint vállalati kultúra
### 11.8 Logolási architektúrák és szétválasztott logcsatornák
### 11.9 Prompt‑vezérelt fejlesztés mint modern workflow
### 11.10 Fejlesztői kultúra mint termékfilozófia

***

# **0. Előszó**

A fejlesztői kultúra a projekt láthatatlan alaprétege: a gondolkodásmód, a szokások és a közös nyelv rendszere.
Ez a fejezet megágyaz a teljes kézikönyvnek, és bemutatja, miért nélkülözhetetlen a tudatos kultúra a hosszú távon fenntartható fejlesztéshez.

A modern szoftverfejlesztés nem csupán technikai tevékenység.
A kód mögött **gondolkodásmód**, **szokások**, **ritmus**, **közös nyelv**, és **kultúra** áll.
Egy projekt akkor lesz hosszú távon fenntartható, ha nem csak működik — hanem **érthető**, **átlátható**, **auditálható**, és **örömmel fejleszthető**.

Ez a dokumentum ennek a kultúrának az alapköve.

***

## **A fejlesztői kultúra szerepe**

A fejlesztői kultúra az a láthatatlan infrastruktúra, amely:

- meghatározza, hogyan gondolkodunk a kódról
- hogyan kommunikálunk egymással
- hogyan építünk modulokat
- hogyan kezeljük a hibákat
- hogyan tartjuk fenn a minőséget
- hogyan fejlődünk együtt

A jó kultúra:

- csökkenti a hibákat
- gyorsítja a fejlesztést
- stabilizálja az architektúrát
- csökkenti a belépési küszöböt
- növeli a projekt élettartamát

A rossz kultúra pedig — még jó kód mellett is — lassan szétesik.

Ez a kézikönyv azt a célt szolgálja, hogy a fejlesztői kultúra **láthatóvá, tanulhatóvá és átadhatóvá** váljon.

***

## **Miért fontos a közös nyelv**

A közös nyelv nem esztétikai kérdés — hanem **architektúra**.
Ha a fejlesztők ugyanazokat a fogalmakat használják:

- a domain tisztább
- a kód érthetőbb
- a modulok konzisztenssé válnak
- a hibák száma csökken
- a kommunikáció gyorsul

A közös nyelv a projekt „DNS‑e”.
Ezért a dokumentum egyik legfontosabb része a **Terminológiai Szótár**, amely minden fogalmat:

- definiál
- példával illusztrál
- anti‑patternnel tisztít
- naminggel rögzít

Ez a szótár a fejlesztői kultúra alapja.

***

## **A CutCalculator mint példaprojekt**

A CutCalculator nem csak egy alkalmazás — hanem **egy működő, élő példaprojekt**, amely:

- tiszta domain‑szétválasztást alkalmaz
- moduláris architektúrát valósít meg
- snapshot‑tudatos UI‑t használ
- audit‑barát logolást épít
- konzisztens naminget követ
- prompt‑vezérelt fejlesztési workflow‑t használ
- fejlesztői kultúrát teremt

A CutCalculator nem a dokumentum tárgya — hanem **példája**.
A cél nem az, hogy a projektet dokumentáljuk, hanem hogy a projektből tanult mintákat **általánosítsuk**, és **projektfüggetlen best practice‑ként** rögzítsük.

Ez a kézikönyv tehát nem a CutCalculator dokumentációja — hanem **egy fejlesztői kultúra kézikönyve**, amelyet a CutCalculator inspirált.

***

## **A dokumentum célja és hatóköre**

Ez a kézikönyv:

- egységes fejlesztői kultúrát rögzít
- projektfüggetlen best practice‑t ad
- közös nyelvet definiál
- architektúrát és workflow‑t tanít
- UI/UX irányelveket ad
- anti‑patternöket gyűjt
- sablonokat és mintákat biztosít
- formalizálja a Prompt DSL‑t
- fejlesztői gondolkodást formál

A dokumentum hatóköre:

- C++/Qt alapú desktop alkalmazások
- domain‑vezérelt architektúrák
- snapshot‑kompatibilis UI‑k
- audit‑barát rendszerek
- moduláris workflow‑k
- AI‑támogatott fejlesztés

A cél:
**egy olyan fejlesztői kultúra megalapozása, amely hosszú távon is fenntartható, örömmel fejleszthető, és új fejlesztők számára is gyorsan átadható.**

***

# **1. Alapelvek és filozófia**

Ez a fejezet rögzíti a CutCalculator fejlesztői kultúrájának alapvető gondolkodási mintáit: konzisztencia, modularitás, domain‑vezérelt szemlélet és snapshot‑tudatosság.
A cél, hogy a technikai döntések mögött álló filozófia minden fejlesztő számára érthető és követhető legyen.

***

## **1.1 A fejlesztői kultúra jelentősége**

A fejlesztői kultúra az a láthatatlan struktúra, amely meghatározza:

- hogyan gondolkodunk a kódról
- hogyan kommunikálunk egymással
- hogyan építünk modulokat
- hogyan kezeljük a hibákat
- hogyan tartjuk fenn a minőséget
- hogyan fejlődünk együtt

A jó kultúra:

- csökkenti a hibákat
- gyorsítja a fejlesztést
- stabilizálja az architektúrát
- csökkenti a belépési küszöböt
- növeli a projekt élettartamát

A rossz kultúra pedig — még jó kód mellett is — lassan szétesik.

Ez a dokumentum azért született, hogy a fejlesztői kultúra **láthatóvá, tanulhatóvá és átadhatóvá** váljon.

***

## **1.2 A konzisztencia mint elsődleges érték**

A konzisztencia nem esztétikai kérdés — hanem **architektúra**.

A konzisztens rendszer:

- gyorsabban fejleszthető
- könnyebben érthető
- kevesebb hibát tartalmaz
- könnyebben tesztelhető
- könnyebben refaktorálható
- könnyebben átadható új fejlesztőknek

A konzisztencia három szinten jelenik meg:

### **1.2.1 Koncepciószintű konzisztencia**
Ugyanaz a fogalom → ugyanaz a név → ugyanaz a szerep.

### **1.2.2 Architektúra‑szintű konzisztencia**
Ugyanaz a minta → ugyanaz a struktúra → ugyanaz a workflow.

### **1.2.3 UI‑szintű konzisztencia**
Ugyanaz a vizuális logika → ugyanaz a visszajelzés → ugyanaz a ritmus.

A konzisztencia a fejlesztői kultúra egyik legfontosabb pillére.

***

## **1.3 A domain‑vezérelt gondolkodás**

A domain‑vezérelt gondolkodás lényege:

- a rendszer fogalmait a valós üzleti fogalmakból vezetjük le
- a domain a projekt „nyelve”
- a domain a projekt „modellje”
- a domain a projekt „igazságforrása”

A domain‑vezérelt gondolkodás előnyei:

- tiszta architektúra
- jobb kommunikáció
- kevesebb félreértés
- könnyebb tesztelhetőség
- stabilabb rendszer

A CutCalculator domainje példaszerűen tiszta:

- Need
- NeedCalculation
- NeedCalculationDetail
- Mode
- Material
- Rule
- Pipeline
- Snapshot

A dokumentum Terminológiai Szótára ezt a gondolkodásmódot rögzíti.

***

## **1.4 A moduláris architektúra előnyei**

A moduláris architektúra célja:

- a felelősségek szétválasztása
- a kód újrafelhasználhatósága
- a tesztelhetőség javítása
- a hibák lokalizálása
- a fejlesztés gyorsítása

A CutCalculator moduláris mintája:

- Repository → IO
- Registry → állapot
- Manager → műveletek
- Presenter → UI‑logika
- View → megjelenítés
- Dialog / Picker / Editor → interakció
- SnapshotManager → UI állapot
- SystemLog / EventLog / AuditLog → logolás

Ez a struktúra projektfüggetlenül is alkalmazható.

***

## **1.5 A snapshot‑kompatibilis UI mint ipari követelmény**

A modern desktop alkalmazások egyik legfontosabb UX‑elve:

**A felhasználó UI‑ja nem változhat meg újraindításkor.**

A snapshot‑kompatibilis UI:

- megőrzi az elrendezést
- megőrzi a pozíciókat
- megőrzi a méreteket
- megőrzi a táblák állapotát
- megőrzi a munkakörnyezetet

A snapshot‑architektúra négy pillére:

- SnapshotManager
- LayoutDefaultStore
- GeometryGuard
- ScreenChangeDetector

Ez nem extra — hanem **ipari követelmény**.

***

## **1.6 A fejlesztő–AI együttműködés alapelvei**

A modern fejlesztésben az AI nem eszköz — hanem **partner**.

A jó együttműködés alapelvei:

### **1.6.1 A fejlesztő adja a kontextust**
A fejlesztő:

- meghatározza a domain fogalmait
- kijelöli a moduláris határokat
- definiálja a naminget
- meghatározza a workflow‑t

### **1.6.2 Az AI adja a gyorsítást**
Az AI:

- generálja a boilerplate‑et
- segít a namingben
- segít a dokumentációban
- segít a refaktorálásban
- segít a sablonokban

### **1.6.3 A prompt mint specifikáció**
A prompt:

- kontextust ad
- határokat ad
- példát ad
- anti‑patternöket tilt

Ezért született meg a dokumentum **Prompt DSL** fejezete.

***
# **2. Terminológiai Szótár (Developer Lexicon)**
*A közös nyelv a fejlesztői kultúra alapja.*

A Terminológiai Szótár célja, hogy a projekt minden résztvevője **ugyanazokat a fogalmakat ugyanabban az értelemben használja**.
Ez a fejlesztői kultúra egyik legfontosabb pillére: a közös nyelv.

A szótár négy nagy területre oszlik:

- **2.1 Domain fogalmak**
- **2.2 Architektúra fogalmak**
- **2.3 UI fogalmak**
- **2.4 Logolási fogalmak**

A fogalmak minden esetben tartalmazzák:

- definíció
- példa
- anti‑pattern
- naming‑szabály

A 9. és 10. fejezet (Prompt DSL + UI/UX Guidelines) szellemisége is beépül:
a fogalmak **prompt‑kompatibilisek**, **UI/UX‑tudatosak**, és **snapshot‑orientáltak**.

***

# **2.1 Domain fogalmak**

## **Entity**
**Definíció:**
A domain alapegysége, egy valós fogalom modellje.

**Példa:**
`Need`, `Material`, `Mode`

**Anti‑pattern:**
- UI‑függő entitás
- Qt‑tól függő entitás

**Naming:**
PascalCase, főnév.

***

## **Value Object**
**Definíció:**
Értékalapú, azonosító nélküli domain‑típus.

**Példa:**
`Dimension`, `ColorCode`

**Anti‑pattern:**
- mutable value object
- value object, amely fájlt ír

**Naming:**
PascalCase.

***

## **Aggregate**
**Definíció:**
Összetett domain‑struktúra, amely több entitást fog össze.

**Példa:**
`NeedCalculation` → több `NeedCalculationDetail`

**Anti‑pattern:**
- túl nagy aggregate
- UI‑logika az aggregate‑ben

**Naming:**
PascalCase.

***

## **Rule**
**Definíció:**
A domain működését meghatározó szabály.

**Példa:**
`NeedRule`, `MaterialRule`

**Anti‑pattern:**
- rule, amely UI‑t módosít
- rule, amely fájlt ír

**Naming:**
`XRule`.

***

# **2.2 Architektúra fogalmak**

## **Repository**
**Definíció:**
Tartós tárolásért felelős IO‑komponens.

**Példa:**
`MaterialRepository`

**Anti‑pattern:**
- validáció a repository‑ban
- audit a repository‑ban

**Naming:**
`XRepository`.

***

## **Registry**
**Definíció:**
A domain állapotának auditálható, gyors elérésű tárolója.

**Példa:**
`ColorRegistry`

**Anti‑pattern:**
- fájlírás a registry‑ben
- workflow a registry‑ben

**Naming:**
`XRegistry`.

***

## **Manager**
**Definíció:**
A domain műveletek és workflow végrehajtója.

**Példa:**
`NeedCalculationManager`

**Anti‑pattern:**
- UI‑logika a managerben
- fájlírás a managerben

**Naming:**
`XManager`.

***

## **Presenter**
**Definíció:**
A UI és a domain közötti közvetítő réteg.

**Példa:**
`NeedsPresenter`

**Anti‑pattern:**
- domain‑adat tárolása
- registry közvetlen módosítása

**Naming:**
`XPresenter`.

***

## **View**
**Definíció:**
A tényleges UI‑komponens.

**Példa:**
`NeedsView`

**Anti‑pattern:**
- validáció
- domain‑logika

**Naming:**
`XView`.

***

# **2.3 UI fogalmak**

## **Inline validation**
**Definíció:**
Valós idejű mezőszintű érvényesítés.

**Példa:**
`ModeNameDialog` névmező

**Anti‑pattern:**
- popup minden hibára

**Naming:**
`validateInput()`.

***

## **Accent frame**
**Definíció:**
Vizuális fókuszjelölés.

**Példa:**
kék keret a fókuszmező körül

**Anti‑pattern:**
- túl erős animáció

**Naming:**
`applyAccentFrame()`.

***

## **Shake animation**
**Definíció:**
Rövid, non‑blocking hibajelzés.

**Példa:**
duplikált név → shake

**Anti‑pattern:**
- shake + popup együtt

**Naming:**
`shake()`.

***

## **Cursor‑based positioning**
**Definíció:**
Dialog pozicionálása a kurzorhoz igazítva.

**Példa:**
`ModeNameDialog`

**Anti‑pattern:**
- dialog a képernyő közepén

**Naming:**
`positionNearCursor()`.

***

## **Snapshot‑compatible UI**
**Definíció:**
UI, amely menthető, visszaállítható, guard‑olt és fallback‑képes.

**Példa:**
`MainWindow` snapshot

**Anti‑pattern:**
- restore a konstruktorban

**Naming:**
`restoreSnapshot()`.

***

# **2.4 Logolási fogalmak**
*A logolás a rendszer átláthatóságának, auditálhatóságának és stabilitásának alapja.*

A CutCalculator log‑architektúrája három elkülönített fogalmi csatornára épül:

- **DebugLog** – fejlesztői diagnosztika
- **SystemLog** – technikai és domain audit
- **EventLog** – felhasználói workflow események

A logszintekhez tartozó ikonok és kulcsszínek **nem dekorációk**, hanem kognitív‑ergonómiai eszközök, amelyek előemésztett információt hordoznak a felhasználó és a fejlesztő számára.

Az alábbi fogalmak a logolási rendszer alapját képezik.

***

## **DebugLog**
**Definíció:**
Fejlesztői diagnosztikai csatorna, amely TRACE és DEBUG szinteket tartalmaz.
Nem auditálható, nem kerül a végfelhasználói logokba.

**Szintek és viselkedés:**

- **TRACE** → vezérléskövetés („erre jártam” graffiti), nincs futási következmény
- **DEBUG** → mély diagnosztika, stack trace, fejlesztői break lehetősége

**Anti‑pattern:**
- DebugLog használata release buildben
- DebugLog keverése SystemLog vagy EventLog üzenetekkel

**Naming:**
`zTrace()`, `zDebug()`

***

## **SystemLog**
**Definíció:**
A rendszer technikai és domain műveleteinek auditálható csatornája.
INFO, WARNING és ERROR szinteket tartalmaz.

**Szintek és ikonok:**

- **INFO** → ℹ️ kék (semleges), ✔️ zöld (siker), ❌ piros (sikertelen, de nem kritikus)
- **WARNING** → ⚠️ sárga (óvatosság, fallback, degradált működés)
- **ERROR** → ❌ piros (kritikus hiba, programleállás)

**Viselkedési szerződés:**

- INFO → normál működés
- WARNING → tovább tudunk menni, de nem ideális
- ERROR → nem tudunk tovább menni, a rendszer megáll

**Anti‑pattern:**
- WARNING vagy ERROR EventLog‑ban
- audit esemény WARNING/ERROR szinten

**Naming:**
`zInfo()`, `zWarning()`, `zError()`

***

## **EventLog**
**Definíció:**
Felhasználói workflow események rövid, emberi nyelvű naplója.
Csak INFO szintet tartalmaz.

**Példák:**

- „MaterialPicker opened”
- „User clicked: Add Mode”
- „BOMWorkbench opened”

**Tiltások:**

- WARNING → ❌
- ERROR → ❌
- technikai részletek → ❌

**Naming:**
`zEventINFO()`

***

## **AuditEvent**
**Definíció:**
Domain művelet, amely auditálható formában kerül rögzítésre a SystemLog INFO szintjén.
Nem külön logcsatorna, hanem a SystemLog része.

**Példák:**

```
INFO Need created: id=42, name="Rugós"
INFO Material removed: id=12
INFO Mode renamed: old="Rugós", new="Rugós 2"
```

**Követelmények:**

- rövid, emberi nyelvű
- determinisztikus formátum
- ISO 8601 + UTC időbélyeg
- paraméterek stabil sorrendben

**Anti‑pattern:**
- audit esemény WARNING vagy ERROR szinten
- audit esemény EventLog‑ban
- audit esemény túl hosszú vagy technikai részletekkel

***

## **EventChannel**
**Definíció:**
A logolási rendszer három elkülönített fogalmi csatornája:

- **DebugLog** → fejlesztői diagnosztika
- **SystemLog** → technikai + audit
- **EventLog** → felhasználói események

**Anti‑pattern:**
- minden log egy csatornába
- user‑esemény SystemLog‑ba
- technikai hiba EventLog‑ba

***

## **Application Event / AppEvent**
**Definíció:**
Felhasználói művelet, amely az EventLog‑ba kerül.

**Példa:**
`EventLog::info("Mode created: %1", name);`

**Anti‑pattern:**
- Qt eventnek hívni
- technikai részleteket beleírni

***

## **Kulcsszín (Key Color)**
**Definíció:**
A logikonok színe, amely pre‑attentív módon hordoz jelentést a felhasználó és a fejlesztő számára.

**Kulcsszínek és jelentésük:**

- **kék ℹ️** → semleges információ
- **zöld ✔️** → sikeres művelet
- **piros ❌** → sikertelen művelet vagy kritikus hiba
- **sárga ⚠️** → figyelmeztetés, óvatosság

**Anti‑pattern:**
- ❌ ikon WARNING esetén
- ⚠️ ikon ERROR esetén
- ✔️ ikon sikertelen műveletnél
- ℹ️ ikon hibás műveletnél

***

## **Viselkedési szerződés (Behavior Contract)**
**Definíció:**
A logszinthez tartozó futási viselkedés, amely meghatározza, hogyan reagál a rendszer.

**Példák:**

- TRACE → nincs futási következmény
- DEBUG → stack trace, break
- INFO → normál működés
- WARNING → fallback, degradált működés
- ERROR → programleállás

**Anti‑pattern:**
- logszint és viselkedés összekeverése
- WARNING → programleállás
- ERROR → továbbfutás

***

# **3. Naming Convention Guide**
*A név nem dekoráció — hanem architektúra.*

A névadás a fejlesztői kultúra egyik legfontosabb eleme.
A jó név:

- egyértelmű
- konzisztens
- domain‑orientált
- kereshető
- auditálható
- hosszú távon stabil

Ez a fejezet rögzíti a projektfüggetlen, CutCalculator‑szellemű névadási szabályokat.

***

# **3.1 Általános naming elvek**

## **3.1.1 A név legyen beszédes, de nem túl hosszú**
A név mondja el, mi a szerepe az osztálynak vagy metódusnak.

**Jó:**
- `NeedCalculationManager`
- `MaterialPickerDialog`

**Rossz:**
- `Mgr`
- `Dlg`
- `ThingHandler`

***

## **3.1.2 A név tükrözze a domain‑szerepet**
A domain fogalmai legyenek a név alapjai.

**Jó:**
- `NeedRepository`
- `NeedRegistry`
- `NeedManager`

**Rossz:**
- `NeedService` (túl általános)
- `NeedController` (webes kontextus)

***

## **3.1.3 Egy fogalom → egy név**
Ha a domainben „Mode” a fogalom, akkor:

- `ModeRepository`
- `ModeRegistry`
- `ModeManager`
- `ModeNameDialog`

Nem:

- `CalculationTypeDialog`
- `ProfileManager`

***

## **3.1.4 Kerüld a rövidítéseket**
A rövidítések hosszú távon olvashatatlanok.

**Rossz:**
- `MatRepo`
- `CalcMgr`
- `Cfg`

***

## **3.1.5 A név legyen stabil**
A név a domain része — nem sprintenként változik.

***

# **3.2 Fájlnév konvenciók**

## **3.2.1 Egy osztály = egy fájl**
- `NeedCalculationManager.h`
- `NeedCalculationManager.cpp`

## **3.2.2 PascalCase fájlnevek**
A fájlnév = osztály neve.

## **3.2.3 Tesztfájlok**
- `NeedCalculationManagerTests.cpp`

## **3.2.4 Kerüld a többértelmű fájlneveket**
**Rossz:**
- `utils.cpp`
- `helpers.cpp`

***

# **3.3 Osztálynév konvenciók**

## **3.3.1 PascalCase**
- `NeedCalculationManager`
- `MaterialPickerDialog`

## **3.3.2 A név végződjön a szerepre**
A szerep legyen egyértelmű:

- `Repository`
- `Registry`
- `Manager`
- `Presenter`
- `View`
- `Dialog`
- `Picker`
- `Editor`
- `Preview`
- `Pipeline`
- `Step`

## **3.3.3 Kerüld a generikus neveket**
**Rossz:**
- `Handler`
- `Processor`
- `Controller`

***

# **3.4 Metódusnév konvenciók**

## **3.4.1 camelCase**
- `loadMaterials()`
- `insertMode()`
- `validateInput()`

## **3.4.2 A metódus neve legyen ige + tárgy**
**Jó:**
- `createNeed()`
- `renameMode()`
- `removeMaterial()`

**Rossz:**
- `needCreation()`
- `modeRename()`

***

## **3.4.3 Validációs metódusok**
- `isValid()`
- `validateInput()`
- `updateValidationState()`

***

## **3.4.4 UI metódusok**
- `updateTable()`
- `showError()`
- `applyAccentFrame()`
- `shake()`
- `positionNearCursor()`

***

# **3.5 Registry / Repository / Manager naming**

## **Repository**
- `XRepository`
- metódusok: `load()`, `save()`, `findById()`, `all()`

## **Registry**
- `XRegistry`
- metódusok: `insert()`, `remove()`, `rename()`, `exists()`

## **Manager**
- `XManager`
- metódusok: `create()`, `update()`, `delete()`, `validate()`

**CutCalculator példa:**
- `NeedCalculationManager`
- `ColorRegistry`
- `MaterialRepository`

***

# **3.6 UI komponensek naming**

## **View**
- `XView`
  - pl.: `NeedsView`

## **Presenter**
- `XPresenter`
  - pl.: `NeedsPresenter`

## **Dialog**
- `XDialog`
  - pl.: `ModeNameDialog`

## **Picker**
- `XPickerDialog`
  - pl.: `MaterialPickerDialog`

## **Editor**
- `XEditorDialog`
  - pl.: `FormulaEditorDialog`

## **Preview**
- `XPreview`

***

# **3.7 Pipeline elemek naming**

## **Pipeline**
- `XPipeline`
  - pl.: `MaterialPipeline`

## **Step**
- `XStep`
  - pl.: `FormulaStep`

***

# **3.8 Validációs komponensek naming**

## **Validator**
- `XValidator`
  - pl.: `BarcodeValidator`

## **State metódusok**
- `setErrorState()`
- `clearErrorState()`

***

# **3.9 Prompt DSL kompatibilis naming**

A promptokban használt neveket **ugyanazok a szabályok** vezérlik, mint a kódban.

## **3.9.1 Promptban használt osztálynevek**
- mindig PascalCase
- mindig domain‑orientált
- mindig szerepvégződéses

## **3.9.2 Promptban tiltott nevek**
- rövidítések
- generikus nevek
- domain‑idegen nevek

## **3.9.3 Prompt → kód konzisztencia**
A promptban szereplő név = a kódban szereplő név.

***

# **3.10 Anti‑pattern naming példák**

### ❌ Túl általános nevek
- `Manager`
- `Helper`
- `Utils`

### ❌ Rövidítések
- `Mgr`
- `Dlg`
- `Cfg`

### ❌ Domain‑idegen nevek
- `Controller`
- `Service`

### ❌ Túl hosszú nevek
- `NeedCalculationDetailMaterialPickerIntegrationManager`

### ❌ Többértelmű nevek
- `Processor`
- `Handler`

### ❌ „event” mint Qt‑event és alkalmazás‑esemény keverése
- `eventLog` → helyes (alkalmazás‑esemény)
- `eventHandler` → Qt‑esemény
- `handleEvent()` → Qt‑esemény
- `logEvent()` → alkalmazás‑esemény

**Tilos:**
- ugyanazt a nevet használni Qt eventre és alkalmazás‑eseményre
- `event` szóval jelölni user‑szintű eseményt (kivéve EventLog kontextusban)

***

# **4. Architektúra és rendszertervezés**

A modern desktop alkalmazások architektúrája akkor stabil, ha:

- a domain tiszta
- a felelősségek szétválasztottak
- a UI snapshot‑tudatos
- a logolás audit‑barát
- a workflow moduláris
- a naming konzisztens
- a rendszer determinisztikus

Ez a fejezet rögzíti a projektfüggetlen architektúra alapelveit, CutCalculator‑példákkal illusztrálva.

***

# **4.1 A CutCalculator architektúra mint referenciamodell**

A CutCalculator egy olyan architektúrát valósít meg, amely:

- **DDD‑szerű domain‑szétválasztást** használ
- **Registry / Repository / Manager triádra** épül
- **Presenter / View** UI‑mintát alkalmaz
- **snapshot‑tudatos UI‑t** használ
- **audit‑barát logolást** valósít meg
- **moduláris pipeline‑okat** használ
- **prompt‑vezérelt fejlesztési workflow‑t** követ

Ez a fejezet ezt a modellt általánosítja és projektfüggetlen mintává emeli.

***

# **4.2 Domain‑szétválasztás mint ipari best practice**

A domain három rétegre oszlik:

## **4.2.1 Konfigurációs domain**
A felhasználó által megadott paraméterek.
**Példa:** `NeedCalculation`, `Mode`, `Material`

## **4.2.2 Számítási domain**
A tényleges számítási folyamat.
**Példa:** `NeedCalculationDetail`, `FormulaStep`

## **4.2.3 Meta‑domain**
A domain működését támogató struktúrák.
**Példa:** `Rule`, `Pipeline`, `Snapshot`

### **Miért fontos ez a szétválasztás?**

- tiszta felelősségek
- jobb tesztelhetőség
- könnyebb refaktorálás
- stabilabb architektúra
- domain‑vezérelt gondolkodás támogatása

***

# **4.3 Registry / Repository / Manager triád**

A CutCalculator egyik legerősebb mintája — és teljesen projektfüggetlen.

***

## **4.3.1 Repository — tartós tárolás (IO)**

**Feladata:**

- betöltés
- mentés
- keresés
- listázás

**Nem tartalmazhat:**

- validációt
- auditot
- workflow‑t
- UI‑logikát

**Példa:** `MaterialRepository`, `NeedRepository`

***

## **4.3.2 Registry — auditálható állapot**

**Feladata:**

- cache
- gyors elérés
- ütközéskezelés
- audit
- érvényesség

**Nem tartalmazhat:**

- fájlírást
- workflow‑t
- UI‑logikát

**Példa:** `ColorRegistry`, `NeedRuleRegistry`

***

## **4.3.3 Manager — műveletek és workflow**

**Feladata:**

- validáció
- tranzakciók
- domain‑műveletek
- workflow
- audit

**Nem tartalmazhat:**

- UI‑logikát
- fájlírást
- állapotot (csak műveletet)

**Példa:** `NeedCalculationManager`, `DetailManager`

***

# **4.4 Snapshot‑architektúra**

A snapshot‑tudatos UI ipari követelmény.
A CutCalculator ezt példaszerűen valósítja meg.

A snapshot‑architektúra négy komponensből áll:

***

## **4.4.1 SnapshotManager**
**Feladata:**

- UI állapot mentése
- UI állapot visszaállítása
- több monitorprofil kezelése
- verziózás

**Követelmény:**
- deferred restore
- guard minden restore előtt

***

## **4.4.2 LayoutDefaultStore**
**Feladata:**
Alapértelmezett UI elrendezés tárolása, fallback baseline biztosítása.

**Miért fontos?**
A snapshot hibája nem okozhat szétesett UI‑t.

***

## **4.4.3 GeometryGuard**
**Feladata:**

- hibás snapshot felismerése
- off‑screen ablakok kezelése
- túl kicsi méretek javítása
- DPI‑váltás kezelése

Ez UX, nem technikai extra.

***

## **4.4.4 ScreenChangeDetector**
**Feladata:**

- monitorváltás felismerése
- snapshot invalidálása
- fallback aktiválása

***

# **4.5 UI‑architektúra**

A CutCalculator UI‑ja a **Presenter / View** mintára épül.

***

## **4.5.1 Presenter**
**Feladata:**

- UI események kezelése
- domain műveletek meghívása
- View frissítése

**Nem tartalmazhat:**

- domain‑adatot
- fájlírást
- logolást

***

## **4.5.2 View**
**Feladata:**

- megjelenítés
- események továbbítása a Presenternek

**Nem tartalmazhat:**

- validációt
- domain‑logikát

***

## **4.5.3 Dialog**
**Feladata:**

- rövid életű műveletek
- inline validation
- cursor‑based positioning

***

## **4.5.4 Picker**
**Feladata:**
Domain‑entitás kiválasztása.

***

## **4.5.5 Editor**
**Feladata:**
Domain‑entitás szerkesztése, preview támogatása.

***

# 📘 **4.6 Logolási architektúra**
*A logolás nem mellékes technikai részlet — hanem a rendszer stabilitásának, auditálhatóságának és visszakövethetőségének alapja.*

A CutCalculator logolási architektúrája három, egymástól élesen elkülönülő rétegre épül:

1. **DebugLog** – fejlesztői diagnosztika (TRACE, DEBUG)
2. **SystemLog** – technikai és domain audit (INFO, WARNING, ERROR)
3. **EventLog** – felhasználói workflow események (INFO)

A három réteg célja, életciklusa és következményei eltérőek.
A rendszer stabilitása érdekében **nem keverhetők**, és minden loghívásnak egyértelműen beazonosítható szerepe van.

***

## **4.6.1 A logolás szerepe az architektúrában**
*(meglévő rész marad — nem ismétlem)*

***

## **4.6.2 A Logger háromrétegű modellje**

A Logger három különböző célt szolgáló logréteget valósít meg:

- **DebugLog** → fejlesztői diagnosztika
- **SystemLog** → technikai + domain audit
- **EventLog** → felhasználói események

Ez a szétválasztás biztosítja:

- a rendszer stabilitását
- a logok érthetőségét
- az auditálhatóságot
- a hibák gyors visszakövethetőségét
- a fejlesztői diagnosztika elkülönítését

***

## **4.6.3 DebugLog szerepe (TRACE, DEBUG)**
*A DebugLog a fejlesztő eszköze — nem része a rendszer működésének.*

A DebugLog kizárólag fejlesztői diagnosztikai célokat szolgál.
Nem auditálható, nem kerül be a végfelhasználói logokba, és nem része a rendszer működésének.

### **TRACE — „itt jár a kód”**
- minimális információ
- stack trace nélkül
- vezérléskövetés
- csak `QT_DEBUG` alatt aktív
- nem állítja meg a futást

### **DEBUG — „mély hibakeresés”**
- részletes diagnosztika
- stack trace
- Linuxon `_isBreakOnError == true` esetén **megállítja a futást**
- fejlesztői breakpointként működik

### **Mikor használjuk?**
- vezérlés útjának követése
- ritka vagy időzítésfüggő hibák
- mély diagnosztika

### **Mikor nem használjuk?**
- domain műveleteknél
- UI workflow eseményeknél
- audit eseményeknél
- release buildben

***

## **4.6.4 SystemLog szerepe (INFO, WARNING, ERROR)**
*A SystemLog a rendszer belső működésének auditálható rétege.*

A SystemLog minden technikai és domain műveletet rögzít.

### **INFO — „normál működés”**
- domain műveletek
- repository/registry interakciók
- snapshot restore
- audit események

### **WARNING — „hiba történt, de tovább tudunk menni”**
- fallback
- részleges siker
- invalid input
- degradált működés

**Ökölszabály:**
> Ha a program tovább tud menni → WARNING.

### **ERROR — „hiba történt, és nem tudunk tovább menni”**
- kritikus hiba
- nincs fallback
- stack trace
- exit() / abort()

**Ökölszabály:**
> Ha a program nem tud tovább menni → ERROR.

***

## **4.6.5 EventLog szerepe (UI workflow)**
*A felhasználói események logja — rövid, emberi nyelvű, non‑blocking.*

Az EventLog a felhasználó által kiváltott UI‑eseményeket rögzíti:

- dialógus megnyitása
- gombnyomás
- drag & drop
- BOMWorkbench megnyitása
- user workflow lépések

**Tiltás:**
- EventLog WARNING → ❌
- EventLog ERROR → ❌

A user‑szintű hibák UI‑visszajelzéssel kezelendők, nem logszinten.

***

## **4.6.6 Időbélyegzés és ipari szabványok**
*A determinisztikus időbélyeg az audit alapja.*

A logok időbélyege **ISO 8601** formátumú, **UTC** időzónában:

```
YYYY-MM-DDTHH:MM:SS.sssZ
```

### **Miért ISO 8601?**
- nemzetközi szabvány
- rendezhető
- nyelvfüggetlen
- audit‑kompatibilis

### **Miért UTC?**
- nincs DST probléma
- több gép között konzisztens
- auditálásnál egyértelmű

### **Követelmények**
- minden log időbélyeges
- determinisztikus formátum
- locale‑független

***

## **4.6.7 Audit‑barát logolás**
*A domain műveletek visszakövethetősége a rendszer stabilitásának alapja.*

A CutCalculator audit‑fogalma **domain audit**:

> A domain műveletek visszakövethetősége rövid, érthető, determinisztikus logbejegyzésekkel.

### **Audit céljai**
- visszakövethetőség
- reprodukálhatóság
- stabilitás
- bizalom

### **Audit eszközei**
1. SystemLog INFO szint
2. rövid, emberi nyelvű üzenetek
3. ISO 8601 időbélyeg

### **Audit megvalósítása**
- domain művelet → Manager
- audit esemény → SystemLog (INFO)
- Logger → időbélyeg + routing

### **Miért nem külön AuditLog?**
- felesleges duplikáció
- SystemLog már audit‑barát
- egyszerűbb, stabilabb architektúra

### **Audit esemény formátuma**
```
<timestamp> INFO <entity> <action>: <parameters>
```

---

## **4.6.7.1 Audit és logolás kapcsolata az ipari szabványok szerint**
*(ÚJ alfejezet — a Gemini‑alapú kiegészítés beépítve)*

A nemzetközi szabványok (ISO/IEC 27001, ISO/IEC 27002, ISO 15489) szerint a logolás és az auditálhatóság a modern rendszerek alapkövetelménye.
Ezek a szabványok nem írnak elő konkrét logformátumot, de meghatározzák, hogy a logoknak:

- **visszakövethetőnek**,
- **érthetőnek**,
- **időben rekonstruálhatónak**,
- **integritás‑védettnek**,
- **bizonyítékként felhasználhatónak**

kell lenniük.

Ezért használunk:

- ISO 8601 időbélyeget,
- UTC időzónát,
- determinisztikus formátumot,
- rövid, emberi nyelvű audit eseményeket.

A CutCalculator audit‑filozófiája teljes összhangban van ezekkel az ipari elvekkel:
**a log nem csak napló, hanem a rendszer memóriája és bizonyítéka.**

***

## **4.6.8 Logger API szereptérkép**

| Makró | Szint | Csatorna | Szerep |
|------|-------|----------|--------|
| `zTrace()` | TRACE | DebugLog | vezérléskövetés |
| `zDebug()` | DEBUG | DebugLog | mély diagnosztika |
| `zInfo()` | INFO | SystemLog | domain/technikai esemény |
| `zWarning()` | WARNING | SystemLog | nem kritikus hiba |
| `zError()` | ERROR | SystemLog | kritikus hiba |
| `zEventINFO()` | INFO | EventLog | user workflow esemény |

***

## **4.6.9 Logolási döntési fa**

```
Hiba történt?
    |
    ├── Nem → INFO (SystemLog vagy EventLog)
    |
    └── Igen →
           |
           ├── Tudunk tovább menni? → WARNING (SystemLog)
           |
           └── Nem tudunk → ERROR (SystemLog)
```

***
# **4.6.10 A logikonok színének ergonomiai szerepe**
*A szín nem dekoráció — hanem kognitív információhordozó.*

A logszintekhez tartozó ikonok **nem pusztán vizuális díszítőelemek**, hanem a szoftverergonómia egyik legfontosabb eszközei.
A szín — mint elsődleges vizuális inger — az emberi agyban **pre‑attentív feldolgozással** működik:
az információt **még azelőtt értelmezzük**, hogy tudatosan elolvasnánk a szöveget.

Ez azt jelenti, hogy a logikonok színe:

- **előemésztett információt** hordoz
- **azonnali jelentést** közvetít
- **csökkenti a kognitív terhelést**
- **gyorsítja a hibák felismerését**
- **segíti a döntéshozatalt**
- **egységes vizuális nyelvet teremt**

A CutCalculator logikonjai ezért **színkódolt jelentést** hordoznak — de fontos megérteni:

> **Nincs külön SUCCESS logszint.
> Az INFO szinten belül a jelentés határozza meg az ikon kulcsszínét.**

***

# **INFO — kék ℹ️ (semleges tájékoztatás)**
A kék szín a kognitív pszichológiában:

- semlegességet
- megbízhatóságot
- nyugalmat
- információt

jelöl.

A kék ℹ️ ikon azt üzeni:

> „Ez egy normál, nem sürgős, nem problémás rendszerüzenet.”

**Mikor használjuk?**

- normál működés
- állapotváltozás
- audit esemény
- technikai információ

**Kognitív szerepe:**
a kék szín *nem igényel azonnali figyelmet*, így nem terheli túl a felhasználót.

***

# **INFO (sikeres művelet) — zöld ✔️ (pozitív visszaigazolás)**
A zöld szín az emberi idegrendszerben:

- sikert
- jóváhagyást
- pozitív eredményt
- lezárt, rendben lévő állapotot

jelent.

A zöld ✔️ ikon azt üzeni:

> „A művelet sikeresen befejeződött.”

**Fontos:**
Ez *nem külön logszint*, hanem **INFO szintű üzenet**, amely *pozitív tartalmú*, ezért kap zöld kulcsszínt.

**Mikor használjuk?**

- mentés sikeres
- import sikeres
- számítás sikeres
- workflow lépés sikeresen lezárult

**Kognitív szerepe:**
a zöld ✔️ ikon **megnyugtat**, lezárja a műveletet, csökkenti a bizonytalanságot.

***

# **INFO (sikertelen művelet) — piros ❌ (negatív eredmény)**
Ha egy INFO szintű üzenet **nem hiba**, de a művelet *nem sikerült*, akkor a kulcsszín piros ❌.

Ez akkor fordul elő, ha:

- a művelet nem kritikus
- a rendszer tovább tud menni
- de a művelet eredménye negatív

**Példa:**

- „✔️ Export completed”
- „❌ Export failed (file locked)”

Mindkettő INFO szintű, de a jelentés eltér.

**Kognitív szerepe:**
a piros ❌ ikon **azonnal jelzi**, hogy a művelet eredménye negatív, még akkor is, ha a rendszer nem hibásodott meg.

***

# **WARNING — sárga ⚠️ (óvatosság)**
A sárga szín:

- óvatosság
- potenciális probléma
- figyelmeztetés

jelzésére szolgál.

A sárga ⚠️ ikon:

- azonnal felhívja a figyelmet
- nem agresszív
- nem végzetes

Ez a vizuális jelzés azt üzeni:

> „Valami nem ideális, de tovább tudunk menni.”

***

# **ERROR — piros ❌ (kritikus hiba)**
A piros szín:

- veszély
- tiltás
- végzetes állapot
- azonnali cselekvési szükséglet

jelzésére szolgál.

A piros ❌ ikon:

- erős, egyértelmű jelzés
- nem keverhető össze a WARNING‑gal
- azonnal kiemelkedik a vizuális térből

Ez összhangban van a CutCalculator ERROR‑viselkedésével:

- stack trace
- programleállás
- kritikus állapot

**Kognitív szerepe:**
a piros ❌ ikon *stop‑jelzés*, amelyet az agy automatikusan veszélynek értelmez.

***

# **Összefoglaló — ikonok és jelentésük INFO szinten belül**

| Jelentés | Ikon | Szín | Logszint |
|*********|******|******|*********-|
| Semleges információ | ℹ️ | kék | INFO |
| Sikeres művelet | ✔️ | zöld | INFO |
| Sikertelen művelet (nem kritikus) | ❌ | piros | INFO |
| Figyelmeztetés | ⚠️ | sárga | WARNING |
| Kritikus hiba | ❌ | piros | ERROR |

***

# **4.7 Validációs architektúra**

A validáció három szinten történik:

***

## **4.7.1 Domain validáció**
- érvényes értékek
- duplikációk
- üzleti szabályok

***

## **4.7.2 UI validáció (inline)**
- azonnali visszajelzés
- accent frame
- shake animation

***

## **4.7.3 Workflow validáció**
- műveletek sorrendje
- tranzakciók
- hibakezelés

***

# **5. Workflow és fejlesztési ritmus**
*A fejlesztés nem csak kódolás — hanem ritmus, szokás, gondolkodásmód és audit.*

A CutCalculator alatt kialakult workflow olyan mintákat hozott létre, amelyek projektfüggetlenül is alkalmazhatók.
A cél: **a fejlesztői gondolkodás egységesítése és a projekt hosszú távú fenntarthatósága.**

A workflow hat nagy pillérre épül:

- modularitás
- auditálhatóság
- snapshot‑tudatosság
- domain‑vezérelt gondolkodás
- konzisztens naming
- folyamatos refaktorálás
- napzáró rituálé
- prompt‑vezérelt együttműködés

***

# **5.1 A fejlesztői workflow alapelvei**

A modern fejlesztés alapja:

- **modularitás** — minden modul külön egység
- **auditálhatóság** — minden művelet visszakövethető
- **snapshot‑tudatosság** — a UI stabil marad
- **domain‑vezérelt gondolkodás** — a fogalmak tiszták
- **konzisztens naming** — a kód kereshető
- **folyamatos refaktorálás** — a rendszer tiszta marad
- **napzáró rituálé** — fókusz és rend
- **prompt‑vezérelt fejlesztés** — gyorsítás és konzisztencia

A workflow célja:
**a fejlesztői gondolkodás egységesítése és a projekt hosszú távú fenntarthatósága.**

***

# **5.2 Prompt‑vezérelt fejlesztés**

A CutCalculator fejlesztése során kialakult egy új, modern workflow:

## **5.2.1 A fejlesztő és az AI közös ritmusa**
A fejlesztő:

- definiálja a problémát
- meghatározza a kontextust
- kijelöli a moduláris határokat
- ellenőrzi a generált kódot

Az AI:

- generálja a boilerplate‑et
- segít a namingben
- segít a dokumentációban
- segít a refaktorálásban
- segít a sablonokban

## **5.2.2 A prompt mint specifikáció**
A prompt:

- nem kérés
- nem utasítás
- hanem **műszaki specifikáció**

A jó prompt:

- kontextust ad
- határokat ad
- naminget ad
- példát ad
- anti‑patternöket tilt
- workflow‑t definiál

## **5.2.3 A prompt mint dokumentáció**
A promptokból:

- DSL
- sablonok
- workflow‑k
- dokumentáció
- naming szabályok

születnek.

***

# **5.3 Moduláris fejlesztési ciklus**

A fejlesztés moduláris egységekre oszlik:

## **5.3.1 1. Tervezés**
- domain meghatározása
- naming kiválasztása
- workflow kijelölése
- UI‑vázlat
- snapshot‑követelmények

## **5.3.2 2. Skeleton**
- osztályok létrehozása
- interface‑ek
- alap metódusok
- logolási csatornák előkészítése

## **5.3.3 3. Implementáció**
- domain logika
- validáció
- workflow
- snapshot integráció
- UI/UX minták alkalmazása

## **5.3.4 4. Integráció**
- Presenter ↔ Manager
- Manager ↔ Registry
- Registry ↔ Repository
- UI ↔ SnapshotManager

## **5.3.5 5. Tesztelés**
- unit tesztek
- UI tesztek
- snapshot tesztek
- audit tesztek

## **5.3.6 6. Refaktorálás**
- naming tisztítás
- moduláris bontás
- anti‑pattern eltávolítás
- UI/UX finomhangolás

## **5.3.7 7. Dokumentálás**
- napzáró
- állapottábla
- roadmap frissítés
- prompt‑archívum frissítése

***

# **5.4 Snapshot‑tudatos UI fejlesztés**

A snapshot‑kompatibilis UI fejlesztése külön workflow‑t igényel:

## **5.4.1 Snapshot‑aware komponensek**
Minden UI‑elem:

- menthető
- visszaállítható
- guard‑olt
- fallback‑képes
- DPI‑független

## **5.4.2 Deferred restore**
A restore csak akkor történik, amikor:

- a widget már látható
- a mérete ismert
- a layout stabil
- a monitorprofil betöltődött

## **5.4.3 Fallback baseline**
Ha a snapshot hibás:

- LayoutDefaultStore → baseline
- GeometryGuard → guard
- ScreenChangeDetector → monitorváltás kezelése

## **5.4.4 Snapshot verziózás**
A snapshot formátuma változhat — legyen verziószám.

***

# **5.5 Audit‑barát fejlesztés**

A fejlesztés minden lépése auditálható:

## **5.5.1 Audit log**
- domain műveletek
- rename
- insert
- delete

## **5.5.2 SystemLog**
- technikai részletek
- CSV import
- registry audit

## **5.5.3 EventLog**
- user‑szintű események

## **5.5.4 Verbose routing**
- logok szétválasztása
- zaj csökkentése

***

# **5.6 Refaktorálási protokoll**

A refaktorálás nem opcionális — hanem **folyamatos folyamat**.

## **5.6.1 Mikor kell refaktorálni?**
- ha a naming billeg
- ha a domain keveredik
- ha a UI túl sok felelősséget kap
- ha a logolás zajos
- ha a snapshot instabil
- ha a pipeline túl nagy
- ha a Presenter túl sokat tud

## **5.6.2 Refaktorálási lépések**
1. domain tisztítás
2. naming tisztítás
3. moduláris bontás
4. UI tisztítás
5. logolás tisztítása
6. snapshot‑guard erősítése

## **5.6.3 Refaktorálási anti‑pattern**
- „majd később megcsináljuk”
- „jó ez így is”
- „nem akarom szétszedni”
- „ez most nem fontos”

***

# **5.7 Napzáró és állapottábla mint rituálé**

A CutCalculator fejlesztése során kialakult egy nagyon erős rituálé:

## **5.7.1 Napzáró dokumentum**
Tartalmazza:

- napi eredmények
- taktikai összefoglaló
- stratégiai összefoglaló
- állapottábla
- roadmap
- prioritási lista
- holnapi fókusz

## **5.7.2 Miért fontos?**
- lezárja a napot
- auditálható
- visszakereshető
- ritmust ad
- fókuszt ad
- motivációt ad

## **5.7.3 A napzáró mint fejlesztői kultúra**
Ez nem adminisztráció — hanem:

- fókusz
- tisztaság
- rend
- ritmus
- önreflexió

A napzáró a fejlesztői kultúra egyik legerősebb eszköze.

***

# **6. Best Practice gyűjtemény**

A best practice nem szabály — hanem **tapasztalatból leszűrt, bizonyítottan működő mintázat**.
Ez a fejezet összegyűjti azokat az elveket, amelyek a CutCalculator fejlesztése során bizonyítottak, és projektfüggetlenül is alkalmazhatók.

A fejezet hét nagy területre oszlik:

- Architektúra
- Naming
- UI/UX
- Logolás
- Validáció
- Snapshot‑kezelés
- Prompt‑vezérelt fejlesztés

***

# **6.1 Architektúra best practice**

## **6.1.1 Egy felelősség elve (SRP)**
Minden osztály egyetlen dolgot csináljon — de azt jól.

**Jó:**
- `NeedRepository` → csak IO
- `NeedRegistry` → csak állapot
- `NeedManager` → csak műveletek

**Rossz:**
- `NeedService` → minden benne van

***

## **6.1.2 A domain legyen tiszta**
A domain:

- ne függjön UI‑tól
- ne függjön Qt‑tól
- ne írjon fájlt
- ne logoljon

A domain legyen *független és tesztelhető*.

***

## **6.1.3 A UI legyen vékony**
A View:

- ne validáljon
- ne számoljon
- ne módosítson domain‑adatot

A View csak:

- megjelenít
- eseményt küld

***

## **6.1.4 A Presenter legyen a UI agya**
A Presenter:

- fogadja a UI eseményeket
- meghívja a Managert
- frissíti a View‑t

De:

- nem tartalmaz domain‑adatot
- nem ír fájlt
- nem logol közvetlenül

***

## **6.1.5 A Manager legyen a domain agya**
A Manager:

- validál
- workflow‑t kezel
- domain műveleteket végez

De:

- nem jelenít meg UI‑t
- nem ír fájlt
- nem tart állapotot

***

## **6.1.6 A Registry legyen auditálható állapot**
A Registry:

- cache
- ütközéskezelés
- audit
- érvényesség

De:

- nem ment fájlt
- nem validál UI‑t
- nem tart workflow‑t

***

## **6.1.7 A Repository legyen tiszta IO**
A Repository:

- betölt
- ment
- keres

De:

- nem validál
- nem auditál
- nem workflow‑zik

***

# **6.2 Naming best practice**

## **6.2.1 A név legyen domain‑orientált**
**Jó:**
- `NeedCalculationManager`
- `MaterialPickerDialog`

**Rossz:**
- `ThingHandler`
- `DataProcessor`

***

## **6.2.2 Kerüld a rövidítéseket**
- `Mgr`
- `Dlg`
- `Cfg`

Ezek hosszú távon olvashatatlanok.

***

## **6.2.3 Egy fogalom → egy név**
Ha a domainben „Mode” a fogalom, akkor:

- `ModeRepository`
- `ModeRegistry`
- `ModeManager`
- `ModeNameDialog`

Nem:

- `CalculationTypeDialog`
- `ProfileManager`

***

## **6.2.4 A név legyen stabil**
A név a domain része — nem sprintenként változik.

***

# **6.3 UI/UX best practice**

## **6.3.1 Inline validation mindenhol**
A felhasználó gépelés közben kapjon visszajelzést.

***

## **6.3.2 Accent frame a fókusz jelölésére**
A felhasználó mindig tudja, hol van.

***

## **6.3.3 Shake animation hibára**
Non‑blocking, egyértelmű, modern.

***

## **6.3.4 Cursor‑based positioning**
A dialog a kurzorhoz igazodjon — nem a képernyő közepére.

***

## **6.3.5 Snapshot‑compatible UI**
Minden UI‑elem:

- menthető
- visszaállítható
- guard‑olt
- fallback‑képes

***

## **6.3.6 Kerüld a fix pixeleket**
Használj:

- percent‑based layout
- layout stretch
- minimumSizeHint

***

## **6.3.7 A vizuális ritmus legyen következetes**
- spacing 8–12 px
- margin 12–16 px
- konzisztens ikonhasználat
- konzisztens gombelrendezés

***

# **6.4 Logolási best practice (ikonokkal és kulcsszínekkel)**
*A logolás célja a rendszer működésének átlátható, auditálható és ergonomikus visszajelzése.*

A logolás nem pusztán technikai részlet — hanem a rendszer stabilitásának, auditálhatóságának és UX‑minőségének alapja.
A CutCalculator logolási best practice‑ei a háromrétegű log‑architektúrára, a viselkedési szerződésekre és a színkódolt ikonokra épülnek.

***

## **6.4.1 Háromrétegű logolás**

A logok három elkülönített csatornára kerülnek:

- **DebugLog** → fejlesztői diagnosztika (TRACE, DEBUG)
- **SystemLog** → technikai + domain audit (INFO, WARNING, ERROR)
- **EventLog** → felhasználói workflow események (INFO)

**Best practice:**

- DebugLog csak fejlesztés alatt használható.
- SystemLog minden domain és technikai műveletet rögzít.
- EventLog csak user‑szintű eseményeket tartalmazhat.
- A három csatorna nem keverhető.

***

## **6.4.2 Verbose routing**

A verbose mód részletes technikai információkat ad:

- fájlnév
- sorszám
- metódusnév
- stack trace (DEBUG esetén)

**Best practice:**

- verbose mód csak fejlesztés alatt legyen aktív
- release buildben verbose = false
- verbose információ soha ne kerüljön EventLog‑ba

***

## **6.4.3 Minden log legyen időbélyeges**

A logok időbélyege **ISO 8601** formátumú, **UTC** időzónában.

**Miért?**

- audit követelmény
- rendezhető
- nyelvfüggetlen
- determinisztikus
- több gép között konzisztens

**Best practice:**

- minden logbejegyzés kötelezően időbélyeges
- a timestamp formátuma nem lehet locale‑függő
- a rendszer órája legyen szinkronizált

***

## **6.4.4 A log legyen rövid és érthető**

A log célja nem a részletek túlmagyarázása, hanem a visszakövethetőség.

**Jó:**
- „Material added: RAL 9010”
- „Mode renamed: old=Rugós, new=Rugós 2”

**Rossz:**
- „Material operation completed successfully with code 0x00000001”
- „An unexpected condition occurred during the execution of the rename operation”

**Best practice:**

- rövid, emberi nyelvű üzenetek
- determinisztikus formátum
- paraméterek mindig ugyanabban a sorrendben

***

## **6.4.5 Ikonok és kulcsszínek használata INFO szinten**

Az INFO szint **háromféle ikonvariánst** használhat, a jelentéstől függően:

### **ℹ️ Kék INFO — semleges tájékoztatás**
Használjuk, ha:

- normál működés
- állapotváltozás
- audit esemény
- technikai információ

### **✔️ Zöld pipa — sikeres művelet**
Használjuk, ha:

- a művelet sikeresen befejeződött
- mentés / import / export / számítás sikeres
- workflow lépés pozitív eredménnyel zárult

### **❌ Piros X — sikertelen művelet (nem kritikus)**
Használjuk, ha:

- a művelet nem sikerült
- de a rendszer tovább tud menni
- nem kritikus hiba történt

**Best practice:**

- INFO ikonja mindig a jelentést tükrözi
- a kulcsszín pre‑attentív információt hordoz
- a felhasználó szövegolvasás nélkül is érti az üzenetet

***

## **6.4.6 WARNING ikonhasználat — ⚠️ sárga figyelmeztetés**

A WARNING szint ikonja **mindig**:

```
⚠️
```

**Best practice:**

- WARNING esetén kizárólag ⚠️ ikon használható
- ❌ ikon WARNING esetén tilos
- WARNING = óvatosság, fallback, degradált működés
- WARNING nem blokkolja a felhasználót

***

## **6.4.7 ERROR ikonhasználat — ❌ piros kritikus hiba**

Az ERROR szint ikonja **mindig**:

```
❌
```

**Best practice:**

- ERROR = kritikus hiba, a rendszer nem tud tovább menni
- ERROR esetén a program leáll (break / abort)
- ERROR ikonja nem keverhető össze WARNING‑gal
- ERROR ikonja nem használható INFO vagy WARNING szinten

***

## **6.4.8 Logok védelme és integritása**

A logok integritása kritikus az auditálhatóság szempontjából.

**Best practice:**

- logfájlok csak append‑módban írhatók
- logfájlok nem módosíthatók utólag
- logfájlok nem törölhetők futás közben
- logfájlokhoz csak a rendszer férhet hozzá

Ez összhangban van az ipari szabványokkal (ISO/IEC 27001 A.8.15).

***

## **6.4.9 Audit események (SystemLog INFO)**

Az audit események a domain műveletek visszakövethetőségét szolgálják.

**Best practice:**

- audit esemény mindig INFO szinten
- audit esemény mindig SystemLog‑ba kerül
- audit esemény rövid és determinisztikus
- audit esemény nem popup
- audit esemény nem WARNING vagy ERROR

**Példa:**

```
ℹ️ Need created: id=42, name="Rugós"
```

***

# **6.5 Validációs best practice**

## **6.5.1 Domain validáció legyen első**
A domain szabályai a legfontosabbak.

***

## **6.5.2 UI validáció legyen azonnali**
Inline validation.

***

## **6.5.3 Workflow validáció legyen tranzakciós**
Ha egy lépés hibás → rollback.

***

## **6.5.4 Ne legyen duplikált validáció**
A domain validál → a UI csak megjelenít.

***

# **6.6 Snapshot‑kezelési best practice**

## **6.6.1 Deferred restore**
Restore csak akkor, amikor a UI már stabil.

***

## **6.6.2 Geometry guard**
Minden snapshot restore előtt:

- méret ellenőrzés
- pozíció ellenőrzés
- monitor ellenőrzés
- DPI ellenőrzés

***

## **6.6.3 Fallback baseline**
Ha a snapshot hibás → LayoutDefaultStore.

***

## **6.6.4 Snapshot verziózás**
A snapshot formátuma változhat — legyen verziószám.

***

# **6.7 Prompt‑vezérelt fejlesztési best practice**

## **6.7.1 A prompt legyen specifikáció**
Ne kérés — hanem:

- kontextus
- határok
- naming
- példák
- anti‑patternök

***

## **6.7.2 A prompt legyen moduláris**
Egy prompt = egy modul.

***

## **6.7.3 A prompt legyen auditálható**
A promptokból:

- DSL
- sablonok
- workflow‑k
- dokumentáció

születnek.

***

## **6.7.4 A prompt legyen iteratív**
A jó prompt:

- pontosít
- tisztít
- épít
- refaktorál

***
# 📘 **7. fejezet — Anti‑pattern gyűjtemény**
*A rossz minták felismerése a fejlesztői kultúra egyik legfontosabb képessége.*

Ez a fejezet már teljesen egységesített, professzionális, duplikációktól mentes formában áll előtted.
A cél: **láthatóvá tenni azokat a hibákat, amelyek hosszú távon rombolják a kódminőséget, az architektúrát, a UI‑t és a fejlesztői kultúrát.**

***

# **7. Anti‑pattern gyűjtemény**
*A rossz minták nem véletlenek — ismétlődő gondolkodási hibák.*

Az anti‑pattern nem egyszerűen „rossz megoldás”, hanem olyan **ismétlődő hiba**, amely:

- technikai adósságot okoz
- karbantarthatatlanná teszi a kódot
- instabillá teszi a UI‑t
- zajossá teszi a logolást
- összekeveri a domaineket
- rontja a fejlesztői kultúrát

Ez a fejezet összegyűjti a legfontosabb anti‑patternöket, CutCalculator‑példákkal illusztrálva.

***

# **7.1 Architektúra anti‑patternök**

## **7.1.1 God Object**
**Definíció:** Egy osztály, amely túl sok felelősséget kap.
**Rossz példa:** `NeedService` → repository + registry + manager + UI logika egyben
**Miért rossz?** tesztelhetetlen, átláthatatlan, instabil
**Megoldás:** Registry / Repository / Manager triád

***

## **7.1.2 UI‑függő domain**
**Rossz:** domain osztály, amely `QWidget`‑et tartalmaz
**Miért rossz?** domain → UI‑hoz kötődik, tesztelhetetlen
**Megoldás:** domain legyen tiszta, UI‑független

***

## **7.1.3 Repository, amely validál**
**Rossz:** `MaterialRepository::insert()` → „name already exists”
**Miért rossz?** validáció domain‑szintű
**Megoldás:** validáció → Manager

***

## **7.1.4 Registry, amely fájlt ír**
**Rossz:** `NeedRegistry::saveToFile()`
**Miért rossz?** Registry állapotot tart, nem IO‑t végez
**Megoldás:** IO → Repository

***

## **7.1.5 Manager, amely UI‑t módosít**
**Rossz:** `NeedManager::showErrorDialog()`
**Miért rossz?** Manager workflow, nem UI
**Megoldás:** UI → Presenter / View

***

# **7.2 Naming anti‑patternök**

## **7.2.1 Rövidítések**
**Rossz:** `Mgr`, `Dlg`, `Cfg`
**Miért rossz?** olvashatatlan, kereshetetlen, domain‑idegen

***

## **7.2.2 Generikus nevek**
**Rossz:** `Handler`, `Processor`, `Controller`
**Miért rossz?** nem derül ki a szerep

***

## **7.2.3 Többértelmű nevek**
**Rossz:** `ThingManager`, `DataService`
**Miért rossz?** nem kommunikál semmit

***

## **7.2.4 Domain‑idegen nevek**
**Rossz:** `ProfileManager` (ha a domainben „Mode” a fogalom)
**Megoldás:** egy fogalom → egy név

***

# **7.3 UI anti‑patternök**

## **7.3.1 Popup minden hibára**
**Miért rossz?** blokkol, megszakítja a munkát
**Megoldás:** inline validation + accent frame + shake

***

## **7.3.2 Fix pixeles layout**
**Rossz:** `setFixedWidth(300)`
**Miért rossz?** DPI‑függő, snapshot‑ellenes
**Megoldás:** percent‑based layout

***

## **7.3.3 Dialog a képernyő közepén**
**Rossz:** minden dialog középre ugrik
**Miért rossz?** eltakarja a fontos részeket
**Megoldás:** cursor‑based positioning

***

## **7.3.4 UI, amely domain‑logikát tartalmaz**
**Rossz:** `onOkClicked()` → „if (nameExists) …”
**Miért rossz?** domain → UI‑ba szivárog
**Megoldás:** validáció → Manager

***

# **7.4 Logolási anti‑patternök (ikonokkal és kulcsszínekkel)**
*A rossz ikonhasználat nem esztétikai hiba — hanem félrevezető kommunikáció, hibás UX és sérült audit‑logika.*

A logszintekhez tartozó ikonok és kulcsszínek **viselkedési szerződést** hordoznak.
Ha az ikon nem a megfelelő jelentést közvetíti, akkor:

- félrevezeti a fejlesztőt,
- félrevezeti a felhasználót,
- sérti az auditálhatóságot,
- rontja a hibakeresést,
- és megtöri a rendszer vizuális nyelvét.

Ezért az alábbi minták **szigorúan tiltottak**.

***

## **7.4.1 ❌ Minden log egy fájlba**
**Miért rossz?**

- zajos, kereshetetlen
- audit‑ellenes
- keveredik a DebugLog / SystemLog / EventLog
- nem lehet megkülönböztetni a user‑eseményt a technikai hibától

**Megoldás:**
három csatorna → DebugLog / SystemLog / EventLog

***

## **7.4.2 ❌ Túl hosszú logbejegyzések**
**Miért rossz?**

- olvashatatlan
- nem audit‑barát
- nem ergonomikus
- nem pre‑attentív

**Megoldás:**
rövid, emberi nyelvű üzenetek

***

## **7.4.3 ❌ Log, amely UI‑t módosít**
**Miért rossz?**

- a log nem UI
- a log nem popup
- domain/technikai információ nem jelenhet meg a felhasználónak

**Megoldás:**
log → log, UI → UI

***

# **7.4.4 ❌ Ikonok és kulcsszínek hibás használata (kritikus anti‑pattern)**
*A színkódolt ikonok jelentést hordoznak. Ha rossz ikont használunk, rossz jelentést közvetítünk.*

Az alábbi ikonhasználatok **szigorúan tiltottak**:

***

## **7.4.4.1 ❌ WARNING esetén piros ❌ ikon használata**
**Miért rossz?**

- a piros ❌ kritikus hibát jelent
- WARNING nem kritikus
- a felhasználó azt hiszi, hogy a rendszer megállt
- sérti a viselkedési szerződést

**Megoldás:**
WARNING → mindig ⚠️

***

## **7.4.4.2 ❌ ERROR esetén sárga ⚠️ ikon használata**
**Miért rossz?**

- a sárga ⚠️ csak óvatosságot jelent
- ERROR esetén a rendszer nem tud tovább menni
- félrevezető UX
- hibás audit‑jelentés

**Megoldás:**
ERROR → mindig ❌

***

## **7.4.4.3 ❌ INFO (sikeres művelet) esetén nem zöld ✔️ ikon használata**
**Miért rossz?**

- a zöld ✔️ a siker vizuális nyugtája
- nélküle a felhasználó nem kap pozitív megerősítést
- sérül a vizuális ritmus

**Megoldás:**
INFO + siker → ✔️

***

## **7.4.4.4 ❌ INFO (sikertelen művelet) esetén nem piros ❌ ikon használata**
**Miért rossz?**

- a piros ❌ jelzi a negatív eredményt
- nélküle a felhasználó nem érti, hogy a művelet nem sikerült
- félrevezető UX

**Megoldás:**
INFO + sikertelen → ❌

***

## **7.4.4.5 ❌ INFO (semleges) esetén nem kék ℹ️ ikon használata**
**Miért rossz?**

- a kék ℹ️ jelzi a semleges információt
- nélküle a felhasználó nem tudja, hogy nincs teendő
- sérül a vizuális konzisztencia

**Megoldás:**
INFO + semleges → ℹ️

***

## **7.4.4.6 ❌ Ikon nélküli WARNING vagy ERROR**
**Miért rossz?**

- nincs pre‑attentív jelzés
- a felhasználó nem látja a probléma súlyát
- auditálhatóság romlik

**Megoldás:**
WARNING → ⚠️
ERROR → ❌

***

## **7.4.4.7 ❌ Zöld ✔️ ikon használata sikertelen műveletnél**
**Miért rossz?**

- a zöld ✔️ a siker szimbóluma
- sikertelen műveletnél félrevezető
- UX‑szinten veszélyes

**Megoldás:**
sikertelen → ❌

***

## **7.4.4.8 ❌ Piros ❌ ikon használata sikeres műveletnél**
**Miért rossz?**

- a piros ❌ veszélyt jelent
- sikeres műveletnél teljesen félrevezető
- rontja a felhasználói bizalmat

**Megoldás:**
sikeres → ✔️

***

# **7.4.5 ❌ EventLog WARNING vagy ERROR**
**Miért rossz?**

- EventLog user‑szintű eseményekre való
- WARNING/ERROR domain vagy technikai hiba
- félrevezető UX

**Megoldás:**
WARNING/ERROR → SystemLog

***

# **7.4.6 ❌ Audit esemény WARNING vagy ERROR szinten**
**Miért rossz?**

- audit = domain művelet
- domain művelet nem hiba
- audit INFO szinten értelmezhető

**Megoldás:**
audit → SystemLog INFO

***

# **7.4.7 ❌ Nem szabványos időbélyeg**
**Miért rossz?**

- nem rendezhető
- nem audit‑kompatibilis
- locale‑függő

**Megoldás:**
ISO 8601 + UTC

***

# **7.4.8 ❌ Log → UI keverése**
**Miért rossz?**

- domain/technikai információ a felhasználónak
- UX‑romlás
- biztonsági kockázat

**Megoldás:**
UI → EventLog
domain → SystemLog

***

# **7.4.9 ❌ Domain log UI‑ba keverve**
**Miért rossz?**

- domain műveletek nem UI‑szintűek
- audit esemény nem popup
- domain log nem user‑üzenet

**Megoldás:**
audit → SystemLog INFO

***

# **7.5 Validációs anti‑patternök**

## **7.5.1 Validáció csak az OK gomb után**
**Miért rossz?** lassú, frusztráló
**Megoldás:** inline validation

***

## **7.5.2 Duplikált validáció**
**Rossz:** UI + Manager + Domain mind validál
**Miért rossz?** karbantarthatatlan
**Megoldás:** domain → egyetlen igazságforrás

***

## **7.5.3 Validáció, amely popupot dob**
**Miért rossz?** blokkol
**Megoldás:** accent frame + shake

***

# **7.6 Snapshot anti‑patternök**

## **7.6.1 Restore a konstruktorban**
**Miért rossz?** widget még nem látható → szétesett UI
**Megoldás:** deferred restore

***

## **7.6.2 Snapshot guard nélkül**
**Miért rossz?** off‑screen ablak, túl kicsi méret
**Megoldás:** GeometryGuard

***

## **7.6.3 Snapshot fallback nélkül**
**Miért rossz?** hibás snapshot → crash vagy fekete ablak
**Megoldás:** LayoutDefaultStore

***

# **7.7 Prompt anti‑patternök**

## **7.7.1 Túl rövid prompt**
**Rossz:** „írj kódot”
**Miért rossz?** nincs kontextus, nincs naming
**Megoldás:** kontextus + határok + példák

***

## **7.7.2 Túl hosszú, rendezetlen prompt**
**Miért rossz?** nem determinisztikus, nem auditálható
**Megoldás:** moduláris promptok

***

## **7.7.3 Prompt, amely nem tilt anti‑patternöket**
**Miért rossz?** hibás minták generálódnak
**Megoldás:** explicit tiltások

***

## **7.7.4 Prompt, amely nem ad példát**
**Miért rossz?** a rendszer nem tudja, milyen stílust vársz
**Megoldás:** példák + ellenpéldák

***

# **8. Sablonok és boilerplate minták**
*A sablonok egységesítik a fejlesztést, csökkentik a hibákat és rögzítik a kultúrát.*

A sablonok célja:

- egységesíteni a fejlesztést
- csökkenteni a hibalehetőséget
- gyorsítani a modulok létrehozását
- rögzíteni a fejlesztői kultúrát
- biztosítani a konzisztenciát

Minden sablon **projektfüggetlen**, de CutCalculator‑példákkal illusztrált.
A sablonok **Prompt DSL‑kompatibilisek**, **UI/UX‑tudatosak**, és **snapshot‑orientáltak**.

***

# **8.1 Registry sablon**

A Registry a domain állapotának auditálható, gyors elérésű tárolója.

## **Strukturális minta**
- konstruktor: üres
- belső tároló: `QVector<T>` vagy `QMap<Key, T>`
- metódusok:
  - `insert()`
  - `remove()`
  - `rename()`
  - `exists()`
  - `all()`
- audit: minden módosítás logolása
- validáció: minimális (létezés, ütközés)

## **Best practice**
- ne írjon fájlt
- ne tartalmazzon workflow‑t
- ne tartalmazzon UI‑t
- legyen gyors
- legyen determinisztikus
- legyen thread‑safe, ha szükséges

## **Anti‑pattern**
- Registry → IO
- Registry → UI
- Registry → workflow

## **CutCalculator példa**
- `ColorRegistry`
- `NeedRuleRegistry`

***

# **8.2 Repository sablon**

A Repository a tartós tárolásért felelős.

## **Strukturális minta**
- konstruktor: fájl elérési út
- metódusok:
  - `load()`
  - `save()`
  - `findById()`
  - `all()`
- formátum: CSV, JSON, XML
- hibakezelés: SystemLog

## **Best practice**
- ne validáljon
- ne auditáljon
- ne tartson állapotot
- legyen tiszta IO
- legyen verziótudatos

## **Anti‑pattern**
- Repository → workflow
- Repository → UI
- Repository → domain validáció

## **CutCalculator példa**
- `MaterialRepository`
- `NeedRepository`

***

# **8.3 Manager sablon**

A Manager a domain műveletekért és workflow‑ért felelős.

## **Strukturális minta**
- konstruktor: Registry + Repository
- metódusok:
  - `create()`
  - `update()`
  - `delete()`
  - `validate()`
- audit: minden művelet
- workflow: tranzakciós logika

## **Best practice**
- ne tartalmazzon UI‑t
- ne írjon fájlt
- ne legyen God Object
- legyen tiszta, moduláris
- legyen domain‑vezérelt

## **Anti‑pattern**
- Manager → UI
- Manager → IO
- Manager → állapot

## **CutCalculator példa**
- `NeedCalculationManager`
- `DetailManager`

***

# **8.4 Dialog sablon**

A Dialog rövid életű UI‑komponens.

## **Strukturális minta**
- konstruktor: input paraméterek
- UI: mezők, gombok
- validáció: inline
- események: `accept()`, `reject()`
- pozicionálás: cursor‑based
- snapshot: opcionális (kicsi dialogoknál nem szükséges)

## **Best practice**
- ne tartalmazzon domain‑logikát
- ne írjon fájlt
- ne módosítson Registry‑t közvetlenül
- legyen snapshot‑kompatibilis, ha nagyobb dialog
- legyen DPI‑független

## **Anti‑pattern**
- Dialog → domain validáció
- Dialog → repository hívás
- Dialog → popup minden hibára

## **CutCalculator példa**
- `ModeNameDialog`
- `RenameDialog`

***

# **8.5 Picker sablon**

A Picker domain‑entitás kiválasztására szolgál.

## **Strukturális minta**
- lista vagy tábla
- keresőmező
- inline validation
- double‑click → accept
- Enter → accept
- Esc → reject

## **Best practice**
- ne módosítson domain‑adatot
- ne validáljon domain‑szinten
- legyen gyors és ergonomikus
- legyen snapshot‑kompatibilis

## **Anti‑pattern**
- Picker → domain módosítás
- Picker → popup minden hibára

## **CutCalculator példa**
- `MaterialPickerDialog`

***

# **8.6 Editor sablon**

Az Editor domain‑entitás szerkesztésére szolgál.

## **Strukturális minta**
- mezők
- inline validation
- preview (ha szükséges)
- accept/reject
- snapshot‑kompatibilis layout

## **Best practice**
- domain‑validáció → Manager
- UI‑validáció → inline
- snapshot‑kompatibilis layout
- DPI‑független méretezés

## **Anti‑pattern**
- Editor → domain módosítás közvetlenül
- Editor → repository hívás

## **CutCalculator példa**
- `FormulaEditorDialog`

***

# **8.7 Snapshot‑kompatibilis UI sablon**

A snapshot‑tudatos UI ipari követelmény.

## **Strukturális minta**
- `saveSnapshot()`
- `restoreSnapshot()`
- `restoreDeferred()`
- `applyFallbackLayout()`
- `GeometryGuard` integráció
- `ScreenChangeDetector` integráció

## **Best practice**
- restore csak show() után
- guard minden restore előtt
- fallback baseline minden hibára
- snapshot verziózás

## **Anti‑pattern**
- restore a konstruktorban
- snapshot guard nélkül
- snapshot fallback nélkül

## **CutCalculator példa**
- `MainWindow`
- `BOMWorkbench`

***

# **8.8 Logolási sablonok**

### SystemLog sablon
- technikai részletek
- hibák
- audit események (INFO)
- workflow trace

### **EventLog sablon**
- user‑szintű események
- rövid, érthető üzenetek

### Audit event sablon
- domain művelet
- entitás + művelet + paraméterek
- SystemLog‑ba kerül (INFO)

## **Best practice**
- logok szétválasztása
- rövid üzenetek
- időbélyeg mindenhol
- verbose routing

## **Anti‑pattern**
- minden log egy fájlba
- log → popup
- log → UI

***

# **9. Prompt DSL és Prompt Engineering**
*A modern fejlesztésben a prompt a fejlesztői kultúra része.*

A prompt nem egyszerű utasítás, hanem:

- architektúra‑leírás
- workflow‑definíció
- naming‑szabály
- anti‑pattern tiltás
- fejlesztői gondolkodásmód

A CutCalculator alatt kialakult egy olyan **Prompt DSL**, amely:

- moduláris
- konzisztens
- auditálható
- determinisztikus
- fejlesztői kultúrát hordoz
- projektfüggetlenül is alkalmazható

Ez a fejezet rögzíti ezt a nyelvet.

***

# **9.1 A prompt mint specifikáció**

A jó prompt:

- kontextust ad
- határokat ad
- naminget ad
- példát ad
- anti‑patternöket tilt
- workflow‑t definiál
- sablonokat használ

A prompt **nem kérés**, hanem **műszaki dokumentum**, amelyből:

- kód
- architektúra
- UI
- workflow
- dokumentáció
- sablonok

születnek.

A prompt a fejlesztői kultúra egyik legfontosabb eszköze.

***

# **9.2 A Prompt DSL szerkezete**

A CutCalculator Prompt DSL négy fő blokkból áll.
Ezek sorrendje és jelenléte **kötelező**, mert a determinisztikus működés alapja.

***

## **9.2.1 Kontextus blokk**

A projekt, modul és cél meghatározása.

**Tartalmazza:**

- domain
- modul
- cél
- hatókör
- input/output elvárások

**Példa:**

```
Kontextus:
- Modul: NeedCalculation
- Cél: új NeedCalculationManager generálása
- Domain: Need, Material, Rule
```

***

## **9.2.2 Naming blokk**

A projekt naming‑szabályainak explicit rögzítése.

**Tartalmazza:**

- preferált nevek
- tiltott nevek
- domain‑fogalmak
- szerepvégződések

**Példa:**

```
Naming:
- Használd: NeedCalculationManager, NeedRegistry
- Tilos: Service, Handler, Processor
```

***

## **9.2.3 Szerkezeti blokk**

A generálandó modul szerkezeti terve.

**Tartalmazza:**

- osztályok
- metódusok
- workflow
- UI‑elemek (ha vannak)
- snapshot‑követelmények

**Példa:**

```
Szerkezet:
- Osztály: NeedCalculationManager
- Metódusok: create(), update(), delete(), validate()
- Workflow: domain validáció + audit
```

***

## **9.2.4 Anti‑pattern blokk**

A tiltott minták explicit felsorolása.

**Tartalmazza:**

- architekturális tiltások
- naming tiltások
- UI tiltások
- snapshot tiltások

**Példa:**

```
Anti-pattern:
- Manager nem írhat fájlt
- UI nem tartalmazhat domain logikát
- Snapshot restore nem lehet konstruktorban
```

***

# 📘 **9.X Prompt szerkezet és ipari anti‑pattern kézikönyv**
*A prompt nem csak szándék — hanem szerkezet. A jó prompt nem csak működik — hanem tanítható, újrahasznosítható és auditálható.*

A Microsoft Copilot promptmodellje négy kulcselemre épül:

- **Goal** → mit várunk
- **Context** → miért, kinek, milyen helyzetben
- **Expectations** → milyen formátumban, milyen stílusban
- **Source** → milyen példák, adatok, fájlok alapján

Ez a négyes keret **tökéletesen illeszkedik** a CutCalculator Prompt DSL‑hez, és megerősíti, hogy a DSL‑ünk nem csak belső workflow, hanem **ipari best practice**.

***

## ✅ **9.X.1 A jó prompt szerkezete (Copilot + CutCalculator)**

| Prompt elem | Copilot definíció | CutCalculator megfelelője |
|************-|******************-|***************************-|
| **Goal** | Mit várunk az AI‑tól | Prompt célja, generált entitás típusa |
| **Context** | Miért, kinek, milyen helyzetben | Kontextus blokk: domain, szerep, cél |
| **Expectations** | Milyen formátumban, milyen stílusban | Szerkezet blokk: struktúra, naming, logika |
| **Source** | Milyen példák, adatok, fájlok alapján | Példák, audit események, indexed promptok |

A CutCalculator Prompt DSL tehát **nem különbözik**, hanem **kibővíti** az ipari promptmodell keretét.

***

## ✅ **9.X.2 Do’s — Hogyan kell jól promptolni (Microsoft Copilot alapján)**

- **Strukturáld a promptot** → a fontos elemek kerüljenek a végére
- **Használj pozitív utasításokat** → mondd meg, mit csináljon, ne azt, mit ne
- **Ismételj a pontosságért** → az első válasz nem mindig a legjobb
- **Légy világos és konkrét** → cél + kontextus + elvárás + példa
- **Tartsd fenn a beszélgetést** → iterálj, pontosíts
- **Adj példákat** → kulcsszavak, minták, stílus
- **Ellenőrizd a válaszokat** → Copilot is tévedhet
- **Adj részleteket** → kollégák, témák, előzmények
- **Légy udvarias** → a nyelvezet befolyásolja a válasz minőségét

***

## ❌ **9.X.3 Don’ts — Hogyan lehet elrontani (Microsoft Copilot alapján)**

- **Ne légy homályos** → „Írj valamit” = rossz prompt
- **Ne kérj etikátlan tartalmat** → Copilot nem segít ilyesmiben
- **Ne adj ellentmondó utasításokat** → „Legyen rövid, de részletes”
- **Ne váltogasd a témát hirtelen** → új téma = új prompt
- **Ne használj informális nyelvet** → slang, dialektus, zsargon = félreértés
- **Ne bízz vakon a válaszban** → mindig ellenőrizd
- **Ne kérj illegális segítséget** → Copilot etikai keretek között működik

***

## 🎯 **9.X.4 A CutCalculator Prompt DSL mint ipari modell**

A CutCalculator Prompt DSL:

- **strukturált** → négy blokk
- **auditálható** → prompt = dokumentum
- **determininsztikus** → példák + tiltások
- **kultúrába illeszkedő** → naming, logolás, UI
- **tanítható** → onboarding, review, dokumentáció
- **skálázható** → prompt‑lánc, modularizálás, verziózás

Ez a DSL nem csak működik —
**hanem tanítható, újrahasznosítható és ipari szintű.**

***
# 📘 **9.Y Prompt DSL Standard Library (ipari sablonok és minták)**
*(A CutCalculator fejlesztői kultúra hivatalos prompt‑könyvtára)*

***

# 📘 **9.Y Prompt DSL Standard Library**
*A Prompt DSL Standard Library a CutCalculator fejlesztői kultúrájának „szerszámosládája”: előre definiált, ipari minőségű prompt‑minták, amelyek garantálják a konzisztens, auditálható és determinisztikus kódgenerálást.*

A Standard Library célja:

- egységesíteni a promptok szerkezetét,
- csökkenteni a hibalehetőséget,
- gyorsítani a fejlesztést,
- biztosítani a projektkultúra átörökítését,
- és kizárni a rossz mintákat (anti‑pattern).

A könyvtár minden eleme a következő ipari modellekre épül:

- **Microsoft Copilot promptmodell**:
  *Goal + Context + Expectations + Source*
- **CutCalculator Prompt DSL**:
  *Kontextus + Naming + Szerkezet + Anti‑pattern*
- **Prompt Engineering szakirodalom**:
  *példák, tiltások, iteráció, szerepdefiníció, modularizáció*

***

# 🎯 **9.Y.1 Alap Prompt Sablon (Master Template)**
*(Minden generált prompt alapja)*

```
# GOAL
A feladat célja: [mit várunk a generált kódtól vagy dokumentumtól].

# CONTEXT
A domain kontextusa: [miért kell, kinek készül, milyen helyzetben használjuk].
A modell szerepe: „Viselkedj úgy, mint egy senior C++/Qt fejlesztő.”

# EXPECTATIONS
A válasz formátuma: [struktúra, stílus, részletesség].
A válasz korlátai: [max hossz, tiltott elemek, stílus].

# SOURCE
Példák: [1–3 releváns példa].
Források: [audit események, indexed promptok, dokumentáció].

# DSL BLOCKS
Kontextus: [CutCalculator modul, domain fogalmak].
Naming: [kötelező névkonvenciók].
Szerkezet: [osztályok, metódusok, logika].
Anti-pattern: [tiltott minták listája].

# OUTPUT
Csak a kért struktúrát add vissza, kommentekkel és logolással.
```

Ez a sablon **minden generált prompt alapja**.

***

# 🧱 **9.Y.2 Modul Prompt Template (Manager / Registry / Dialog)**

```
# GOAL
Hozz létre egy [Manager/Registry/Dialog] osztályt a CutCalculator architektúrában.

# CONTEXT
A modul célja: [pl. Mode kezelés, Material kezelés].
A modell szerepe: „Senior Qt/C++ fejlesztő vagy, aki ismeri a projektkultúrát.”

# EXPECTATIONS
A válasz tartalmazza:
- osztálydefiníció
- publikus API
- privát mezők
- logolás (ikonokkal)
- audit események
- UI jelzések (ha releváns)

# SOURCE
Példák: [korábbi Manager/Registry minták].
Forrás: [audit események, naming guide].

# DSL BLOCKS
Kontextus: [modul neve, domain fogalmak].
Naming: [CamelCase, m_ prefix, zInfo/zWarning/zError].
Szerkezet: [kötelező metódusok].
Anti-pattern:
- Tilos UI‑t keverni domain logikával
- Tilos WARNING → ❌ ikon
- Tilos ERROR → ⚠️ ikon
- Tilos EventLog‑ba WARNING/ERROR

# OUTPUT
Csak a kódot add vissza, magyar kommentekkel, angol technikai kifejezésekkel.
```

***

# 🧩 **9.Y.3 Prompt Template — Logolás generálása**

```
# GOAL
Generálj logolási kódot a következő művelethez: [művelet].

# CONTEXT
A logolás célja: auditálhatóság + UX visszajelzés.
A modell szerepe: „Te vagy a projekt logolási szakértője.”

# EXPECTATIONS
Használd a megfelelő ikont:
- ℹ️ semleges INFO
- ✔️ sikeres INFO
- ❌ sikertelen INFO
- ⚠️ WARNING
- ❌ ERROR (kritikus)

# SOURCE
Forrás: 6.4 Logolási best practice, 7.4 Anti‑pattern.

# DSL BLOCKS
Anti-pattern:
- Tilos WARNING → ❌
- Tilos ERROR → ⚠️
- Tilos ikon nélküli WARNING/ERROR
- Tilos audit eseményt WARNING/ERROR szinten írni

# OUTPUT
Adj vissza egy zInfo/zWarning/zError hívást.
```

***

# 🧱 **9.Y.4 Prompt Template — UI/UX visszajelzés generálása**

```
# GOAL
Generálj UI visszajelzést a következő művelethez: [művelet].

# CONTEXT
A felhasználó gyors, pre‑attentív információt kapjon.

# EXPECTATIONS
Használd a megfelelő ikont:
- ℹ️ információ
- ✔️ siker
- ❌ sikertelen művelet
- ⚠️ figyelmeztetés

# SOURCE
Forrás: 10.X Ikonok és kulcsszínek szerepe a UI/UX-ben.

# DSL BLOCKS
Anti-pattern:
- Tilos ❌ ikon WARNING esetén
- Tilos ⚠️ ikon ERROR esetén
- Tilos ✔️ ikon sikertelen műveletnél

# OUTPUT
Adj vissza egy rövid, emberi nyelvű UI üzenetet.
```

***

# 🧪 **9.Y.5 Prompt Template — Teszt generálása**

```
# GOAL
Generálj unit tesztet a következő osztályhoz: [osztály].

# CONTEXT
A teszt célja: regresszió megelőzése, auditálhatóság.

# EXPECTATIONS
A teszt legyen:
- determinisztikus
- rövid
- jól olvasható
- logolás nélkül

# SOURCE
Példák: [korábbi tesztminták].

# DSL BLOCKS
Anti-pattern:
- Tilos UI‑t tesztelni unit tesztben
- Tilos random értékeket használni
- Tilos külső függőséget mock nélkül hívni

# OUTPUT
Adj vissza egy teljes tesztfájlt.
```

***

# ❌ **9.Y.6 Prompt Anti‑Pattern Library (ipari tiltások)**
*(A „hogyan ne csináljuk” kézikönyv)*

### ❌ Homályos prompt
„Írj egy kódot.”

### ❌ Ellentmondó utasítás
„Legyen rövid, de részletes.”

### ❌ Témaugrás
„Írj egy dialogot. Ja, és a MaterialRegistry‑t is.”

### ❌ Informális nyelvezet
„Csinájjá’ egy osztályt.”

### ❌ Domain‑idegen naming
„ProfileManager” egy Mode‑alapú domainben.

### ❌ Tiltások hiánya
Ha nincs anti‑pattern blokk → rossz kód.

### ❌ Példák hiánya
A modell nem tudja, milyen stílust vársz.

### ❌ Rossz ikonhasználat
- WARNING → ❌
- ERROR → ⚠️
- INFO (siker) → nem ✔️
- INFO (sikertelen) → nem ❌

### ❌ Audit esemény WARNING/ERROR szinten
Audit = mindig INFO.

***

# **9.3 Prompt Standard Library**

A CutCalculator alatt kialakult egy „standard library” — olyan prompt‑minták, amelyek újra és újra használhatók.

***

## **9.3.1 Registry prompt sablon**

```
Kontextus: XRegistry generálása
Naming: XRegistry, insert/remove/rename
Szerkezet: QVector<T>, audit minden módosításnál
Anti-pattern: Registry nem ír fájlt
```

***

## **9.3.2 Manager prompt sablon**

```
Kontextus: XManager generálása
Naming: XManager, create/update/delete
Szerkezet: Registry + Repository + workflow
Anti-pattern: Manager nem tartalmaz UI-t
```

***

## **9.3.3 Dialog prompt sablon**

```
Kontextus: XDialog generálása
Naming: XDialog, inline validation
Szerkezet: mezők, gombok, accept/reject
Anti-pattern: Dialog nem validál domain-szinten
```

***

## **9.3.4 Snapshot prompt sablon**

```
Kontextus: Snapshot-kompatibilis UI generálása
Naming: restoreSnapshot(), restoreDeferred()
Szerkezet: GeometryGuard + fallback baseline
Anti-pattern: restore a konstruktorban
```

***
# **9.X Logolási szabályok a Prompt DSL-ben**
*A generált kód logolási viselkedése a Prompt DSL része — nem opcionális, hanem architekturális követelmény.*

A Prompt DSL célja, hogy a generált kód **determininsztikus**, **audit‑barát**, **ergonomikus** és **konzisztens** legyen.
A logolás a rendszer egyik legfontosabb viselkedési szerződése, ezért a DSL‑ben is rögzíteni kell.

A logolási szabályok biztosítják, hogy:

- minden generált modul helyes logszinteket használjon,
- a logok ikonja és kulcsszíne a jelentést tükrözze,
- audit események mindig INFO szinten jelenjenek meg,
- WARNING és ERROR soha ne kerüljön EventLog‑ba,
- a DebugLog csak fejlesztés alatt legyen aktív,
- a logok ISO 8601 + UTC időbélyeget használjanak.

***

# **9.X.1 Logcsatornák a Prompt DSL-ben**

A generált kód kizárólag a következő csatornákat használhatja:

- **DebugLog** → `zTrace()`, `zDebug()`
- **SystemLog** → `zInfo()`, `zWarning()`, `zError()`
- **EventLog** → `zEventINFO()`

**Tiltás:**

- nincs `zEventWarning()`
- nincs `zEventError()`
- nincs `AuditLog` külön csatorna

***

# **9.X.2 Logszintek és ikonok a Prompt DSL-ben**

A Prompt DSL‑ben a logszint meghatározza:

- a viselkedést,
- az ikon kulcsszínét,
- a csatornát,
- az auditálhatóságot.

A generált kódnak a következő ikonokat kell használnia:

| Jelentés | Ikon | Szín | Logszint |
|*********|******|******|*********-|
| Semleges információ | ℹ️ | kék | INFO |
| Sikeres művelet | ✔️ | zöld | INFO |
| Sikertelen művelet (nem kritikus) | ❌ | piros | INFO |
| Figyelmeztetés | ⚠️ | sárga | WARNING |
| Kritikus hiba | ❌ | piros | ERROR |

**Tiltás:**

- WARNING → ❌ ikon → ❌
- ERROR → ⚠️ ikon → ❌
- INFO (sikeres) → nem ✔️ → ❌
- INFO (sikertelen) → nem ❌ → ❌
- INFO (semleges) → nem ℹ️ → ❌

***

# **9.X.3 Audit események generálása**

A Prompt DSL‑ben minden domain művelethez audit eseményt kell generálni:

- audit esemény → **SystemLog INFO**
- ikon → ℹ️ (semleges) vagy ✔️ (sikeres)
- formátum → rövid, determinisztikus
- időbélyeg → ISO 8601 + UTC

**Példa (helyes):**

```
zInfo("ℹ️ Need created: id=%1, name=\"%2\"", id, name);
```

**Tiltás:**

- audit WARNING → ❌
- audit ERROR → ❌
- audit EventLog‑ban → ❌
- audit túl hosszú vagy technikai részletekkel → ❌

***

# **9.X.4 WARNING és ERROR generálása**

A Prompt DSL‑ben a WARNING és ERROR generálása **viselkedési szerződéshez kötött**.

### **WARNING (⚠️) akkor használható, ha:**

- a rendszer tovább tud menni
- fallback vagy degradált működés történik
- a hiba nem kritikus

**Példa:**

```
zWarning("⚠️ Snapshot corrupted, using fallback baseline");
```

### **ERROR (❌) akkor használható, ha:**

- a rendszer nem tud tovább menni
- kritikus hiba történt
- invariáns sérült
- a program leáll

**Példa:**

```
zError("❌ Cannot open settings file: %1", path);
```

**Tiltás:**

- WARNING → EventLog
- ERROR → EventLog
- WARNING → ❌ ikon
- ERROR → ⚠️ ikon

***

# **9.X.5 DebugLog használata a generált kódban**

A Prompt DSL‑ben a DebugLog csak fejlesztői célra használható:

- `zTrace()` → vezérléskövetés
- `zDebug()` → mély diagnosztika

**Tiltás:**

- DebugLog használata release buildben
- DebugLog keverése audit eseményekkel

***

# **9.X.6 Időbélyegzés a generált kódban**

A generált logoknak kötelezően:

- **ISO 8601** formátumot
- **UTC** időzónát

kell használniuk.

Ez biztosítja:

- auditálhatóságot
- rendezhetőséget
- több gép közötti konzisztenciát

***

# **9.X.7 Anti‑pattern tiltások a Prompt DSL-ben**

A Prompt DSL‑ben explicit módon tiltani kell:

- ❌ ikon WARNING esetén
- ⚠️ ikon ERROR esetén
- ✔️ ikon sikertelen műveletnél
- ℹ️ ikon hibás műveletnél
- WARNING/ERROR EventLog‑ban
- audit esemény WARNING/ERROR szinten
- nem szabványos időbélyeg
- log → popup
- domain log UI‑ba keverve

Ezek a tiltások garantálják, hogy a generált kód **mindig konzisztens** legyen.

***

# **9.4 Prompt anti‑patternök**

## **9.4.1 Túl rövid prompt**
**Rossz:** „írj egy dialogot”
**Miért rossz?** nincs kontextus, nincs naming, nincs workflow

***

## **9.4.2 Túl hosszú, rendezetlen prompt**
**Rossz:** 3 oldalnyi szöveg struktúra nélkül
**Miért rossz?** nem determinisztikus, nem auditálható

***

## **9.4.3 Prompt, amely nem tilt anti‑patternöket**
**Miért rossz?** hibás minták generálódnak

***

## **9.4.4 Prompt, amely nem ad példát**
**Miért rossz?** a rendszer nem tudja, milyen stílust vársz

***

## **9.4.5 Prompt, amely domain‑idegen neveket használ**
**Rossz:** `ProfileManager` (ha a domainben „Mode” a fogalom)

***

# **9.5 Prompt workflow**

A prompt‑vezérelt fejlesztés lépései:

1. **Kontextus definiálása**
2. **Naming meghatározása**
3. **Szerkezet kijelölése**
4. **Anti‑pattern tiltások**
5. **Példák adása**
6. **Generálás**
7. **Audit** (a prompt → dokumentáció)

Ez a folyamat teszi a promptot determinisztikussá és auditálhatóvá.

***

# **9.6 A prompt mint fejlesztői kultúra**

A prompt:

- tisztaságot ad
- ritmust ad
- konzisztenciát ad
- auditálható
- visszakereshető
- fejlesztői gondolkodást formál

A Prompt DSL a CutCalculator fejlesztői kultúrájának egyik legfontosabb eleme.

***
# **10. UI/UX Guidelines ikonokkal — A modern felhasználói élmény alapelvei a CutCalculatorben**
*A felület nem dísz. A felület a rendszer arca. A jó UX pedig a felhasználó és a fejlesztő közös nyelve.*

A modern UI/UX célja, hogy a digitális termék:

- **könnyen használható**,
- **érthető**,
- **következetes**,
- **gyorsan értelmezhető**,
- **vizuálisan tiszta**,
- **és élvezetes** legyen.

A CutCalculator UI/UX‑filozófiája erre épül — de kiegészül egy egyedi elemmel:
**az ikon‑vezérelt, színkódolt, pre‑attentív visszajelzésrendszerrel**, amely a logolási architektúrával teljes összhangban működik.

***

# 🎯 **10.1 Miért fontos a UI/UX?**

A UI/UX nem „szépítés”.
A UI/UX a rendszer *érzékszerve*.

- A felhasználó a felületet látja → nem a kódot.
- A felhasználó a visszajelzést érti → nem a logot.
- A felhasználó a színeket érzékeli → nem a stack trace‑t.

A jó UX:

- csökkenti a hibákat,
- csökkenti a support terhelést,
- csökkenti a frusztrációt,
- növeli a bizalmat,
- növeli a hatékonyságot,
- növeli a rendszer értékét.

A jó UX a fejlesztő barátja is.
Barátom barátja a barátom.

***

# 🎯 **10.2 UX alapelvek (modern, CutCalculator‑specifikus)**

## **1) Felhasználó‑központúság**
A felület nem a fejlesztőnek készül.
A felület a felhasználó számára:

- érthető,
- kiszámítható,
- következetes,
- és biztonságos.

A CutCalculatorben ez azt jelenti:

- minden művelet kap vizuális visszajelzést,
- minden hiba érthető,
- minden figyelmeztetés látható,
- minden siker megerősített.

***

## **2) Intuitív navigáció**
A felhasználónak nem szabad gondolkodnia azon, hogy:

- „Hol vagyok?”
- „Mit kell csinálnom?”
- „Mi történik most?”

A rendszernek *magától értetődőnek* kell lennie.

***

## **3) Feladat‑orientáltság**
A felhasználó célja nem a program használata.
A felhasználó célja a *munka elvégzése*.

A UI feladata:

- segíteni,
- nem akadályozni,
- nem elrejteni,
- nem túlterhelni.

***

## **4) Azonosíthatóság és következetesség**
A felhasználó gyorsabban tanul, ha:

- ugyanaz a szín → ugyanaz a jelentés
- ugyanaz az ikon → ugyanaz a viselkedés
- ugyanaz a gomb → ugyanaz a funkció

Ezért van a CutCalculatorben:

- ℹ️ → információ
- ✔️ → siker
- ❌ → sikertelen művelet
- ⚠️ → figyelmeztetés
- ❌ (ERROR) → kritikus hiba

***

## **5) Teljes élmény**
A UX nem csak a gombokról szól.

A UX része:

- a töltési idő,
- a hibaüzenetek,
- a visszajelzések,
- a navigáció,
- a vizuális ritmus,
- a konzisztencia,
- a hibák megelőzése.

A CutCalculator UX‑e audit‑tudatos:
a felhasználó mindig tudja, mi történt.

***

# 🎯 **10.3 UI alapelvek (modern, CutCalculator‑specifikus)**

## **1) Egyszerűség és tisztaság**
A UI nem lehet zsúfolt.
A UI nem lehet túlmagyarázott.

A CutCalculator UI‑ja:

- minimalista,
- tiszta,
- fókuszált.

***

## **2) Vizuális hierarchia**
A fontos dolgok legyenek:

- nagyobbak,
- erősebb színűek,
- közelebb a fókuszhoz.

A kevésbé fontosak legyenek:

- halványabbak,
- kisebbek,
- távolabb.

***

## **3) Következetesség**
A UI‑ban minden elem:

- ugyanúgy néz ki,
- ugyanúgy viselkedik,
- ugyanazt jelenti.

Ez a UX egyik legerősebb stabilizáló ereje.

***

## **4) Kontraszt és olvashatóság**
A felhasználó nem fogja „kitalálni”, mi van a képernyőn.
A szöveg legyen:

- jól olvasható,
- megfelelő kontrasztú,
- megfelelő méretű.

***

## **5) Visszajelzés**
A felhasználó minden művelet után tudni akarja:

- sikerült?
- nem sikerült?
- mi történt?
- mi a következő lépés?

Ezért van a CutCalculatorben:

- ✔️ siker
- ❌ sikertelen művelet
- ⚠️ figyelmeztetés
- ℹ️ információ

***

# 🎯 **10.4 Modern trendek (CutCalculator‑kompatibilis)**

## **Mikrointerakciók**
Apró, de fontos visszajelzések:

- gomb animáció
- hover állapot
- progress indikátor
- ikon villanás

Ezek segítik a felhasználót.

***

## **Personák és felhasználói sztorik**
A CutCalculator UI‑ja nem „általános felhasználónak” készül.
A célcsoport:

- gyártási szakemberek,
- mérnökök,
- adminisztrátorok,
- operátorok.

A UI‑t az ő gondolkodásukhoz kell igazítani.

***

## **Adatvezérelt design**
A UX nem vélemény.
A UX mérhető:

- hibaarány
- idő a feladat elvégzéséhez
- félreértések száma
- support ticketek száma

***

## **Mozi‑szerű élmény**
Nem animációk miatt —
hanem a **zökkenőmentes folyamatok** miatt.

A CutCalculatorben ez:

- gyors reakció
- egyértelmű visszajelzés
- vizuális ritmus
- konzisztens ikonok

***

# 🎯 **10.5 Ikonok és kulcsszínek — a CutCalculator vizuális nyelve**

Ez a rendszer egyik legfontosabb UX‑eleme.

| Ikon | Szín | Jelentés | Mikor használjuk |
|******|******|*********-|******************|
| ℹ️ | kék | semleges információ | normál működés |
| ✔️ | zöld | sikeres művelet | mentés, import, számítás |
| ❌ | piros | sikertelen művelet | nem kritikus hiba |
| ⚠️ | sárga | figyelmeztetés | fallback, degradált működés |
| ❌ (ERROR) | piros | kritikus hiba | programleállás |

Ez a vizuális nyelv:

- gyors,
- egyértelmű,
- kognitív,
- pre‑attentív,
- audit‑tudatos.

***

# 🎯 **10.6 Anti‑pattern — hogyan ne csináljuk**

- ❌ WARNING → ❌ ikon
- ❌ ERROR → ⚠️ ikon
- ❌ ikon nélküli hiba
- ❌ túl hosszú hibaüzenet
- ❌ technikai részletek a UI‑ban
- ❌ következetlen színek
- ❌ túl sok információ
- ❌ túl kevés információ
- ❌ UI, ami nem mondja meg, mi történt

***
# **10.7 A UX mint hibamegelőző rendszer**
*A jó UX nem csak szép — hanem biztonságos. A jó UX nem csak kényelmes — hanem megelőzi a hibákat. A jó UX nem csak élmény — hanem minőségbiztosítás.*

A szoftverhibák jelentős része **nem a logikából**, hanem **a félreértett felhasználói felületből** ered.
A modern UX egyik legfontosabb felismerése:

> **A felhasználó nem hibázik — a rendszer hagyta hibázni.**

A CutCalculator UX‑filozófiája ezért nem csak esztétikai, hanem **minőségbiztosítási és audit‑tudatos**.

A jó UX:

- megelőzi a hibát,
- csökkenti a hibák súlyosságát,
- gyorsítja a hibák felismerését,
- segíti a hibák kijavítását,
- csökkenti a support terhelést,
- csökkenti a fejlesztői terhelést.

***

# 🎯 **10.7.1 A hibák 80%-a UX‑eredetű (és megelőzhető)**

A hibák nagy része nem programozási hiba, hanem:

- félreértett gomb
- rosszul elhelyezett funkció
- hiányzó visszajelzés
- túl sok visszajelzés
- rossz ikon
- rossz szín
- rossz hierarchia
- rossz navigáció
- hiányzó kontextus

A CutCalculator UX‑e ezeket a hibákat **előre kizárja**.

***

# 🎯 **10.7.2 A jó UX megelőzi a hibát (prevention)**

A jó UX:

- nem engedi, hogy rossz adatot adj be
- nem engedi, hogy rossz gombot nyomj meg
- nem engedi, hogy félreértsd a visszajelzést
- nem engedi, hogy eltévedj
- nem engedi, hogy bizonytalan legyél

Ezért van:

- ✔️ siker ikon
- ❌ sikertelen művelet ikon
- ⚠️ figyelmeztetés ikon
- ℹ️ információ ikon
- konzisztens színkódolás
- rövid, emberi nyelvű üzenetek
- egyértelmű gombok
- egyértelmű hierarchia

A UX itt **védőháló**.

***

# 🎯 **10.7.3 A jó UX csökkenti a hibák súlyosságát (mitigation)**

Ha mégis hiba történik, a jó UX:

- azonnal jelzi
- egyértelműen jelzi
- nem dramatizálja
- nem rejti el
- nem technikai részleteket ír
- megmondja, mi a következő lépés

Példák:

**Jó:**
❌ „A mentés nem sikerült (fájl zárolva). Próbáld újra később.”

**Rossz:**
„Unhandled exception: file access violation at offset 0x00000014.”

A jó UX **nem büntet**, hanem **segít**.

***

# 🎯 **10.7.4 A jó UX gyorsítja a hibák felismerését (detection)**

A CutCalculator ikon‑vezérelt rendszere pre‑attentív:

- a felhasználó *szöveg nélkül* is látja, hogy baj van
- a fejlesztő *log nélkül* is látja, mi történt
- a support *képernyőkép alapján* is érti a helyzetet

Ezért fontos a konzisztens ikonhasználat:

- ❌ = hiba
- ⚠️ = figyelmeztetés
- ✔️ = siker
- ℹ️ = információ

A vizuális nyelv **gyorsabb**, mint a szöveg.

***

# 🎯 **10.7.5 A jó UX segíti a hibák kijavítását (recovery)**

A jó UX nem csak jelzi a hibát —
**hanem segít kijavítani**.

Példák:

- „A fájl nem nyitható meg. Ellenőrizd, hogy létezik‑e.”
- „A snapshot sérült. Fallback baseline használata.”
- „A mező nem lehet üres.”
- „Érvénytelen számformátum.”

A jó UX *irányt ad*.

***

# 🎯 **10.7.6 A jó UX csökkenti a support terhelést**

A support terhelés nagy része:

- félreértett funkció
- félreértett hibaüzenet
- hiányzó visszajelzés
- rossz ikon
- rossz szín
- rossz navigáció

A jó UX:

- kevesebb ticket
- kevesebb telefon
- kevesebb magyarázat
- kevesebb frusztráció

A jó UX **üzleti érték**.

***

# 🎯 **10.7.7 A jó UX csökkenti a fejlesztői terhelést**

A fejlesztők idejét gyakran nem a kódolás viszi el, hanem:

- hibák reprodukálása
- félreértett funkciók tisztázása
- rossz visszajelzések javítása
- supporttal való egyeztetés

A jó UX:

- kevesebb bug
- kevesebb félreértés
- kevesebb visszajelzés
- kevesebb újratervezés

A jó UX **fejlesztői időt szabadít fel**.

***

# 🎉 **Összegzés — a UX mint minőségbiztosítás**

A jó UX:

- megelőzi a hibát
- csökkenti a hibák súlyosságát
- gyorsítja a hibák felismerését
- segíti a hibák kijavítását
- csökkenti a support terhelést
- csökkenti a fejlesztői terhelést

A UX nem extra.
A UX nem dísz.
A UX **minőségbiztosítás**.

A UX a rendszer egyik legfontosabb védelmi vonala.

***
# **10.8 A UX és a logolás kapcsolata — hogyan beszél ugyanazon a nyelven a UI és a SystemLog**
*A CutCalculator egyik legfontosabb tervezési elve: a felhasználói felület és a logolási rendszer ugyanazt a vizuális és fogalmi nyelvet használja. Ez teszi a rendszert érthetővé, auditálhatóvá és hibabiztossá.*

A legtöbb alkalmazásban a UI és a logolás két külön világ:

- a UI saját ikonokat használ,
- a log saját szinteket,
- a kettő között nincs kapcsolat,
- a felhasználó mást lát, mint amit a rendszer tud,
- a fejlesztő mást lát, mint amit a felhasználó ért.

A CutCalculatorben ez **nem így van**.

A CutCalculatorben a UI és a logolás **egy nyelvet beszél**.

Ez a nyelv:

- ikon‑vezérelt,
- színkódolt,
- pre‑attentív,
- audit‑tudatos,
- és konzisztens.

***

# 🎯 **10.8.1 Miért fontos, hogy a UI és a logolás ugyanazt a nyelvet használja?**

Mert így:

- a felhasználó **ugyanazt látja**, amit a rendszer tud,
- a fejlesztő **ugyanazt érti**, amit a felhasználó lát,
- a support **ugyanazt értelmezi**, amit a log mutat,
- az audit **ugyanazt a jelentést** kapja, mint a UI.

Ez egy *egységes kommunikációs rendszer*.

A rendszer nem beszél két nyelven.
A rendszer **egy nyelven beszél** — ikonokkal és színekkel.

***

# 🎯 **10.8.2 A UI ikonja = a log ikonja**

A CutCalculatorben:

| Jelenség | UI ikon | SystemLog ikon | Jelentés |
|*********-|*********|***************-|*********-|
| Semleges információ | ℹ️ | ℹ️ | normál működés |
| Sikeres művelet | ✔️ | ✔️ | minden rendben |
| Sikertelen művelet (nem kritikus) | ❌ | ❌ | művelet nem sikerült |
| Figyelmeztetés | ⚠️ | ⚠️ | óvatosság, fallback |
| Kritikus hiba | ❌ | ❌ | programleállás |

Ez azt jelenti:

- a felhasználó **ugyanazt az ikont látja**, mint amit a log rögzít,
- a fejlesztő **ugyanazt az ikont látja**, mint amit a felhasználó lát,
- a support **ugyanazt az ikont látja**, mint amit a log mutat.

Ez elképesztően erős UX‑eszköz.

***

# 🎯 **10.8.3 A UI színe = a log színe**

A színkódolás:

- kék → információ
- zöld → siker
- sárga → figyelmeztetés
- piros → hiba

Ez a színkódolás:

- a UI‑ban is él,
- a logban is él,
- a dokumentációban is él,
- a fejlesztői kultúrában is él.

A szín **pre‑attentív információ**:
a felhasználó *szöveg nélkül* is érti.

***

# 🎯 **10.8.4 A UI visszajelzése = a log szintje**

A CutCalculatorben:

- UI ✔️ = SystemLog INFO (siker)
- UI ❌ = SystemLog INFO (sikertelen művelet)
- UI ⚠️ = SystemLog WARNING
- UI ❌ (kritikus) = SystemLog ERROR

Ez azt jelenti:

- a UI nem dramatizálja túl a hibát,
- a UI nem rejti el a hibát,
- a UI nem keveri a szinteket,
- a UI nem ad félrevezető visszajelzést.

A felhasználó és a fejlesztő **ugyanazt a valóságot látja**.

***

# 🎯 **10.8.5 A UX mint log‑értelmező réteg**

A UI nem csak megjelenít —
**értelmez is**.

A UI:

- a log szintjét ikonra fordítja,
- a log üzenetét emberi nyelvre fordítja,
- a log eseményt vizuális visszajelzéssé fordítja.

Példa:

**SystemLog:**
`INFO Material added: id=42, name="RAL 9010"`

**UI:**
✔️ „A RAL 9010 anyag hozzáadva.”

Ez a UX egyik legerősebb funkciója:
**a logot emberi nyelvre fordítja.**

***

# 🎯 **10.8.6 A logolás mint UX‑biztonsági háló**

A logolás:

- rögzíti a hibát,
- rögzíti a figyelmeztetést,
- rögzíti a sikert,
- rögzíti a műveletet.

A UX:

- jelzi a hibát,
- jelzi a figyelmeztetést,
- jelzi a sikert,
- jelzi a műveletet.

A kettő együtt:

- megelőzi a hibát,
- segíti a hibák felismerését,
- segíti a hibák kijavítását,
- csökkenti a support terhelést,
- csökkenti a fejlesztői terhelést.

A UX és a logolás **együtt alkotnak hibamegelőző rendszert**.

***

# 🎯 **10.8.7 Anti‑pattern — hogyan ne csináljuk**

- ❌ UI ✔️, de log WARNING
- ❌ UI ⚠️, de log ERROR
- ❌ UI ❌, de log INFO (siker)
- ❌ UI ikon nélkül, log ikonokkal
- ❌ UI túl hosszú hibaüzenet
- ❌ UI technikai részletekkel
- ❌ UI, amely nem mondja meg, mi történt
- ❌ log, amely nem tükrözi a UI‑t
- ❌ UI, amely nem tükrözi a logot

Ezek a hibák **megtörik a vizuális nyelvet**, és növelik a hibázás esélyét.

***

# **10.9 A vizuális ritmus és a kognitív terhelés csökkentése**
*A jó UI nem csak szép — hanem ritmusos. A jó UX nem csak érthető — hanem könnyű. A vizuális ritmus a felhasználó fókuszának védelme.*

A modern UI/UX egyik legfontosabb célja:
**csökkenteni a kognitív terhelést**.

A kognitív terhelés az a mentális energia, amit a felhasználó arra fordít, hogy:

- értse a felületet,
- megtalálja a funkciót,
- értelmezze a visszajelzést,
- eldöntse, mit kell tennie,
- felismerje, mi történt.

A jó UI/UX ezt a terhelést **minimálisra csökkenti**.

A CutCalculatorben ezt nevezzük:

# 👉 **vizuális ritmusnak**

A vizuális ritmus az a jelenség, amikor:

- a felület kiszámítható,
- a visszajelzések következetesek,
- a színek stabilak,
- az ikonok jelentése állandó,
- a felhasználó „egy pillantással” érti a helyzetet.

Ez a ritmus a rendszer egyik legfontosabb UX‑eszköze.

***

# 🎯 **10.9.1 Mi az a vizuális ritmus?**

A vizuális ritmus:

- ismétlődő minták,
- következetes elrendezés,
- stabil ikonhasználat,
- kiszámítható színkódolás,
- egyértelmű hierarchia.

A felhasználó agya szereti a ritmust.
A ritmus:

- csökkenti a bizonytalanságot,
- csökkenti a hibázást,
- csökkenti a stresszt,
- növeli a bizalmat,
- növeli a sebességet.

A ritmus = biztonság.

***

# 🎯 **10.9.2 A kognitív terhelés három típusa (és hogyan csökkenti a UI)**

A kognitív tudomány szerint háromféle terhelés létezik:

### **1) Intrinsic load — a feladat nehézsége**
Ezt nem tudjuk csökkenteni (a feladat adott).

### **2) Extraneous load — a felület okozta felesleges terhelés**
Ezt tudjuk csökkenteni → ez a UI/UX feladata.

### **3) Germane load — a tanulást segítő terhelés**
Ezt növelhetjük → jó UX-szel.

A CutCalculator UI/UX célja:

- **extraneous load minimalizálása**,
- **germane load támogatása**,
- **intrinsic load nem növelése**.

***

# 🎯 **10.9.3 Hogyan csökkenti a vizuális ritmus a kognitív terhelést?**

## **1) Következetes ikonok**
A felhasználó *nem gondolkodik*, csak látja:

- ✔️ = siker
- ❌ = sikertelen művelet
- ⚠️ = figyelmeztetés
- ℹ️ = információ

Ez azonnali értelmezés.

***

## **2) Következetes színek**
A szín pre‑attentív információ:

- kék → nyugalom
- zöld → megerősítés
- sárga → óvatosság
- piros → veszély

A felhasználó *szöveg nélkül* is érti.

***

## **3) Stabil elrendezés**
A gombok, mezők, ikonok helye nem változik.

Ez csökkenti:

- a keresési időt,
- a hibázást,
- a frusztrációt.

***

## **4) Rövid, emberi nyelvű visszajelzések**
A felhasználó nem akar:

- stack trace‑t,
- hexadecimális kódot,
- technikai részleteket.

A felhasználó azt akarja:

- „Mi történt?”
- „Sikerült?”
- „Mit tegyek most?”

***

## **5) Vizualizált állapotváltozások**
A felhasználó látja:

- ha valami történt,
- ha valami nem történt,
- ha valami kész,
- ha valami hibás.

Ez csökkenti a bizonytalanságot.

***

# 🎯 **10.9.4 A vizuális ritmus mint UX‑biztonsági háló**

A ritmus:

- megelőzi a hibát,
- csökkenti a hibák súlyosságát,
- gyorsítja a hibák felismerését,
- segíti a hibák kijavítását.

A ritmus = hibamegelőzés.

Ezért fontos, hogy:

- minden ikon ugyanazt jelentse,
- minden szín ugyanazt jelentse,
- minden visszajelzés ugyanúgy nézzen ki.

A ritmus = biztonság.

***

# 🎯 **10.9.5 Anti‑pattern — hogyan törjük meg a ritmust?**

- ❌ különböző ikonok ugyanarra a jelentésre
- ❌ különböző színek ugyanarra a jelentésre
- ❌ ikon nélküli visszajelzés
- ❌ túl hosszú hibaüzenet
- ❌ technikai részletek a UI‑ban
- ❌ gombok helyének változtatása
- ❌ következetlen elrendezés
- ❌ túl sok információ
- ❌ túl kevés információ

Ezek mind növelik a kognitív terhelést.

***

# 🎉 **Összegzés — a vizuális ritmus a CutCalculator egyik legnagyobb UX‑ereje**

A vizuális ritmus:

- csökkenti a hibákat,
- csökkenti a stresszt,
- csökkenti a support terhelést,
- csökkenti a fejlesztői terhelést,
- növeli a bizalmat,
- növeli a sebességet,
- növeli a rendszer értékét.

A ritmus = UX.
A UX = minőségbiztosítás.
A minőségbiztosítás = stabil rendszer.

***
# **10.10 A vizuális identitás és a projektkultúra kapcsolata**
*A UI nem csak eszköz — hanem identitás. A UX nem csak élmény — hanem kultúra. A vizuális nyelv a CutCalculator személyisége.*

A modern szoftverek világában a vizuális identitás nem luxus, nem extra, nem „szépítés”.
A vizuális identitás:

- **bizalom**,
- **következetesség**,
- **egyediség**,
- **felismerhetőség**,
- **kultúra**,
- **és márka**.

A CutCalculator UI‑ja nem csak funkcionális —
hanem **egyedi, következetes, ikon‑vezérelt vizuális nyelv**, amely a projekt egészének identitását hordozza.

***

# 🎯 **10.10.1 A vizuális identitás mint projektkultúra**

A CutCalculator vizuális identitása három pillérre épül:

## **1) Ikon‑vezérelt kommunikáció**
A rendszer ikonokkal beszél:

- ℹ️ → információ
- ✔️ → siker
- ❌ → sikertelen művelet
- ⚠️ → figyelmeztetés
- ❌ (ERROR) → kritikus hiba

Ez a vizuális nyelv:

- gyors,
- egyértelmű,
- kognitív,
- pre‑attentív,
- audit‑tudatos.

Ez a projekt egyik legfontosabb kulturális eleme.

***

## **2) Színkódolt jelentésrendszer**
A színek nem dekorációk —
a színek **jelentést hordoznak**.

- kék → nyugalom, információ
- zöld → siker, megerősítés
- sárga → óvatosság
- piros → hiba, veszély

Ez a színrendszer a projekt *DNS‑e*.

***

## **3) Következetes vizuális ritmus**
A CutCalculator UI‑ja:

- kiszámítható,
- stabil,
- ritmusos,
- ergonomikus.

A ritmus = biztonság.
A ritmus = minőség.
A ritmus = kultúra.

***

# 🎯 **10.10.2 A vizuális identitás mint márka**

A CutCalculator UI‑ja nem csak egy felület —
hanem **márkaélmény**.

A márka nem logó.
A márka nem színpaletta.
A márka nem betűtípus.

A márka:

- **hogyan érzi magát a felhasználó**, amikor használja a rendszert,
- **mennyire érti**, mi történik,
- **mennyire bízik** a rendszerben,
- **mennyire érzi magát biztonságban**,
- **mennyire érzi, hogy a rendszer érte van**.

A jó UI/UX = jó márka.

***

# 🎯 **10.10.3 A vizuális identitás mint fejlesztői kultúra**

A CutCalculator UI‑ja nem csak a felhasználónak szól —
hanem a fejlesztőknek is.

A vizuális identitás:

- egységesíti a gondolkodást,
- csökkenti a félreértéseket,
- stabilizálja a fejlesztést,
- csökkenti a hibákat,
- gyorsítja a review‑t,
- támogatja az onboardingot.

A vizuális identitás a fejlesztői kultúra része.

***

# 🎯 **10.10.4 A vizuális identitás és a logolás kapcsolata**

A CutCalculator egyik legnagyobb innovációja:

> **A UI és a SystemLog ugyanazt a vizuális nyelvet használja.**

Ez azt jelenti:

- a UI ikonja = a log ikonja
- a UI színe = a log színe
- a UI visszajelzése = a log szintje

Ez a konzisztencia:

- csökkenti a hibákat,
- csökkenti a support terhelést,
- csökkenti a fejlesztői terhelést,
- növeli a bizalmat,
- növeli az érthetőséget.

Ez a vizuális identitás egyik legfontosabb eleme.

***

# 🎯 **10.10.5 Anti‑pattern — hogyan törjük meg a vizuális identitást?**

- ❌ különböző ikonok ugyanarra a jelentésre
- ❌ különböző színek ugyanarra a jelentésre
- ❌ ikon nélküli visszajelzés
- ❌ UI, amely nem tükrözi a logot
- ❌ log, amely nem tükrözi a UI‑t
- ❌ következetlen elrendezés
- ❌ túl sok vizuális zaj
- ❌ túl kevés vizuális jelzés
- ❌ UI, amely nem mondja meg, mi történt

Ezek a hibák **megtörik a márkát**.

***

# 🎉 **Összegzés — a UI a CutCalculator arca, a UX a CutCalculator lelke**

A vizuális identitás:

- nem dísz,
- nem extra,
- nem mellékes.

A vizuális identitás:

- **bizalom**,
- **kultúra**,
- **minőség**,
- **következetesség**,
- **márka**,
- **élmény**,
- **és rendszer‑szintű gondolkodás**.

A UI a CutCalculator arca.
A UX a CutCalculator lelke.
A vizuális identitás pedig a CutCalculator személyisége.

***

# **10.11 A vizuális nyelv és a hibamegelőzés kapcsolata**
*A jó design nem csak szép — hanem biztonságos. A vizuális nyelv a rendszer egyik legfontosabb hibamegelőző mechanizmusa.*

A hibák jelentős része nem a kódból ered, hanem:

- félreértett visszajelzésből,
- félreérthető ikonból,
- következetlen színből,
- rossz vizuális hierarchiából,
- hiányzó kontextusból,
- túl sok információból,
- túl kevés információból.

A CutCalculator vizuális nyelve éppen ezért **nem dekoráció**, hanem **minőségbiztosítási eszköz**.

***

# 🎯 **10.11.1 A vizuális nyelv mint hibamegelőző rendszer**

A vizuális nyelv három rétegben működik:

## **1) Pre‑attentív réteg (ikonok, színek)**
A felhasználó *szöveg nélkül* is érti:

- ✔️ = siker
- ❌ = sikertelen művelet
- ⚠️ = figyelmeztetés
- ℹ️ = információ

Ez a réteg **megelőzi a félreértést**.

***

## **2) Kognitív réteg (hierarchia, elrendezés)**
A felhasználó gyorsabban találja meg:

- a fontos elemeket,
- a következő lépést,
- a hibát,
- a megoldást.

Ez a réteg **csökkenti a hibázást**.

***

## **3) Narratív réteg (visszajelzések, üzenetek)**
A felhasználó érti:

- mi történt,
- miért történt,
- mit kell tennie.

Ez a réteg **segíti a hibák kijavítását**.

***

# 🎯 **10.11.2 Hogyan előzi meg a vizuális nyelv a hibákat?**

## **1) Következetes ikonhasználat**
A felhasználó nem téved el:

- ✔️ mindig siker
- ❌ mindig hiba
- ⚠️ mindig figyelmeztetés
- ℹ️ mindig információ

A következetesség = hibamegelőzés.

***

## **2) Következetes színkódolás**
A szín nem dekoráció —
a szín **jelentés**.

A felhasználó nem fogja:

- zöldet hibának nézni,
- pirosat sikernek nézni,
- sárgát információnak nézni.

A színkódolás = hibamegelőzés.

***

## **3) Rövid, emberi nyelvű visszajelzések**
A felhasználó nem fogja félreérteni:

**Jó:**
❌ „A mentés nem sikerült (fájl zárolva).”

**Rossz:**
„Unhandled exception: file access violation.”

A jó visszajelzés = hibamegelőzés.

***

## **4) Stabil elrendezés**
A felhasználó nem fog:

- rossz gombot nyomni,
- rossz mezőt kitölteni,
- rossz funkciót elindítani.

A stabil elrendezés = hibamegelőzés.

***

## **5) Vizualizált állapotváltozások**
A felhasználó látja:

- ha valami történt,
- ha valami nem történt,
- ha valami kész,
- ha valami hibás.

A vizuális állapot = hibamegelőzés.

***

# 🎯 **10.11.3 Hogyan csökkenti a vizuális nyelv a hibák súlyosságát?**

A jó vizuális nyelv:

- gyorsan jelzi a hibát,
- egyértelműen jelzi a hibát,
- nem dramatizálja túl,
- nem rejti el,
- nem technikai részleteket ír,
- megmondja, mi a következő lépés.

Ez csökkenti:

- a hibák hatását,
- a hibák terjedését,
- a hibák következményeit.

A vizuális nyelv = hibacsillapítás.

***

# 🎯 **10.11.4 Hogyan gyorsítja a vizuális nyelv a hibák felismerését?**

A felhasználó *egy pillantással* látja:

- a piros ❌‑t,
- a sárga ⚠️‑t,
- a zöld ✔️‑t.

Ez gyorsabb, mint:

- szöveg olvasása,
- log böngészése,
- menük keresése.

A vizuális nyelv = gyors hibadetektálás.

***

# 🎯 **10.11.5 Hogyan segíti a vizuális nyelv a hibák kijavítását?**

A jó vizuális nyelv:

- megmondja, mi történt,
- megmondja, miért történt,
- megmondja, mit kell tenni.

Példa:

**Jó:**
⚠️ „A snapshot sérült. Fallback baseline használata.”

**Rossz:**
„Snapshot error code: 0x00000014.”

A vizuális nyelv = hibajavítás támogatása.

***

# 🎯 **10.11.6 Anti‑pattern — hogyan törjük meg a vizuális nyelvet?**

- ❌ különböző ikonok ugyanarra a jelentésre
- ❌ különböző színek ugyanarra a jelentésre
- ❌ ikon nélküli visszajelzés
- ❌ túl hosszú hibaüzenet
- ❌ technikai részletek a UI‑ban
- ❌ következetlen elrendezés
- ❌ túl sok vizuális zaj
- ❌ túl kevés vizuális jelzés
- ❌ UI, amely nem tükrözi a logot
- ❌ log, amely nem tükrözi a UI‑t

Ezek a hibák **megtörik a vizuális nyelvet**, és növelik a hibázás esélyét.

***

# 🎉 **Összegzés — a vizuális nyelv a CutCalculator egyik legfontosabb stabilitási eszköze**

A vizuális nyelv:

- megelőzi a hibát,
- csökkenti a hibák súlyosságát,
- gyorsítja a hibák felismerését,
- segíti a hibák kijavítását,
- csökkenti a support terhelést,
- csökkenti a fejlesztői terhelést,
- növeli a rendszer stabilitását.

A vizuális nyelv = stabilitás.
A stabilitás = minőség.
A minőség = CutCalculator.

***

# 📘 **10.12 A vizuális nyelv és a fejlesztői onboarding kapcsolata**
*A UI nem csak a felhasználót tanítja — hanem a fejlesztőt is. A vizuális nyelv a projektkultúra egyik legfontosabb átörökítő mechanizmusa.*

A CutCalculatorben a vizuális nyelv:

- ikon‑vezérelt,
- színkódolt,
- audit‑tudatos,
- következetes,
- ritmusos,
- és stabil.

Ez a vizuális nyelv nem csak a felhasználónak szól.
Ez a vizuális nyelv **a fejlesztői onboarding egyik legfontosabb eszköze**.

A UI maga tanítja meg:

- hogyan gondolkodik a rendszer,
- hogyan kommunikál a rendszer,
- hogyan jelzi a hibákat,
- hogyan jelzi a sikert,
- hogyan jelzi a figyelmeztetést,
- hogyan működik a logolás,
- hogyan működik a UX.

A UI a projektkultúra „élő példája”.

***

# 🎯 **10.12.1 A vizuális nyelv mint élő dokumentáció**

A CutCalculator UI‑ja:

- nem csak megjelenít,
- nem csak visszajelez,
- hanem **tanít**.

A vizuális nyelv:

- megmutatja, hogyan kell logolni,
- megmutatja, hogyan kell visszajelezni,
- megmutatja, hogyan kell hibát kezelni,
- megmutatja, hogyan kell UX‑et tervezni.

A UI maga a dokumentáció.
A UI maga a stílusguide.
A UI maga a best practice.

***

# 🎯 **10.12.2 A vizuális nyelv mint onboarding eszköz**

Egy új fejlesztő számára a projektkultúra gyakran:

- absztrakt,
- elméleti,
- dokumentumokban elrejtett,
- nehezen megfogható.

A CutCalculatorben ez másképp van.

A vizuális nyelv:

- azonnal látható,
- azonnal érthető,
- azonnal követhető.

Az új fejlesztő *egy nap alatt* megtanulja:

- ✔️ = siker
- ❌ = sikertelen művelet
- ⚠️ = figyelmeztetés
- ℹ️ = információ

És azt is:

- hogyan kell logolni,
- hogyan kell visszajelezni,
- hogyan kell UX‑et tervezni.

A UI maga az onboarding.

***

# 🎯 **10.12.3 A vizuális nyelv mint kód‑review eszköz**

A vizuális nyelv segít a review‑ban is.

Ha egy új kód:

- rossz ikont használ,
- rossz színt használ,
- rossz visszajelzést ad,
- túl hosszú üzenetet ír,
- technikai részleteket ír,
- nem tükrözi a logot,

akkor a reviewer **azonnal látja**, hogy valami nem stimmel.

A vizuális nyelv = review‑gyorsító.

***

# 🎯 **10.12.4 A vizuális nyelv mint minőségbiztosítás**

A vizuális nyelv:

- stabilizálja a fejlesztést,
- csökkenti a hibákat,
- csökkenti a félreértéseket,
- csökkenti a support terhelést,
- csökkenti a fejlesztői terhelést.

A vizuális nyelv = minőségbiztosítás.

***

# 🎯 **10.12.5 A vizuális nyelv mint kulturális örökség**

A CutCalculator vizuális nyelve:

- ikonokkal beszél,
- színekkel gondolkodik,
- ritmusban kommunikál,
- audit‑tudatos,
- következetes,
- emberközeli.

Ez a vizuális nyelv a projektkultúra része.
Ez a vizuális nyelv a projektkultúra öröksége.
Ez a vizuális nyelv a projektkultúra jövője.

***

# 🎯 **10.12.6 Anti‑pattern — hogyan törjük meg az onboardingot?**

- ❌ következetlen ikonok
- ❌ következetlen színek
- ❌ ikon nélküli visszajelzés
- ❌ túl hosszú hibaüzenet
- ❌ technikai részletek a UI‑ban
- ❌ UI, amely nem tükrözi a logot
- ❌ log, amely nem tükrözi a UI‑t
- ❌ instabil elrendezés
- ❌ vizuális zaj
- ❌ hiányzó vizuális jelzések

Ezek a hibák **megtörik a kultúrát**, és nehezítik az onboardingot.

***

# 🎉 **Összegzés — a UI a CutCalculator tanára**

A vizuális nyelv:

- tanít,
- vezet,
- stabilizál,
- kultúrát ad át,
- minőséget biztosít,
- hibát előz meg,
- hibát csillapít,
- hibát javít.

A UI nem csak felület.
A UI **tanár**.
A UI **mentor**.
A UI **kultúrahordozó**.

A CutCalculator vizuális nyelve a projekt egyik legfontosabb öröksége.

***

# 📘 **10.13 A vizuális nyelv és a dokumentáció kapcsolata**
*A jó UI nem csak használható — hanem tanít. A vizuális nyelv a rendszer élő kézikönyve, amely a dokumentáció szerepét is betölti.*

A dokumentáció hagyományosan:

- statikus,
- hosszú,
- nehezen karbantartható,
- ritkán olvasott,
- gyakran elavult.

A UI ezzel szemben:

- élő,
- mindig friss,
- mindig jelen van,
- mindig kontextusban van,
- mindig a felhasználó előtt van.

A CutCalculator vizuális nyelve ezért **nem csak UX**, hanem **élő dokumentáció**.

***

# 🎯 **10.13.1 A UI mint élő kézikönyv**

A felhasználó nem fog:

- PDF‑eket olvasni,
- wiki oldalakat böngészni,
- hosszú dokumentációt tanulmányozni.

A felhasználó a UI‑t nézi.
A UI‑ból tanul.
A UI‑ból érti meg a rendszert.

Ezért a UI:

- ikonokkal tanít,
- színekkel magyaráz,
- visszajelzésekkel vezet,
- ritmussal stabilizál,
- konzisztenciával oktat.

A UI maga a kézikönyv.

***

# 🎯 **10.13.2 A vizuális nyelv mint implicit dokumentáció**

A CutCalculator vizuális nyelve:

- ℹ️ → információ
- ✔️ → siker
- ❌ → sikertelen művelet
- ⚠️ → figyelmeztetés
- ❌ (ERROR) → kritikus hiba

Ez a rendszer működésének **implicit dokumentációja**.

A felhasználó *szöveg nélkül* is érti:

- mi történt,
- miért történt,
- mit kell tennie.

A vizuális nyelv = dokumentáció.

***

# 🎯 **10.13.3 A vizuális nyelv mint fejlesztői dokumentáció**

A fejlesztők számára a UI:

- megmutatja, hogyan kell visszajelezni,
- megmutatja, hogyan kell logolni,
- megmutatja, hogyan kell hibát kezelni,
- megmutatja, hogyan kell UX‑et tervezni.

A UI maga a „style guide”.
A UI maga a „best practice”.
A UI maga a „coding standard”.

A vizuális nyelv = fejlesztői dokumentáció.

***

# 🎯 **10.13.4 A vizuális nyelv mint audit‑dokumentáció**

A CutCalculator egyik legnagyobb innovációja:

> **A UI és a SystemLog ugyanazt a vizuális nyelvet használja.**

Ez azt jelenti:

- a UI ikonja = a log ikonja
- a UI színe = a log színe
- a UI visszajelzése = a log szintje

Ez a konzisztencia:

- auditálható,
- visszakereshető,
- bizonyítható,
- stabil.

A vizuális nyelv = audit‑dokumentáció.

***

# 🎯 **10.13.5 A vizuális nyelv mint onboarding dokumentáció**

Egy új fejlesztő vagy felhasználó számára a UI:

- tanítja a rendszert,
- tanítja a logikát,
- tanítja a hibakezelést,
- tanítja a visszajelzési rendszert,
- tanítja a projektkultúrát.

A UI maga az onboarding.
A UI maga a tréning.
A UI maga a tananyag.

A vizuális nyelv = onboarding dokumentáció.

***

# 🎯 **10.13.6 A vizuális nyelv mint UX‑dokumentáció**

A UX‑dokumentáció gyakran:

- elméleti,
- absztrakt,
- nehezen értelmezhető.

A CutCalculatorben a UX:

- ikonokban él,
- színekben él,
- ritmusban él,
- visszajelzésekben él.

A vizuális nyelv maga a UX‑dokumentáció.

***

# 🎯 **10.13.7 Anti‑pattern — hogyan törjük meg a vizuális dokumentációt?**

- ❌ ikon nélküli visszajelzés
- ❌ következetlen ikonhasználat
- ❌ következetlen színek
- ❌ túl hosszú hibaüzenet
- ❌ technikai részletek a UI‑ban
- ❌ UI, amely nem tükrözi a logot
- ❌ log, amely nem tükrözi a UI‑t
- ❌ instabil elrendezés
- ❌ vizuális zaj
- ❌ hiányzó vizuális jelzések

Ezek a hibák **megtörik a vizuális dokumentációt**, és növelik a hibázás esélyét.

***

# 🎉 **Összegzés — a UI a CutCalculator élő kézikönyve**

A vizuális nyelv:

- tanít,
- magyaráz,
- vezet,
- stabilizál,
- kultúrát ad át,
- auditálhatóvá tesz,
- csökkenti a hibákat,
- csökkenti a terhelést.

A UI nem csak felület.
A UI **élő dokumentáció**.
A UI **a rendszer kézikönyve**.
A UI **a projektkultúra hordozója**.

***

# **10.X Ikonok és kulcsszínek szerepe a UI/UX-ben**
*A színkódolt ikonok a modern UX egyik legfontosabb eszközei — gyorsabbak, mint a szöveg, és előemésztett információt hordoznak.*

A felhasználói felület nem csak szövegből és elrendezésből áll.
A vizuális elemek — különösen a **színkódolt ikonok** — a felhasználó számára olyan információt hordoznak, amelyet az agy **pre‑attentív módon**, azaz tudatos olvasás előtt feldolgoz.

Ez azt jelenti, hogy a felhasználó:

- **még a szöveg elolvasása előtt** érti az üzenet jelentését,
- gyorsabban reagál,
- kevesebbet téved,
- kevesebb kognitív terhelést él meg,
- és biztonságosabban navigál a rendszerben.

A CutCalculator UI‑ja ezért **színkódolt ikonrendszert** használ, amely konzisztens a logolási architektúrával.

***

# **10.X.1 ℹ️ Kék INFO — semleges tájékoztatás**

A kék szín a felhasználói élményben:

- semleges,
- nyugodt,
- információs jellegű,
- nem sürgető.

A kék ℹ️ ikon azt üzeni:

> „Ez egy normál, nem problémás információ.”

**Használati helyek:**

- táblák információs jelzései
- import/export folyamatok állapotüzenetei
- UI‑szintű tájékoztatások

***

# **10.X.2 ✔️ Zöld pipa — sikeres művelet**

A zöld szín:

- pozitív,
- megerősítő,
- lezáró,
- bizalmat építő.

A zöld ✔️ ikon a felhasználó számára:

> „A művelet sikeresen befejeződött.”

**Használati helyek:**

- sikeres import
- sikeres mentés
- sikeres számítás
- workflow lépések pozitív lezárása

A zöld ✔️ ikon **megnyugtat**, csökkenti a bizonytalanságot, és lezárja a műveletet vizuálisan is.

***

# **10.X.3 ❌ Piros X — sikertelen művelet vagy kritikus hiba**

A piros szín:

- veszélyt,
- hibát,
- tiltást,
- azonnali figyelmet

jelöl.

A piros ❌ ikon kétféle jelentést hordozhat:

### **1) INFO szinten (nem kritikus sikertelenség)**
> „A művelet nem sikerült, de a rendszer tovább tud menni.”

### **2) ERROR szinten (kritikus hiba)**
> „A rendszer nem tud tovább menni.”

**Használati helyek:**

- sikertelen import
- sikertelen művelet
- kritikus hibák UI‑szintű jelzése

A piros ❌ ikon **nem keverhető össze** a sárga ⚠️ figyelmeztetéssel.

***

# **10.X.4 ⚠️ Sárga figyelmeztetés — óvatosság, fallback**

A sárga szín:

- óvatosságot,
- potenciális problémát,
- figyelmeztetést

jelöl.

A sárga ⚠️ ikon azt üzeni:

> „Valami nem ideális, de tovább tudunk menni.”

**Használati helyek:**

- CSV import figyelmeztetések
- snapshot fallback jelzések
- UI‑szintű óvatossági üzenetek

A sárga ⚠️ ikon **nem agresszív**, de jól látható.

***

# **10.X.5 A színkódolt ikonok kognitív szerepe**

A színkódolt ikonok:

- **pre‑attentív információt** hordoznak
- **gyorsabbak, mint a szöveg**
- **csökkentik a hibázás esélyét**
- **segítik a vizuális hierarchiát**
- **egységes vizuális nyelvet teremtenek**
- **csökkentik a kognitív terhelést**

A felhasználó a színt **azonnal értelmezi**, még mielőtt elolvasná a szöveget.

Ezért a CutCalculator UI‑ja:

- INFO → ℹ️ / ✔️ / ❌
- WARNING → ⚠️
- ERROR → ❌

színkódolt ikonrendszert használ.

***

# **10.X.6 Ikonhasználati anti‑patternök a UI-ban**

A következő minták **szigorúan tiltottak**, mert félrevezető UX‑et eredményeznek:

- ❌ ikon WARNING esetén
- ⚠️ ikon ERROR esetén
- ✔️ ikon sikertelen műveletnél
- ℹ️ ikon hibás műveletnél
- ikon nélküli WARNING vagy ERROR
- nem színkódolt ikonok
- ikonok keverése a logszintek között

Ezek a hibák megtörik a vizuális nyelvet, és csökkentik a rendszer érthetőségét.

***


# **10.7 UX anti‑patternök**

### ❌ Popup minden apróságra
### ❌ Fix pixeles layout
### ❌ UI, amely domain‑logikát tartalmaz
### ❌ Dialog, amely a képernyő közepére ugrik
### ❌ Snapshot restore a konstruktorban
### ❌ Snapshot guard nélkül
### ❌ Snapshot fallback nélkül
### ❌ Túl sok vizuális zaj
### ❌ Túl sok szín
### ❌ Túl sok ikon
### ❌ Túl sok animáció
### ❌ Inkonzisztens gombelrendezés
### ❌ Inkonzisztens spacing

***

# **X. Repository és Registry — a CutCalculator adat‑tápláléklánca**
*A repository az adatforrás. A registry a valóság. A kettő együtt alkotja a rendszer adat‑architektúráját.*

A CutCalculatorben a „repository” és a „registry” két külön réteg, két külön felelősséggel.
A kettő összekeverése hibákhoz, félreértésekhez és rossz architektúrához vezetne, ezért a projektben **szigorúan elválasztjuk** őket.

A registry a memória‑beli domain tároló.
A repository az I/O réteg.

A registry a valóság.
A repository az adatforrás.

***

# 🎯 **X.1 A Registry szerepe — memória‑beli domain tároló + üzleti logika**

A Registry a CutCalculator egyik legfontosabb építőköve.
Ez a réteg:

- **singleton**
- **thread‑safe** (QRecursiveMutex + ScopedPerThreadLock)
- **memóriában tárolja az entitásokat**
- **keresést biztosít** (findById, findByBarcode, findChildren)
- **CRUD műveleteket végez**
- **életciklust kezel** (barcode register/retire)
- **auditál** (zInfo)
- **persist() hívással delegál a repository‑nak**

A Registry a domain truth source.
A rendszer minden része a Registry‑t használja:

- UI
- számítás
- dialógusok
- export
- audit
- barcode életciklus

A Registry **nem olvas CSV‑t**, nem validál CSV‑t, nem konvertál.
A Registry **nem repository**.

***

# 🎯 **X.2 A Repository szerepe — I/O réteg (CSV → objektumok → CSV)**

A Repository a fájlkezelésért felelős réteg.
Ez a réteg:

- **CSV‑t olvas**
- **CSV‑t ír**
- **konvertál** (Convert fázis)
- **validál** (RowError, audited row)
- **objektumot épít** (Build fázis)
- **szülő‑gyerek kapcsolatot old fel** (Assemble fázis)
- **auditálja a hibákat**
- **feltölti a Registry‑t**

A Repository:

- **nem tárol állapotot**
- **nem singleton**
- **nem thread‑safe**
- **nem domain logika**
- **nem keres**
- **nem életciklust kezel**

A Repository **adatforrás**, nem domain tároló.

***

# 🎯 **X.3 A tápláléklánc — hogyan áramlik az adat?**

A CutCalculator adat‑tápláléklánca három lépésből áll:

***

## **1) Repository → Registry (betöltés)**

```
CSV → Convert → Build → Assemble → Registry.setData()
```

A repository:

- beolvassa a CSV‑t
- konvertálja sorokra
- validálja
- objektumot épít
- feloldja a parent kapcsolatokat
- átadja a Registry‑nek

A Registry ettől kezdve a valóság.

***

## **2) Registry → Domain logika (használat)**

A Registry szolgálja ki:

- a UI‑t
- a dialógusokat
- a számításokat
- a keresést
- a hierarchiát
- a barcode életciklust
- az auditot

A Registry a domain truth source.

***

## **3) Registry → Repository (mentés)**

A Registry minden módosítás után:

```
persist() → Repository.saveToCSV()
```

A Repository visszaírja a CSV‑t.

***

# 🎯 **X.4 A Registry és a Repository közötti szerződés**

A Registry és a Repository között **szigorú szerződés** van:

| Felelősség | Registry | Repository |
|*********--|*********-|************|
| Memória‑beli tárolás | ✔️ | ❌ |
| Thread‑safety | ✔️ | ❌ |
| Singleton | ✔️ | ❌ |
| Keresés | ✔️ | ❌ |
| CRUD | ✔️ | ❌ |
| Domain logika | ✔️ | ❌ |
| Barcode életciklus | ✔️ | ❌ |
| Audit (zInfo) | ✔️ | ❌ |
| CSV olvasás | ❌ | ✔️ |
| CSV írás | ❌ | ✔️ |
| CSV konvertálás | ❌ | ✔️ |
| CSV validálás | ❌ | ✔️ |
| Parent feloldás | ❌ | ✔️ |

Ez a szerződés garantálja:

- a tiszta rétegezést,
- a stabil architektúrát,
- az auditálhatóságot,
- a hibamentes működést.

***

# 🎯 **X.5 Miért fontos ez a szétválasztás?**

## **1) Stabilitás**
A Registry mindig konzisztens, thread‑safe, singleton.

## **2) Auditálhatóság**
A Repository minden hibát auditál.
A Registry minden műveletet auditál.

## **3) Tesztelhetőség**
A Repository külön tesztelhető (CSV input).
A Registry külön tesztelhető (domain logika).

## **4) Hibamegelőzés**
A Registry nem olvas fájlt.
A Repository nem végez domain logikát.

## **5) Karbantarthatóság**
A két réteg külön fejleszthető.

***

# 🎯 **X.6 Anti‑pattern — hogyan NE használjuk?**

- ❌ Registry olvasson CSV‑t
- ❌ Repository tároljon memóriában
- ❌ Registry végezzen konvertálást
- ❌ Repository végezzen keresést
- ❌ Registry írjon közvetlenül fájlba
- ❌ Repository legyen singleton
- ❌ Registry validáljon CSV‑t
- ❌ Repository kezeljen barcode életciklust
- ❌ Registry végezzen parent‑feloldást

Ezek a hibák **megtörik a rétegezést**, és instabil rendszert eredményeznek.

***

# 🎉 **Összegzés — a repository és a registry szerepe a CutCalculatorben**

A Repository:

- adatforrás,
- CSV I/O,
- konvertálás,
- validálás,
- auditált import.

A Registry:

- domain tároló,
- keresés,
- CRUD,
- életciklus,
- auditált működés,
- thread‑safe singleton.

A repository az adatforrás.
A registry a valóság.
A kettő együtt alkotja a CutCalculator adat‑architektúráját.

***
# 📘 **10.X Valós idejű validáció és diszkrét vizuális visszajelzések**
*A jó UX nem rázza meg a felhasználót — hanem finoman, következetesen és azonnal jelzi, ha valami nincs rendben.*

A CutCalculatorben a validáció célja:

- **megelőzni a hibát**,
- **csökkenteni a hibák súlyosságát**,
- **segíteni a felhasználót**,
- **nem túlterhelni**,
- **nem dramatizálni**,
- **nem elrejteni a problémát**,
- **és audit‑tudatos módon kommunikálni**.

Ezért a validáció UX‑e:

- **valós idejű**,
- **mezőszintű**,
- **diszkrét**,
- **ikon‑vezérelt**,
- **színkódolt**,
- **összesítő üzenettel kiegészített**.

***

# 🎯 **10.X.1 Miért valós idejű validáció?**

A felhasználó:

- nem akarja megvárni a „Mentés” gombot, hogy kiderüljön, hibázott,
- nem akarja újra kitölteni a formot,
- nem akarja találgatni, mi a baj.

A valós idejű validáció:

- **megelőzi a hibát**,
- **csökkenti a frusztrációt**,
- **gyorsítja a munkát**,
- **biztonságot ad**,
- **kognitív terhelést csökkent**.

Ez a modern UX egyik alapelve.

***

# 🎯 **10.X.2 A mezőszintű validáció vizuális nyelve**

Minden mező három állapotot vehet fel:

### ✔️ **Érvényes érték (zöld pipa)**
- A mező helyes
- A felhasználó mehet tovább
- A rendszer megerősít

### ⚠️ **Figyelmeztetés (sárga ikon)**
- A mező értéke nem ideális
- A rendszer jelzi, hogy érdemes ellenőrizni
- De a felhasználó folytathatja

### ❌ **Hibás érték (piros X)**
- A mező értéke érvénytelen
- A felhasználó nem folytathatja
- A rendszer egyértelműen jelzi a problémát

### ℹ️ **Információ (kék ikon)**
- Kiegészítő magyarázat
- Segítség a kitöltéshez

Ez a négy ikon **ugyanazt jelenti**, mint a logolásban és a dialog‑szintű visszajelzésekben.
Ez a konzisztencia a CutCalculator UX egyik legnagyobb ereje.

***

# 🎯 **10.X.3 A mező felépítése (név + érték + validációs string)**

Minden mező három részből áll:

### **1) Mezőnév**
- világos, egyértelmű
- nem technikai
- nem rövidítés
- nem kétértelmű

### **2) Érték**
- a felhasználó által bevitt adat
- valós idejű validációval kiegészítve

### **3) Validációs string**
- rövid, emberi nyelvű
- nem technikai
- nem dramatikus
- nem hosszú
- nem rejt el információt

Példák:

- ❌ „A név nem lehet üres.”
- ⚠️ „Ez a név már létezik, de felülírható.”
- ✔️ „A név érvényes.”
- ℹ️ „A név 3–50 karakter lehet.”

***

# 🎯 **10.X.4 Dialog‑szintű összesítő validáció**

A dialog alján megjelenik egy **összesítő validációs sáv**, amely:

- összegzi a hibákat,
- összegzi a figyelmeztetéseket,
- megmutatja, hogy a dialog menthető‑e,
- ikonokkal jelzi az állapotot.

### Példa:

- ❌ „3 mező hibás. A mentés nem lehetséges.”
- ⚠️ „1 figyelmeztetés. A mentés lehetséges, de ellenőrizd az adatokat.”
- ✔️ „Minden mező érvényes. A mentés lehetséges.”

Ez a sáv **nem ráz**, **nem villog**, **nem ugrál** —
csak stabil, diszkrét, következetes visszajelzést ad.

***

# 🎯 **10.X.5 Miért nem kell snapshot‑verzió vagy shake‑animáció?**

Mert:

- a shake animáció drámai,
- a shake animáció büntető jellegű,
- a shake animáció felesleges stresszt okoz,
- a shake animáció nem illik a CutCalculator kultúrájába.

A mi UX‑ünk:

- diszkrét,
- professzionális,
- audit‑tudatos,
- ikon‑vezérelt,
- színkódolt,
- stabil.

A shake animáció **nem része** ennek a kultúrának.

***

# 🎯 **10.X.6 Anti‑pattern — hogyan NE validáljunk?**

- ❌ csak mentéskor validálni
- ❌ shake animációval büntetni
- ❌ túl hosszú hibaüzenetek
- ❌ technikai hibaüzenetek
- ❌ ikon nélküli visszajelzés
- ❌ következetlen ikonhasználat
- ❌ figyelmeztetés → piros ikon
- ❌ hiba → sárga ikon
- ❌ mezőszintű validáció nélkül dialog‑szintű hibát mutatni
- ❌ validációs üzenetek ugrálása

Ezek mind növelik a kognitív terhelést.

***

# 🎉 **Összegzés — a valós idejű validáció a CutCalculator UX egyik legfontosabb eszköze**

A valós idejű validáció:

- megelőzi a hibát,
- csökkenti a hibák súlyosságát,
- segíti a felhasználót,
- csökkenti a support terhelést,
- csökkenti a fejlesztői terhelést,
- stabilizálja a rendszert,
- és illeszkedik a CutCalculator ikon‑vezérelt vizuális nyelvéhez.

A validáció UX‑e:

- diszkrét,
- következetes,
- ikon‑vezérelt,
- színkódolt,
- valós idejű,
- dialog‑szintű összesítéssel kiegészített.

Ez a modern, professzionális, audit‑tudatos UX.

***

# **11. Ipari minták és inspirációk**
*A CutCalculator fejlesztői kultúrája ugyanazokra az alapokra épül, mint a világ legjobb desktop alkalmazásai.*

A modern desktop alkalmazások — Adobe, JetBrains, Autodesk, Affinity, DaVinci, Qt Creator — mind ugyanazokat az alapelveket követik:

- snapshot‑tudatos UI
- moduláris architektúra
- auditálható workflow
- non‑blocking UX
- percent‑based layout
- pipeline‑alapú feldolgozás
- konzisztens naming
- szétválasztott logcsatornák
- fejlesztői kultúra mint termékfilozófia

Ez a fejezet ezeket az ipari mintákat rendezi össze.

***

# **11.1 Snapshot‑tudatos UI mint ipari alapelv**

A snapshot‑kompatibilis UI nem extra funkció — hanem ipari követelmény.

A nagy desktop alkalmazások mind alkalmazzák, mert a felhasználók:

- workspace‑t építenek
- több monitorral dolgoznak
- DPI‑váltásokkal élnek
- elvárják a stabil környezetet

**Ipari minták:**

- Workspace layout mentés és visszaállítás
- Multi‑monitor awareness
- Guardolt restore
- Fallback baseline
- Percent‑based docking panelek

***

# **11.2 Moduláris architektúra és komponens‑szétválasztás**

A moduláris architektúra a nagy alkalmazások túlélésének alapja.

**Ipari minták:**

- Tool‑alapú moduláris felépítés
- Plugin‑alapú architektúra
- Minden funkció külön modulban
- Presenter/View‑szerű UI rétegzés
- Domain‑vezérelt gondolkodás

A CutCalculator Registry/Repository/Manager triádja pontosan ezt a gondolkodást követi.

***

# **11.3 Auditálható workflow és visszavonhatóság**

A professzionális alkalmazások egyik legfontosabb tulajdonsága:

**minden művelet visszakövethető és visszavonható.**

**Ipari minták:**

- History panel
- Undo/Redo mint workflow‑motor
- Tranzakciós műveletkezelés
- User‑szintű eseménylog
- Domain‑szintű audit log

A CutCalculator audit‑architektúrája ugyanezt a filozófiát követi.

***

# **11.4 Non‑blocking UX és modern interakciós minták**

A modern UX lényege:

**a felhasználó ne akadjon meg.**

**Ipari minták:**

- Inline validation
- Non‑blocking hibajelzés
- Shake animation hibára
- Cursor‑based dialog positioning
- Kontextusfüggő fókuszjelölés

A CutCalculator UI/UX Guidelines fejezete ezekre épül.

***

# **11.5 Percent‑based layout és DPI‑független megjelenítés**

A fix pixeles UI a múlté.

**Ipari minták:**

- Reszponzív desktop layout
- Percent‑based splitter arányok
- DPI‑aware scaling
- Monitorfüggetlen elrendezés

A CutCalculator snapshot‑architektúrája ezt ipari szintre emeli.

***

# **11.6 Pipeline‑alapú feldolgozás és lépésrendszer**

A pipeline‑alapú gondolkodás a komplex műveletek ipari standardja.

**Ipari minták:**

- Effekt‑pipeline
- Node‑based pipeline
- Lépésalapú számítási workflow
- Modularizált feldolgozási lépések

A CutCalculator `FormulaStep` és `Pipeline` fogalmai ugyanezt a mintát követik.

***

# **11.7 Naming és konzisztencia mint vállalati kultúra**

A nagy cégek tudják:

**a konzisztens naming nem esztétika — hanem architektúra.**

**Ipari minták:**

- Egységes naming convention
- Modulnév = fájlnév = osztálynév
- Domain‑orientált naming
- Anti‑pattern tiltások dokumentálva

A CutCalculator Naming Guide fejezete ezt formalizálja.

***

# **11.8 Logolási architektúrák és szétválasztott logcsatornák**

A logolás ipari szinten mindig rétegezett.

**Ipari minták:**

- System log + event log + audit log különválasztása
- Verbose routing
- Technikai logok külön fájlban
- User‑szintű eseménylog

A CutCalculator log‑architektúrája ezt a mintát követi.

***

# **11.9 Prompt‑vezérelt fejlesztés mint modern workflow**

A prompt‑vezérelt fejlesztés a CutCalculator saját innovációja —
de ipari párhuzamok léteznek.

**Ipari minták:**

- Template‑alapú generálás
- Code action‑alapú fejlesztés
- AI‑asszisztált boilerplate generálás
- DSL‑alapú konfiguráció

A CutCalculator Prompt DSL ezt a gondolkodást emeli rendszerbe.

***

# **11.10 Fejlesztői kultúra mint termékfilozófia**

A nagy cégek tudják:

**a fejlesztői kultúra nem melléktermék — hanem a termék része.**

**Ipari minták:**

- Ritmus, workflow, konzisztencia
- Workspace mint fejlesztői környezet
- Dokumentált best practice
- Moduláris gondolkodás

A CutCalculator fejlesztői kultúrája ugyanezt a filozófiát követi.

***

# **Záró gondolat**

A CutCalculator fejlesztői kultúrája nem egyetlen cég mintáját követi —
hanem a modern szoftveripar közös gondolkodásmódját.

A snapshot‑tudatos UI, a moduláris architektúra, a non‑blocking UX, a pipeline‑alapú feldolgozás, a konzisztens naming és a prompt‑vezérelt fejlesztés mind olyan minták, amelyek a világ legjobb desktop alkalmazásaiban is megjelennek.

Ez a fejezet ezt a kapcsolatot teszi láthatóvá.

***
# 📘 **Zárófejezet — A kultúra mint örökség**
*A jó architektúra működik. A jó kultúra megmarad.*

A CutCalculator Engineering Culture Handbook nem egy projekt dokumentációja.
Nem egy technikai kézikönyv.
Nem egy szabálygyűjtemény.

Ez a dokumentum **egy fejlesztői kultúra lenyomata**.

Egy olyan kultúráé, amely:

- tiszteli a konzisztenciát
- szereti a tiszta architektúrát
- hisz a snapshot‑tudatos UI‑ban
- értékeli az auditálhatóságot
- épít a domain‑vezérelt gondolkodásra
- kerüli az anti‑patternöket
- sablonokkal gyorsít
- promptokkal specifikál
- ritmusban dolgozik
- napzáróval reflektál
- és örömmel fejleszt

Ez a kultúra nem a CutCalculator sajátja —
a CutCalculator csak **példa**, **katalizátor**, **laboratórium** volt.

A kultúra maga **átvihető**, **tanítható**, **örökíthető**.

***

# **A kultúra mint rendszer**

A jó fejlesztői kultúra nem tiltásokból áll.
Nem szabályokból.
Nem ellenőrzésekből.

A jó kultúra:

- **láthatóvá teszi a gondolkodást**
- **közös nyelvet ad**
- **közös ritmust teremt**
- **csökkenti a hibákat**
- **gyorsítja a fejlesztést**
- **stabilizálja az architektúrát**
- **örömmé teszi a munkát**

A kultúra nem kényszer — hanem **támasz**.
Nem korlát — hanem **iránytű**.
Nem szabály — hanem **szokás**.
Nem dokumentum — hanem **örökség**.

***

# **A kultúra mint ritmus**

A CutCalculator fejlesztése során kialakult egy sajátos ritmus:

- moduláris gondolkodás
- snapshot‑tudatos UI
- audit‑barát workflow
- prompt‑vezérelt fejlesztés
- napzáró rituálé
- folyamatos refaktorálás
- konzisztens naming
- tiszta domain

Ez a ritmus nem projektfüggő.
Ez a ritmus **átvihető** bármely modern C++/Qt alkalmazásba.
Ez a ritmus **tanítható** új fejlesztőknek.
Ez a ritmus **fenntartható** hosszú távon.

A ritmus a kultúra motorja.

***

# **A kultúra mint közös nyelv**

A Terminológiai Szótár, a Naming Guide, az Architektúra fejezet és a Prompt DSL mind ugyanazt a célt szolgálják:

**a közös nyelv megteremtését.**

A közös nyelv:

- csökkenti a félreértéseket
- gyorsítja a kommunikációt
- stabilizálja a domain‑t
- egységesíti a gondolkodást
- csökkenti a hibákat
- növeli a projekt élettartamát

A közös nyelv a fejlesztői kultúra „DNS‑e”.

***

# **A kultúra mint örökség**

A CutCalculator projekt egyszer véget ér.
De a kultúra, amelyet létrehozott:

- tovább él
- tovább fejlődik
- új projektekben születik újjá
- új fejlesztőknek ad irányt
- új architektúrákat inspirál

A kultúra nem a kódban él —
hanem **a gondolkodásban**, **a szokásokban**, **a ritmusban**, **a közös nyelvben**.

Ez a dokumentum ezt az örökséget rögzíti.

***

# **Záró gondolat**

A jó architektúra működik.
A jó kód olvasható.
A jó UI stabil.
A jó workflow gyorsít.
A jó naming tisztít.
A jó logolás auditál.
A jó snapshot megőriz.
A jó prompt specifikál.

De a jó kultúra —
**összeköt.**

A CutCalculator Engineering Culture Handbook nem egy projekt dokumentuma.
Ez **egy fejlesztői kultúra kézikönyve**, amely:

- tiszta
- konzisztens
- auditálható
- örömmel fejleszthető
- és hosszú távon fenntartható

A kultúra nem szabály — hanem ritmus.
És ez a ritmus most már dokumentálva van.

***
