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

### 4.6 Logolási architektúra
- SystemLog
- EventLog
- Audit log
- Verbose routing

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

### 2.4 Logolási fogalmak

## Logcsatornák (CutCalculator)
A CutCalculator két logcsatornát használ:
**SystemLog** (technikai + audit) és **EventLog** (felhasználói események).
Az audit események nem külön logcsatornát alkotnak, hanem a SystemLog részei, INFO szinten.

### SystemLog (hiba log)
- technikai logok
- hibák (trace → error)
- audit események (INFO szinten)
- workflow részletek
- verbose routing

### EventLog (eseménylog)
- felhasználói események
- rövid, emberi nyelvű üzenetek
- non‑blocking UI visszajelzés

### Audit event (nem külön logcsatorna!)
- domain műveletek
- entitás + művelet + paraméterek
- SystemLog‑ba kerül, INFO szinten

## EventChannel (új fogalom)
Definíció:
  A logolási rendszer három elkülönített fogalmi csatornája:
  SystemLog, EventLog, AuditEvent.
Példa:
  - SystemLog → technikai részletek + audit
  - EventLog → user‑szintű alkalmazás‑események
  - AuditEvent → domain műveletek (SystemLog‑ban)
Anti‑pattern:
  - minden log egy csatornába
  - user‑esemény SystemLog‑ba
  - technikai hiba EventLog‑ba

### Application Event / AppEvent (új fogalom)
Definíció:
  Felhasználói művelet, amely az EventLog‑ba kerül.
Példa:
  „Mode created: Rugós”
Anti‑pattern:
  - Qt eventnek hívni
  - technikai részleteket beleírni
Naming:
  EventLog::info("Mode created: %1", name);

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

# **4.6 Logolási architektúra**

- SystemLog (hiba + audit + technikai)
- EventLog (felhasználói események)
- Audit event (SystemLog része)
- Verbose routing

***

## **4.6.1 SystemLog**
- technikai részletek
- CSV import
- registry audit

***

## **4.6.2 EventLog**
- user‑szintű események
- rövid, érthető üzenetek

***
## **Ne keverd össze a Qt eventet az alkalmazás‑eseménnyel**

A CutCalculator kettő logcsatornát használ:  
- **SystemLog** – technikai részletek  
- **EventLog** – alkalmazás‑események (AppEvent)  

A „EventLog” *nem* Qt‑eseményeket logol.  
A Qt event rendszer (`QEvent`, `mousePressEvent`, stb.) teljesen külön világ.  
Az EventLog kizárólag **felhasználói műveleteket és alkalmazás‑szintű eseményeket** rögzít.

Ez a különválasztás a konzisztencia és az auditálhatóság alapja.

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
Szuper, Zoltán — akkor gördülünk tovább a könyvszerű, végleges, tiszta anyaggal.

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

# **6.4 Logolási best practice**

## **6.4.1 Szétválasztott logok**
- SystemLog → technikai
- EventLog → felhasználói
- AuditLog → üzleti

***

## **6.4.2 Verbose routing**
A logok kategóriák szerint menjenek külön csatornákra.

***

## **6.4.3 Minden log legyen időbélyeges**
Ez audit követelmény.

***

## **6.4.4 A log legyen rövid és érthető**
**Jó:**
- „Material added: RAL 9010”

**Rossz:**
- „Material operation completed successfully with code 0x00000001”

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

# **7.4 Logolási anti‑patternök**

- ❌ külön AuditLog csatorna létrehozása
- ❌ audit események EventLog‑ba írása
- ❌ audit események warning/error szinten
- ❌ audit események popupban

## **7.4.1 Minden log egy fájlba**
**Miért rossz?** zajos, kereshetetlen, audit‑ellenes
**Megoldás:** log szétválasztás

***

## **7.4.2 Túl hosszú logbejegyzések**
**Miért rossz?** olvashatatlan, nem audit‑barát
**Megoldás:** rövid, érthető üzenetek

***

## **7.4.3 Log, amely UI‑t módosít**
**Rossz:** log → popup
**Miért rossz?** log nem UI
**Megoldás:** log → log

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
Szuper, Zoltán — akkor gördülünk tovább a könyvszerű, végleges, tiszta anyaggal.

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
# **10. UI/UX Guidelines**
*A jó UI nem dekoráció — hanem működő architektúra. A jó UX nem trükk — hanem következetes gondolkodás.*

A UI/UX célja:

- gyors munkavégzés
- hibamentes működés
- stabil, kiszámítható viselkedés
- snapshot‑kompatibilitás
- auditálhatóság
- fejlesztői konzisztencia
- felhasználói bizalom

A CutCalculator UI‑ja olyan mintákat hozott létre, amelyek projektfüggetlenül is alkalmazhatók.

***

# **10.1 Alapelvek**

## **10.1.1 A UI legyen láthatóan logikus**
A felhasználó mindig tudja:

- hol van
- mit csinál
- mi történik
- miért nem történik valami

Ez a „látható logika” elve.

***

## **10.1.2 A UI legyen snapshot‑kompatibilis**
A modern desktop UX egyik legfontosabb követelménye:

**A felhasználó UI‑ja nem változhat meg újraindításkor.**

Ez ipari alapelv.

***

## **10.1.3 A UI legyen non‑blocking**
A felhasználó:

- ne kapjon felesleges popupot
- ne akadjon meg a munkában
- ne kelljen „OK”-ot nyomogatnia

A modern UX: **inline feedback**.

***

## **10.1.4 A UI legyen audit‑barát**
A UI‑ban történő műveletek:

- legyenek logolva
- legyenek visszakövethetők
- legyenek érthetők

***

## **10.1.5 A UI legyen konzisztens**
A konzisztencia:

- gyorsít
- csökkenti a hibát
- növeli a bizalmat
- csökkenti a tanulási időt

***

# **10.2 Layout és elrendezés**

## **10.2.1 Percent‑based layout**
A fix pixeles layout:

- DPI‑függő
- monitorfüggő
- snapshot‑ellenes

A percent‑based layout:

- stabil
- rugalmas
- monitorfüggetlen
- snapshot‑kompatibilis

***

## **10.2.2 MinimumSizeHint használata**
A widgetek mérete:

- ne legyen fix
- ne legyen túl kicsi
- ne legyen túl nagy

A `minimumSizeHint()` a legjobb barát.

***

## **10.2.3 Margók és spacing**
A jó spacing:

- levegőt ad
- olvashatóvá tesz
- fókuszt ad

**Ajánlott:**

- spacing: 8–12 px
- margin: 12–16 px

***

## **10.2.4 DPI‑független méretezés**
A UI ne legyen:

- túl kicsi 4K‑n
- túl nagy 1080p‑n

A DPI‑független layout ipari követelmény.

***

# **10.3 Interakciós minták**

## **10.3.1 Inline validation mindenhol**
A felhasználó gépelés közben kapjon visszajelzést.

***

## **10.3.2 Accent frame a fókusz jelölésére**
A fókusz vizuális jelölése:

- kék keret
- finom glow
- enyhe árnyék

***

## **10.3.3 Shake animation hibára**
A shake:

- rövid
- horizontális
- non‑blocking
- egyértelmű

***

## **10.3.4 Cursor‑based positioning**
A dialog:

- ne a képernyő közepére ugorjon
- ne takarja el a fontos részeket
- ne legyen távol a kurzortól

***

## **10.3.5 Double‑click = accept**
Picker és lista esetén:

- double‑click → accept
- Enter → accept
- Esc → reject

***

## **10.3.6 A gombok sorrendje legyen konzisztens**
**Jó:**
- OK / Cancel
- Add / Remove
- Rename / Delete

**Rossz:**
- Cancel / OK (fordítva)
- Execute / OK / Apply (túl sok)

***

# **10.4 Snapshot‑tudatos UI**

## **10.4.1 Deferred restore**
A restore csak akkor történjen, amikor:

- a widget már látható
- a mérete ismert
- a layout stabil

***

## **10.4.2 Geometry guard**
A snapshot restore előtt:

- pozíció ellenőrzés
- méret ellenőrzés
- monitor ellenőrzés
- DPI ellenőrzés

Ha hibás → fallback.

***

## **10.4.3 Fallback baseline**
A fallback baseline:

- stabil
- determinisztikus
- auditálható

Ez a LayoutDefaultStore szerepe.

***

## **10.4.4 Snapshot verziózás**
A snapshot formátuma változhat — ezért:

- legyen verziószám
- legyen migráció
- legyen fallback

***

## **10.4.5 Multi‑monitor kompatibilitás**
A snapshot:

- ne tűnjön el monitorváltáskor
- ne legyen off‑screen
- ne legyen túl kicsi

***

# **10.5 Vizuális ritmus és hierarchia**

## **10.5.1 A fontos dolgok legyenek közel egymáshoz**
A vizuális közelség = logikai közelség.

***

## **10.5.2 A címek legyenek rövidek és egyértelműek**
**Jó:** „Material Picker”
**Rossz:** „Please select a material from the list below”

***

## **10.5.3 A táblák legyenek audit‑barátok**
- fix oszlopsorrend
- percent‑based szélesség
- rendezhető oszlopok
- vizuális fókusz

***

## **10.5.4 A gombok legyenek egyértelműek**
**Jó:** Add / Rename / Delete
**Rossz:** OK / Process / Execute

***

## **10.5.5 A vizuális zaj minimalizálása**
Kerüld:

- túl sok színt
- túl sok ikont
- túl sok animációt
- túl sok keretet

***

# **10.6 Hibakezelés és visszajelzés**

## **10.6.1 Hibák → inline**
Popup csak kritikus hibára.

***

## **10.6.2 Figyelmeztetések → UI‑szintű jelzés**
- sárga keret
- ikon
- tooltip

***

## **10.6.3 Információk → EventLog**
A felhasználó lássa, mi történt — de non‑blocking módon.

***

## **10.6.4 Technikai hibák → SystemLog**
A felhasználó ne lássa a technikai részleteket.

***

## 10.6.5 Audit események → SystemLog (INFO)
- rename
- insert
- delete
- workflow lépések
- domain műveletek

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
