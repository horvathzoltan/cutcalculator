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

# ⭐ **ELŐSZÓ**
*A kultúra nem szabály — hanem ritmus.*

A modern szoftverfejlesztésben egyre világosabb:
a technológia önmagában kevés.
A kód önmagában kevés.
A dokumentáció önmagában kevés.

Ami igazán számít:

- **hogyan gondolkodunk**,
- **hogyan kommunikálunk**,
- **hogyan szervezünk**,
- **hogyan építünk**,
- **hogyan tartjuk fenn a rendet**,
- **hogyan adjuk át a tudást**,
- **hogyan teremtünk ritmust**.

A CutCalculator projekt egy olyan alkalmazás, ami 
egy **laboratórium**, ahol kialakult egy fejlesztői kultúra:

- tiszta architektúrával,
- auditálható workflow‑val,
- snapshot‑tudatos UI‑jal,
- konzisztens naminggel,
- ikon‑vezérelt UX‑szel,
- prompt‑vezérelt fejlesztéssel,
- sablonokkal és DSL‑lel,
- napzáró rituáléval,
- és egy közös nyelvvel.

Ez a könyv ezt a kultúrát rögzíti.
Nem azért, hogy egy projektet dokumentáljon —
hanem azért, hogy **egy gondolkodásmódot örökítsen tovább**.

A CutCalculator csak példa.
A kultúra maga **átvihető**, **tanítható**, **skálázható**.

Ez a könyv azoknak szól, akik:

- tiszta, fenntartható rendszereket akarnak építeni,
- szeretnék, ha a kódjuk örömmel olvasható lenne,
- hisznek a konzisztenciában,
- értékelik a ritmust,
- és szeretnék, ha a fejlesztés nem káosz, hanem élmény lenne.

Ez a könyv **nem szabályzat**.
Ez a könyv **iránytű**.

Induljunk.

***

# ⭐ **TARTALOMJEGYZÉK**

***

## **I. rész — Alapok és közös nyelv**
1.1 Terminológiai Szótár
1.2 A fejlesztői kultúra alapelvei
1.3 Naming Convention Guide

***

## **II. rész — Architektúra és rendszertervezés**
2.1 Architektúra és rendszertervezés
2.2 Logolási architektúra
2.3 Architektúra best practice
2.4 Architektúra anti‑patternök

***

## **III. rész — UX és vizuális nyelv**
3.1 UI/UX Guidelines
3.2 Ikonok és kulcsszínek
3.3 Validáció és vizuális visszajelzések
3.4 A UX mint hibamegelőző rendszer
3.5 Vizuális ritmus és kognitív terhelés
3.6 Vizuális identitás és projektkultúra
3.7 A vizuális nyelv mint hibamegelőzés
3.8 A vizuális nyelv és onboarding
3.9 UI anti‑patternök
3.10 UI/UX best practice

***

## **IV. rész — Workflow és fejlesztői ritmus**
4.1 Workflow és fejlesztési ritmus
4.2 Snapshot‑tudatos UI workflow
4.3 Refaktorálási protokoll
4.4 Napzáró rituálé

***

## **V. rész — Prompt DSL és generatív fejlesztés**
5.1 Prompt DSL és Prompt Engineering
5.2 Prompt DSL szerkezete
5.3 Prompt szerkezet és ipari anti‑pattern kézikönyv
5.4 Prompt DSL Standard Library
5.5 Prompt best practice
5.6 Prompt anti‑patternök

***

## **VI. rész — Sablonok és boilerplate minták**
6.1 Registry sablon
6.2 Repository sablon
6.3 Manager sablon
6.4 Dialog sablon
6.5 Picker sablon
6.6 Editor sablon
6.7 Snapshot‑kompatibilis UI sablon
6.8 Logolási sablonok

***

## **VII. rész — Ipari minták és inspirációk**
7.1 Ipari minták
7.2 Snapshot‑tudatos UI mint ipari alapelv
7.3 Moduláris architektúra
7.4 Auditálható workflow
7.5 Non‑blocking UX
7.6 Percent‑based layout
7.7 Pipeline‑alapú feldolgozás
7.8 Naming mint vállalati kultúra
7.9 Logolási architektúrák
7.10 Prompt‑vezérelt fejlesztés
7.11 Fejlesztői kultúra mint termékfilozófia

***

## **VIII. rész — Zárófejezet
8.1 A kultúra mint rendszer
8.2 A kultúra mint ritmus
8.3 A kultúra mint közös nyelv
8.4 A kultúra mint örökség — és a projekt valódi kezdete
8.5 Záró gondolat

***

# 📘 **I. rész — Alapok és közös nyelv**
*A kultúra alapja a közös nyelv. A közös nyelv alapja a tiszta fogalomkészlet.*

A fejlesztői kultúra nem a kódban kezdődik.
A fejlesztői kultúra **a szavakban** kezdődik.

Ahhoz, hogy egy csapat tisztán, gyorsan, félreértések nélkül tudjon együtt dolgozni, szükség van:

- közös fogalmakra,
- közös jelentésekre,
- közös mintákra,
- közös elvekre,
- közös ritmusra.

Ez a rész ezt a közös nyelvet teremti meg.

***

# ⭐ **1.1 Terminológiai Szótár**
*A fogalmak tisztasága a kultúra tisztasága.*

Ez a szótár rögzíti a CutCalculator‑kultúra alapfogalmait.
Minden fogalom **egyértelmű**, **stabil**, **projektfüggetlen**,
és a könyv további részeiben **következetesen ugyanazt jelenti**.

## **Domain fogalmak**
- **Domain** — a rendszer üzleti logikája, UI‑tól és IO‑tól független.
- **Entitás** — a domain egy konkrét objektuma (Material, Need, Product).
- **Value Object** — értékalapú típus, nincs ID‑je.
- **Identifier (ID)** — egy entitás egyedi azonosítója.
- **Rule / NeedRule** — domain‑szabály, amely meghatározza a számítás logikáját.

## **Architektúra fogalmak**
- **Repository** — IO‑réteg (betölt, ment, keres).
- **Registry** — memóriabeli állapot, auditálható módosításokkal.
- **Manager** — domain‑műveletek és workflow agya.
- **Presenter** — UI‑logika, a View és a Manager között.
- **View** — megjelenítés, logika nélkül.
- **Pipeline** — lépésekből álló feldolgozási folyamat.
- **Step** — a pipeline egy műveleti egysége.
- **Snapshot** — a UI állapotának mentése és visszaállítása.
- **Fallback baseline** — biztonságos alap‑layout, ha a snapshot hibás.

## **UI/UX fogalmak**
- **Inline validation** — valós idejű mezőszintű validáció.
- **Accent frame** — vizuális kiemelés hibás mezőknél.
- **Dialog summary bar** — hibák/figyelmeztetések összesítése.
- **Cursor‑based positioning** — dialog a kurzor közelében.
- **Non‑blocking UX** — nincs felesleges popup.
- **Visual rhythm** — következetes spacing, margin, ikonhasználat.
- **Pre‑attentív jelzés** — azonnal értelmezhető vizuális jel.

## **Workflow fogalmak**
- **Audit event** — domain‑művelet naplózott lenyomata.
- **SystemLog** — technikai + domain audit log.
- **EventLog** — felhasználói események rövid naplója.
- **DebugLog** — fejlesztői diagnosztikai log.
- **Napzáró** — napi fejlesztési összefoglaló.
- **Refaktorálási protokoll** — a rendszer tisztításának szabályai.
- **Deferred restore** — snapshot visszaállítása stabil UI‑állapot után.

## **Prompt DSL fogalmak**
- **Prompt DSL** — generatív fejlesztési szabályrendszer.
- **Kontextus blokk** — modul célja, domainje, hatóköre.
- **Naming blokk** — projekt naming‑szabályai.
- **Szerkezeti blokk** — osztályok, metódusok, workflow terve.
- **Anti‑pattern blokk** — tiltott minták listája.
- **Master Template** — a DSL alap sablonja.

***

# ⭐ **1.2 A fejlesztői kultúra alapelvei**
*A kultúra nem szabály — hanem gondolkodásmód.*

A CutCalculator‑kultúra alapelvei:

### ✔️ **Konzisztencia**
A konzisztencia gyorsít, tisztít, stabilizál.

### ✔️ **Tisztaság**
A tiszta kód nem magyarázkodik — érthető.

### ✔️ **Auditálhatóság**
A rendszer minden lépése visszakövethető.

### ✔️ **Snapshot‑tudatosság**
A UI stabil marad, még változó környezetben is.

### ✔️ **Domain‑vezérelt gondolkodás**
A domain a rendszer igazságforrása.

### ✔️ **Anti‑pattern kerülés**
A rossz minták felismerése és kizárása.

### ✔️ **Prompt‑vezérelt fejlesztés**
A prompt nem kérés — specifikáció.

### ✔️ **Ritmus**
A fejlesztés nem sprint — hanem folyamat.

***

# ⭐ **1.3 Naming Convention Guide**
*A név nem dekoráció. A név architektúra.*

A névadás a fejlesztői kultúra egyik legfontosabb eleme.
A jó név:

- egyértelmű,
- konzisztens,
- domain‑orientált,
- kereshető,
- auditálható,
- hosszú távon stabil.

Ez a fejezet rögzíti a projektfüggetlen, CutCalculator‑szellemű névadási szabályokat.

***

## **1.3.1 Általános naming elvek**

### **A név legyen beszédes, de nem túl hosszú**
**Jó:**
- `NeedCalculationManager`
- `MaterialPickerDialog`

**Rossz:**
- `Mgr`
- `Dlg`
- `ThingHandler`

### **Egy fogalom → egy név**
Ha a domainben „Mode” a fogalom, akkor:

- `ModeRepository`
- `ModeRegistry`
- `ModeManager`
- `ModeNameDialog`

Nem:
- `CalculationTypeDialog`
- `ProfileManager`

***

### **Kerüld a rövidítéseket — kivéve, ha köznyelvi vagy projekt‑szintű fogalom**

A rövidítések általában rontják az olvashatóságot, ezért alapértelmezés szerint kerülendők.

**Kivétel: köznyelvi, széles körben ismert technikai rövidítések**, amelyeknek saját Wikipédia‑oldaluk van, és a fejlesztők természetes módon így használják őket:

- **GPS, DPI, CPU, RAM, UUID, API, HTTP, JSON, XML, UTF‑8**, stb.

Ezeket nem kell kifejteni, mert a kifejtett forma *kevésbé természetes*, és nem növeli az érthetőséget.

**Kivétel: a projekt saját, dokumentált terminológiája**, amely:

- konzisztensen jelenik meg a kódban,
- egyértelmű a csapat számára,
- domain‑szinten stabil.

Ilyenek lehetnek például a CutCalculator‑specifikus rövidítések (pl. `NR`, `MRV`, `PTV`), ha a projekt szótárában szerepelnek.

**Kerülendő rövidítések:**

- amelyek nem közismert fogalmak,
- amelyek többféleképpen értelmezhetők,
- amelyek csak egy fejlesztő fejében egyértelműek.

Ha egy rövidítés nem közismert és nincs dokumentálva → **nem használjuk**.

***

### **A név legyen stabil**
A domain‑név nem sprintenként változik.

***

## **1.3.2 Fájlnév konvenciók (snake_case)**

A Qt buildrendszer miatt a fájlnevek **snake_case** formátumúak:

- `need_calculation_manager.cpp`
- `material_picker_dialog.h`
- `color_registry.cpp`

Ez teljesen rendben van — a fájlnév nem domain‑fogalom, hanem *technikai útvonal*.

**Szabály:**
- csak kisbetű
- szavak `_`‑el elválasztva
- `.cpp` / `.h` végződés
- nincs szóköz, nincs ékezet

***

## **1.3.3 Osztálynév konvenciók (PascalCase)**

- `NeedCalculationManager`
- `MaterialPickerDialog`
- `ColorRegistry`

**Tiltott:**
- `needCalculationManager`
- `material_picker_dialog`
- `Color_registry`

***

## **1.3.4 Metódusnév konvenciók (camelCase)**

- `loadMaterials()`
- `insertMode()`
- `validateInput()`

**UI metódusok:**
- `updateTable()`
- `applyAccentFrame()`
- `positionNearCursor()`

***

## **1.3.5 Változónév konvenciók**

### **camelCase + opcionális `_` a fogalmi határhoz**

A CutCalculator‑kultúrában a változónév:

- alapból **camelCase**,
- de **fogalmi határnál használható `_`**, ha javítja az olvashatóságot.

**Jó:**
- `materialId`
- `selected_material_id`
- `currentModeName`
- `max_width_px`

**Rossz:**
- `mtrlId`
- `selMatId`
- `currentmodename`

### **Privát tagok: `_` előtag**

- `_materialId`
- `_currentMode`
- `_tableFormatter`

### **Pointerek, referenciák: nincs `p` vagy `r` előtag**
Ez **C++98‑as örökség**, modern C++‑ban kerülendő.

### **Konstansok: SCREAMING_SNAKE_CASE**

- `MAX_WIDTH_PX`
- `DEFAULT_COLOR_NAME`

***

## **1.3.6 Registry / Repository / Manager naming**

### **Repository**
- `XRepository`
- metódusok: `load()`, `save()`, `findById()`, `all()`

### **Registry**
- `XRegistry`
- metódusok: `insert()`, `remove()`, `rename()`, `exists()`

### **Manager**
- `XManager`
- metódusok: `create()`, `update()`, `delete()`, `validate()`

***

## **1.3.7 UI komponensek naming**

- `XView`
- `XPresenter`
- `XDialog`
- `XPickerDialog`
- `XEditorDialog`
- `XPreview`

***

## **1.3.8 Pipeline elemek naming**

- `XPipeline`
- `XStep`

***

## **1.3.9 Validációs komponensek naming**

- `XValidator`
- `setErrorState()`
- `clearErrorState()`

***

## **1.3.10 Anti‑pattern naming példák**

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

***

# ⭐ **I. rész összegzése**

Ez a rész megteremtette:

- a közös nyelvet,
- a közös gondolkodást,
- a közös elveket,
- a közös naming‑rendszert,
- a közös ritmust.

Innen indul a könyv többi része:

- az architektúra,
- a UX,
- a workflow,
- a Prompt DSL,
- a sablonok,
- az ipari minták.

A kultúra alapja most már stabil.

***

# 📘 **II. rész — Architektúra és rendszertervezés**
*A jó architektúra nem bonyolult — hanem tiszta, következetes és örömmel fejleszthető.*

A modern szoftverarchitektúra célja nem az, hogy lenyűgözze a fejlesztőt.
A modern architektúra célja:

- **stabilitás**,
- **fenntarthatóság**,
- **tesztelhetőség**,
- **auditálhatóság**,
- **kiszámíthatóság**,
- **és öröm a fejlesztésben**.

A CutCalculator‑eredetű architektúra ezekre az alapokra épül:

- Domain‑vezérelt gondolkodás
- Registry/Repository/Manager triád
- Tiszta UI‑rétegzés (View/Presenter/Manager)
- Snapshot‑tudatos UI
- Háromrétegű logolási architektúra
- Anti‑patternök szigorú kerülése

Ez a rész ezt a rendszert írja le.

***

# ⭐ **2.1 Architektúra és rendszertervezés**
*A rendszer akkor stabil, ha minden réteg a saját szerepét végzi.*

A modern architektúra három nagy pillére:

1. **Tiszta domain**
2. **Tiszta rétegzés**
3. **Tiszta felelősségek**

A CutCalculator architektúrája ezekre épül.

***

# 🎯 **2.1.1 A domain‑szétválasztás mint ipari best practice**

A domain legyen:

- tiszta
- UI‑független
- IO‑független
- tesztelhető
- stabil

A domain a rendszer „igazságforrása”.

Ezért:

- a domain nem tartalmaz UI‑t,
- nem tartalmaz Qt‑t,
- nem ír fájlt,
- nem logol,
- nem tart workflow‑t.

A domain **független és örök**.

***

# 🎯 **2.1.2 Registry/Repository/Manager triád**
*A triád a rendszer stabilitásának alapmintája.*

Ez a triád ipari mintákon alapul:

- **Repository pattern (Fowler)**
- **DDD Service/Domain separation**
- **Redux‑szerű state store**
- **Clean Architecture use‑case réteg**

A triád három szerepe:

***

## ⭐ **2.1.3 Repository — IO réteg**

Feladata:

- betöltés
- mentés
- keresés
- konvertálás

Tiltások:

- ❌ domain‑validáció
- ❌ workflow
- ❌ UI

A Repository **tiszta IO**.

***

## ⭐ **2.1.4 Registry — memóriabeli állapot**

Feladata:

- gyors elérés
- cache
- auditált állapot
- ütközéskezelés

Tiltások:

- ❌ fájlírás
- ❌ workflow
- ❌ UI

A Registry **tiszta állapot**.

***

## ⭐ **2.1.5 Manager — domain‑műveletek és workflow**

Feladata:

- validáció
- tranzakciók
- workflow
- audit események generálása

Tiltások:

- ❌ UI
- ❌ közvetlen IO
- ❌ állapot tárolása

A Manager **a domain agya**.

***

# 🎯 **2.1.6 Snapshot‑architektúra (ipari minta)**
*A snapshot‑tudatos UI a modern desktop alkalmazások alapkövetelménye.*

A snapshot‑tudatos UI:

- megőrzi a felhasználó munkaterét
- DPI‑független
- monitorváltás‑tudatos
- guardolt
- fallback‑képes
- verziózott

Ez ipari minta:

- Adobe
- JetBrains
- Qt Creator
- Affinity

A snapshot‑architektúra a stabil UX alapja.

***

# 🎯 **2.1.7 UI‑architektúra — View / Presenter / Manager**

A modern UI rétegei:

- **View** → megjelenítés
- **Presenter** → UI‑logika
- **Manager** → domain‑workflow
- **Registry** → állapot
- **Repository** → IO

Ez a Clean Architecture UI‑rétegzésének megfelelője.

A View nem gondolkodik.
A Presenter nem dönt domain‑szinten.
A Manager nem jelenít meg UI‑t.

***

# ⭐ **2.2 Logolási architektúra**
*A logolás nem mellékes technikai részlet — hanem a rendszer stabilitásának alapja.*

A logolás három csatornára épül:

- **DebugLog** → fejlesztői diagnosztika
- **SystemLog** → technikai + domain audit
- **EventLog** → felhasználói workflow

Ez a szétválasztás biztosítja:

- a rendszer stabilitását
- a logok érthetőségét
- az auditálhatóságot
- a hibák gyors visszakövethetőségét

A logolás ikon‑vezérelt és színkódolt, teljes összhangban a UI‑val.

***

# ⭐ **2.3 Architektúra best practice**
*A jó architektúra egyszerű, tiszta és következetes.*

### ✔️ **SRP — Egy felelősség elve**
Minden osztály egyetlen dolgot csináljon — de azt jól.

### ✔️ **Tiszta domain**
A domain nem függ UI‑tól vagy IO‑tól.

### ✔️ **Vékony UI**
A View nem validál, nem számol, nem módosít domain‑adatot.

### ✔️ **Presenter mint UI‑agy**
A Presenter irányítja a UI‑t, de nem tartalmaz domain‑logikát.

### ✔️ **Manager mint domain‑agy**
A Manager workflow‑t kezel, validál, auditál.

### ✔️ **Registry mint auditálható állapot**
A Registry gyors, determinisztikus, tiszta.

### ✔️ **Repository mint tiszta IO**
A Repository nem validál, nem auditál, nem workflow‑zik.

***

# ⭐ **2.4 Architektúra anti‑patternök**
*A rossz minták felismerése legalább olyan fontos, mint a jó minták követése.*

### ❌ **God Object**
Egy osztály, amely túl sok felelősséget kap.

### ❌ **UI‑függő domain**
A domain nem tartalmazhat QWidget‑et.

### ❌ **Repository, amely validál**
A validáció domain‑szintű.

### ❌ **Registry, amely fájlt ír**
A Registry állapotot tart, nem IO‑t végez.

### ❌ **Manager, amely UI‑t módosít**
A Manager workflow, nem UI.

Ezek a tiltások a rendszer stabilitásának védelmi vonalai.

***

# ⭐ **II. rész összegzése**

Ez a rész lefektette:

- a rendszer architektúráját,
- a triádot,
- a tiszta rétegzést,
- a snapshot‑tudatos UI alapjait,
- a logolási architektúrát,
- a best practice‑eket,
- és az anti‑patternöket.

Ez a könyv **gerince**.
Erre épül a UX, a workflow, a Prompt DSL és a sablonok teljes rendszere.

***

# 📘 **III. rész — UX és vizuális nyelv**
*A felület nem dísz. A felület a rendszer arca. A vizuális nyelv a rendszer lelke.*

A modern UX nem arról szól, hogy „szép legyen”.
A modern UX arról szól, hogy:

- **érthető**,
- **kiszámítható**,
- **következetes**,
- **gyorsan értelmezhető**,
- **hibabiztos**,
- **audit‑tudatos**,
- **és örömmel használható** legyen.

A CutCalculator UX‑filozófiája erre épül — és kiegészül egy egyedi elemmel:

> **A UI és a logolás ugyanazt a vizuális nyelvet beszéli.**

Ez ipari szinten is ritka, és a könyv egyik legnagyobb innovációja.

***

# ⭐ **3.1. UI/UX Guidelines**
*A jó UX nem extra — hanem minőségbiztosítás.*

A CutCalculator UX‑filozófiája öt alapelvre épül:

### ✔️ Felhasználó‑központúság
A felület a felhasználóért van.

### ✔️ Intuitív navigáció
A felhasználó nem tévedhet el.

### ✔️ Feladat‑orientáltság
A cél nem a program használata — hanem a munka elvégzése.

### ✔️ Következetesség
Ugyanaz az ikon → ugyanaz a jelentés.
Ugyanaz a szín → ugyanaz a jelentés.

### ✔️ Teljes élmény
A UX nem csak gombokból áll — hanem ritmusból, visszajelzésből, stabilitásból.

***

# ⭐ **3.2 Ikonok és kulcsszínek — a vizuális nyelv alapja**

A vizuális nyelv négy ikonra épül:

| Ikon | Szín | Jelentés |
|******|******|*********-|
| ℹ️ | kék | semleges információ |
| ✔️ | zöld | sikeres művelet |
| ❌ | piros | sikertelen művelet |
| ⚠️ | sárga | figyelmeztetés |

Ez a rendszer:

- gyors,
- kognitív,
- pre‑attentív,
- audit‑tudatos,
- és teljes összhangban van a logolással.

A CutCalculator egyik legnagyobb ereje:
**a UI ikonja = a log ikonja.**

***

# ⭐ **3.3 Validáció és vizuális visszajelzések**

A modern UX egyik alapelve:
**a validáció legyen valós idejű, mezőszintű és diszkrét.**

A mezők négy állapotot vehetnek fel:

- ✔️ érvényes
- ⚠️ figyelmeztetés
- ❌ hibás
- ℹ️ információ

A dialog alján összesítő sáv jelenik meg:

- ❌ hibák száma
- ⚠️ figyelmeztetések száma
- ✔️ menthető állapot

Ez a rendszer:

- megelőzi a hibát,
- csökkenti a frusztrációt,
- gyorsítja a munkát,
- stabilizálja a UX‑et.

***

# ⭐ **3.4 A UX mint hibamegelőző rendszer**

A hibák 80%-a UX‑eredetű.
A jó UX:

- megelőzi a hibát,
- csökkenti a hibák súlyosságát,
- gyorsítja a hibák felismerését,
- segíti a hibák kijavítását.

A UX itt nem dekoráció —
**hanem minőségbiztosítási eszköz.**

***

# ⭐ **3.4.1 A UX és a logolás kapcsolata**
*A UI és a SystemLog ugyanazt a nyelvet beszéli.*

Ez a könyv egyik legfontosabb felismerése:

- a felhasználó ugyanazt látja, amit a rendszer tud,
- a fejlesztő ugyanazt érti, amit a felhasználó lát,
- a support ugyanazt értelmezi, amit a log mutat.

A UI ikonja = a log ikonja.
A UI színe = a log színe.
A UI visszajelzése = a log szintje.

Ez ipari szinten is ritka, és elképesztően erős UX‑eszköz.

***

# ⭐ **3.5 Vizuális ritmus és kognitív terhelés**

A vizuális ritmus:

- kiszámítható,
- stabil,
- következetes,
- ergonomikus.

A ritmus csökkenti:

- a hibázást,
- a bizonytalanságot,
- a kognitív terhelést.

A ritmus = biztonság.
A ritmus = minőség.

***

# ⭐ **3.6 Vizuális identitás és projektkultúra**

A vizuális identitás:

- bizalom,
- következetesség,
- egyediség,
- márka,
- kultúra.

A UI a projekt arca.
A UX a projekt lelke.

A vizuális identitás a projektkultúra egyik legerősebb hordozója.

***

# ⭐ **3.7 A vizuális nyelv mint hibamegelőzés**

A vizuális nyelv három rétegben működik:

1. **Pre‑attentív réteg** (ikonok, színek)
2. **Kognitív réteg** (hierarchia, elrendezés)
3. **Narratív réteg** (visszajelzések)

Ez a rendszer:

- megelőzi a hibát,
- csökkenti a hibák súlyosságát,
- gyorsítja a hibák felismerését,
- segíti a hibák kijavítását.

***

# ⭐ **3.8 A vizuális nyelv és onboarding**

A UI nem csak a felhasználót tanítja —
**a fejlesztőt is.**

A vizuális nyelv:

- élő dokumentáció,
- onboarding eszköz,
- review‑gyorsító,
- minőségbiztosítás.

A UI maga a kultúra.

***

# ⭐ **3.9 UI anti‑patternök**

- ❌ popup minden hibára
- ❌ fix pixeles layout
- ❌ dialog a képernyő közepén
- ❌ domain‑logika a UI‑ban
- ❌ rossz ikonhasználat
- ❌ következetlen színek

Ezek a hibák megtörik a ritmust.

***

# ⭐ **3.10 UI/UX best practice**

- inline validation
- accent frame
- snapshot‑kompatibilis UI
- cursor‑based positioning
- percent‑based layout
- konzisztens spacing és margin

Ez a fejezet a UX „szerszámosládája”.

***

# ⭐ **III. rész összegzése**

Ez a rész megteremtette:

- a vizuális nyelvet,
- a validációs rendszert,
- a hibamegelőző UX‑modellt,
- a logolás és UI közös nyelvét,
- a vizuális ritmust,
- a projekt vizuális identitását.

Ez a könyv **lelke**.
Erre épül a workflow, a Prompt DSL és a sablonok teljes rendszere.

***

# 📘 **IV. rész — Workflow és fejlesztői ritmus**
*A fejlesztés nem sprint — hanem ritmus. A ritmus pedig kultúra.*

A modern fejlesztésben a workflow nem adminisztráció.
Nem projektmenedzsment.
Nem „munkafolyamat”.

A workflow:

- **gondolkodásmód**,
- **szokás**,
- **ritmus**,
- **minőségbiztosítás**,
- **auditálhatóság**,
- **és fejlesztői élmény**.

A CutCalculator alatt kialakult workflow olyan mintákat hozott létre, amelyek projektfüggetlenül is alkalmazhatók.
Ez a rész ezt a ritmust írja le.

***

# ⭐ **4.1 Workflow és fejlesztési ritmus**
*A jó workflow nem lassít — hanem gyorsít.*

A workflow nyolc pillére:

### ✔️ **Modularitás**
Minden modul külön egység.

### ✔️ **Auditálhatóság**
Minden művelet visszakövethető.

### ✔️ **Snapshot‑tudatosság**
A UI stabil marad.

### ✔️ **Domain‑vezérelt gondolkodás**
A fogalmak tiszták.

### ✔️ **Konzisztens naming**
A kód kereshető.

### ✔️ **Folyamatos refaktorálás**
A rendszer tiszta marad.

### ✔️ **Napzáró rituálé**
A fejlesztő fókuszt és lezárást kap.

### ✔️ **Prompt‑vezérelt fejlesztés**
A prompt nem kérés — specifikáció.

Ez a nyolc pillér adja a fejlesztés ritmusát.

***

# ⭐ **4.1.1 Moduláris fejlesztési ciklus**
*A fejlesztés nem káosz — hanem lépések sorozata.*

A CutCalculator fejlesztése során kialakult egy hétlépéses ciklus:

## **1. Tervezés**
- domain meghatározása
- naming kiválasztása
- workflow kijelölése
- UI‑vázlat
- snapshot‑követelmények

## **2. Skeleton**
- osztályok létrehozása
- interface‑ek
- alap metódusok
- logolási csatornák előkészítése

## **3. Implementáció**
- domain logika
- validáció
- workflow
- snapshot integráció
- UI/UX minták alkalmazása

## **4. Integráció**
- Presenter ↔ Manager
- Manager ↔ Registry
- Registry ↔ Repository
- UI ↔ SnapshotManager

## **5. Tesztelés**
- unit tesztek
- UI tesztek
- snapshot tesztek
- audit tesztek

## **6. Refaktorálás**
- naming tisztítás
- moduláris bontás
- anti‑pattern eltávolítás
- UI/UX finomhangolás

## **7. Dokumentálás**
- napzáró
- állapottábla
- roadmap frissítés
- prompt‑archívum frissítése

Ez a ciklus biztosítja, hogy a fejlesztés:

- átlátható,
- auditálható,
- fenntartható,
- és örömmel végezhető legyen.

***

# ⭐ **4.2 Snapshot‑tudatos UI workflow**
*A snapshot nem extra — hanem ipari követelmény.*

A snapshot‑tudatos UI biztosítja, hogy a felhasználó munkaterülete:

- megőrződik,
- stabil marad,
- DPI‑független,
- monitorváltás‑tudatos,
- guardolt,
- fallback‑képes.

A workflow négy kulcseleme:

## **1) Snapshot‑aware komponensek**
Minden UI‑elem:

- menthető
- visszaállítható
- guard‑olt
- fallback‑képes
- DPI‑független

## **2) Deferred restore**
A restore csak akkor történik, amikor:

- a widget már látható
- a mérete ismert
- a layout stabil
- a monitorprofil betöltődött

## **3) Fallback baseline**
Ha a snapshot hibás:

- LayoutDefaultStore → baseline
- GeometryGuard → guard
- ScreenChangeDetector → monitorváltás kezelése

## **4) Snapshot verziózás**
A snapshot formátuma változhat — legyen verziószám.

Ez a workflow ipari szintű stabilitást ad.

***

# ⭐ **4.3 Refaktorálási protokoll**
*A refaktorálás nem opció — hanem ritmus.*

A refaktorálás akkor szükséges, ha:

- a naming billeg
- a domain keveredik
- a UI túl sok felelősséget kap
- a logolás zajos
- a snapshot instabil
- a pipeline túl nagy
- a Presenter túl sokat tud

A refaktorálás lépései:

1. domain tisztítás
2. naming tisztítás
3. moduláris bontás
4. UI tisztítás
5. logolás tisztítása
6. snapshot‑guard erősítése

Tiltások:

- „majd később megcsináljuk”
- „jó ez így is”
- „nem akarom szétszedni”

A refaktorálás a kultúra része.

***

# ⭐ **4.4 Napzáró rituálé**
*A nap lezárása a fejlesztői kultúra egyik legerősebb eszköze.*

A napzáró dokumentum tartalmazza:

- napi eredmények
- taktikai összefoglaló
- stratégiai összefoglaló
- állapottábla
- roadmap
- prioritási lista
- holnapi fókusz

A napzáró:

- lezárja a napot
- auditálható
- visszakereshető
- ritmust ad
- fókuszt ad
- motivációt ad

A napzáró nem adminisztráció —
**hanem önreflexió, rend és kultúra**.

***

# ⭐ **IV. rész összegzése**

Ez a rész megteremtette:

- a fejlesztés ritmusát,
- a moduláris workflow‑t,
- a snapshot‑tudatos gondolkodást,
- a refaktorálási protokollt,
- a napzáró rituálét,
- és a prompt‑vezérelt fejlesztés alapját.

Ez a könyv *pulzusa*.
Erre épül a Prompt DSL és a sablonok teljes rendszere.

***

# 📘 **V. rész — Prompt DSL és generatív fejlesztés**
*A prompt nem kérés. A prompt specifikáció. A Prompt DSL pedig a fejlesztői kultúra nyelve.*

A modern fejlesztésben a prompt‑vezérelt gondolkodás nem játék, nem trükk, nem „AI‑hack”.
A prompt‑vezérelt fejlesztés:

- **architektúra**,
- **workflow**,
- **dokumentáció**,
- **kultúra**,
- **és minőségbiztosítás**.

A CutCalculator alatt kialakult Prompt DSL egy olyan rendszer, amely:

- moduláris,
- determinisztikus,
- auditálható,
- konzisztens,
- projektfüggetlen,
- és ipari szintű.

Ez a rész ezt a nyelvet írja le.

***

# ⭐ **5.1 Prompt DSL és Prompt Engineering**
*A prompt a fejlesztői kultúra egyik legfontosabb eszköze.*

A prompt nem utasítás.
A prompt nem kérés.
A prompt **műszaki specifikáció**.

A jó prompt:

- kontextust ad,
- határokat ad,
- naminget ad,
- példát ad,
- anti‑patternöket tilt,
- workflow‑t definiál,
- sablonokat használ.

A promptból:

- kód,
- architektúra,
- UI,
- workflow,
- dokumentáció,
- sablonok

születnek.

A prompt → **dokumentáció**.
A prompt → **architektúra**.
A prompt → **kultúra**.

***

# ⭐ **5.2 A Prompt DSL szerkezete**
*A DSL négy blokkja a determinisztikus működés alapja.*

A CutCalculator Prompt DSL négy kötelező blokkból áll:

***

## **1) Kontextus blokk**
A projekt, modul és cél meghatározása.

Tartalmazza:

- domain
- modul
- cél
- hatókör
- input/output elvárások

Ez a blokk adja meg a *miértet* és a *hol vagyunk* kérdésre a választ.

***

## **2) Naming blokk**
A projekt naming‑szabályainak explicit rögzítése.

Tartalmazza:

- preferált nevek
- tiltott nevek
- domain‑fogalmak
- szerepvégződések

A név → architektúra.

***

## **3) Szerkezeti blokk**
A generálandó modul szerkezeti terve.

Tartalmazza:

- osztályok
- metódusok
- workflow
- UI‑elemek
- snapshot‑követelmények

Ez a blokk adja meg a *mit* és *hogyan* kérdésre a választ.

***

## **4) Anti‑pattern blokk**
A tiltott minták explicit felsorolása.

Tartalmazza:

- architekturális tiltások
- naming tiltások
- UI tiltások
- snapshot tiltások

Ez a blokk adja meg a *mit nem szabad* kérdésre a választ.

***

# ⭐ **5.3 Prompt szerkezet és ipari anti‑pattern kézikönyv**
*A CutCalculator DSL tökéletesen illeszkedik a Microsoft Copilot ipari promptmodelljéhez.*

A Copilot promptmodellje:

- **Goal**
- **Context**
- **Expectations**
- **Source**

A CutCalculator DSL:

- **Kontextus**
- **Naming**
- **Szerkezet**
- **Anti‑pattern**

A kettő **egy az egyben megfeleltethető**.

Ez azt jelenti:

- a CutCalculator DSL ipari szintű,
- kompatibilis a modern AI‑modellekkel,
- tanítható,
- skálázható,
- auditálható.

***

# ⭐ **5.3.1 Do’s — Hogyan kell jól promptolni**

- strukturált prompt
- pozitív utasítások
- példák adása
- iteráció
- kontextus + elvárás + példa
- világos cél
- részletek megadása
- udvarias, tiszta nyelvezet

A jó prompt → jó kód.

***

# ⭐ **5.3.2 Don’ts — Hogyan lehet elrontani**

- homályos prompt
- ellentmondó utasítások
- témaugrás
- informális nyelvezet
- domain‑idegen naming
- tiltások hiánya
- példák hiánya
- rossz ikonhasználat

A rossz prompt → rossz kód.

***

# ⭐ **5.4 Prompt DSL Standard Library**
*A projektkultúra „szerszámosládája”.*

A Standard Library tartalmazza:

- Master Template
- Modul Template (Manager/Registry/Dialog)
- Log Template
- UI/UX visszajelzés template
- Teszt template

Ezek a sablonok:

- gyorsítják a fejlesztést,
- csökkentik a hibákat,
- egységesítik a kódot,
- átörökítik a kultúrát.

***

# ⭐ **5.5 Prompt best practice**

- a prompt legyen specifikáció
- legyen moduláris
- legyen auditálható
- legyen iteratív
- legyen konzisztens

A prompt → a fejlesztői kultúra hordozója.

***

# ⭐ **5.6 Prompt anti‑patternök**

- túl rövid prompt
- túl hosszú, rendezetlen prompt
- tiltások hiánya
- példák hiánya
- domain‑idegen naming

A tiltások a kultúra védelmi vonalai.

***

# ⭐ **V. rész összegzése**

Ez a rész megteremtette:

- a Prompt DSL nyelvét,
- a generatív fejlesztés alapjait,
- a prompt mint specifikáció gondolkodásmódját,
- a sablonrendszert,
- a best practice‑eket,
- az anti‑patternöket,
- és a teljes generatív workflow‑t.

Ez a könyv *intellektuális csúcspontja*.
Erre épül a sablonok rendszere és a teljes fejlesztői kultúra átörökíthetősége.

***

# 📘 **VI. rész — Sablonok és boilerplate minták (ÚJ, PROJEKT‑ALAPÚ)**
*A sablonok nem kényelmi funkciók — hanem a fejlesztői kultúra hordozói.*

A CutCalculator sablonjai **nem elméleti minták**, hanem a projektben bizonyított, auditált, stabil struktúrák.
Minden sablon a következő elvekre épül:

- **tiszta felelősség**
- **auditálhatóság**
- **UI‑függetlenség**
- **repo‑függetlenség**
- **snapshot‑tudatosság**
- **logolási konzisztencia**
- **Prompt DSL kompatibilitás**

A sablonok célja:

- gyors modulépítés
- hibák csökkentése
- konzisztens architektúra
- auditálható workflow
- könnyű onboarding

***

# ⭐ **6.1 Registry sablon (CutCalculator‑minta)**
*A Registry a memóriabeli állapot auditálható tárolója.*

A Registry NEM:

- nem validál
- nem workflow
- nem UI
- nem IO (kivéve: *perzisztálhat* repo segítségével)

A Registry IGEN:

- memóriabeli állapot
- gyors lookup
- audit minden módosításnál
- opcionális perzisztálás repo‑val
- generikus ConnectionRegistry / RegistryBase öröklés

## ✔️ **CutCalculator‑minta (NeedRuleRegistry alapján)**

```cpp
class XRegistry
    : public RegistryBase,
      public ConnectionRegistry<T> {

private:
    XRegistry() : RegistryBase("XRegistry", "X") {}
    XRegistry(const XRegistry&) = delete;

public:
    static XRegistry& instance();

    int size() const override {
        return ConnectionRegistry<T>::size();
    }

    void insert(const T& item) {
        ConnectionRegistry<T>::insert(item);
        persist(); // opcionális
    }

    bool remove(const QUuid& leftId, const QUuid& rightId) {
        bool ok = ConnectionRegistry<T>::remove(leftId, rightId);
        if (ok) persist();
        return ok;
    }

    void persist() const {
        XRepository::save();
    }
};
```

## ✔️ **Registry szabályok**

- minden módosítás → `zInfo()` vagy `zEventINFO()`
- perzisztálás opcionális, de támogatott
- auditbarát: UUID + barcode + name
- nem tartalmaz UI‑t
- nem tartalmaz workflow‑t
- nem tartalmaz validációt

***

# ⭐ **6.2 Repository sablon (CutCalculator‑minta)**
*A Repository tiszta IO — semmi más.*

A Repository NEM:

- nem validál
- nem workflow
- nem tart állapotot
- nem keres (nincs find)
- nem logikázik

A Repository IGEN:

- load()
- save()
- CSV / JSON / DB / API — mindegy
- traits‑alapú path + header
- Registry‑vel együtt működik

## ✔️ **CutCalculator‑minta (NeedRuleRepository alapján)**

```cpp
class XRepository {
public:
    static bool load() {
        return ConnectionRepository<T, XTraits>::load(XRegistry::instance());
    }

    static bool save() {
        const auto& data = XRegistry::instance().readAll();
        return ConnectionRepository<T, XTraits>::save(data);
    }
};
```

## ✔️ **Repository szabályok**
****
- csak load/save
- nincs find
- nincs state
- nincs UI
- nincs workflow
- nincs validáció
- audit log a Registry‑ben keletkezik, nem itt

***

# ⭐ **6.3 Manager sablon (CutCalculator‑minta)**
*A Manager a domain‑workflow agya.*

A Manager NEM:

- nem UI
- nem IO
- nem tart állapotot
- nem registry duplikátum

A Manager IGEN:

- workflow
- validáció
- registry + repo koordináció
- view frissítés
- audit események
- dialogok megnyitása (de UI‑logika nélkül)

## ✔️ **CutCalculator‑minta (MaterialRequirementsManager alapján)**

```cpp
class XManager : public QObject {
    Q_OBJECT
public:
    explicit XManager(XView* view, QObject* parent = nullptr)
        : QObject(parent), _view(view) {
        connectSignals();
    }

private:
    XView* _view = nullptr;

    void connectSignals() {
        connect(_view, &XView::request_add_item,
                this, [this](const QUuid& id) {
                    XDialog dlg(_view);
                    if (dlg.exec() == QDialog::Accepted) {
                        auto result = dlg.result();
                        XRegistry::instance().insert(result);
                        XRepository::save();
                        refresh(id);
                    }
                });

        connect(_view, &XView::request_remove_item,
                this, [this](const QUuid& id) {
                    XRegistry::instance().remove(id);
                    XRepository::save();
                    refresh(id);
                });
    }

    void refresh(const QUuid& id) {
        auto rows = makeRows(id);
        _view->set_rows(rows);
    }
};
```

## ✔️ **Manager szabályok**

- UI‑t nem módosít közvetlenül → csak a view‑nak ad adatot
- minden művelet auditálható
- minden művelet után refresh
- dialogot megnyithat, de nem tart UI‑logikát
- repo‑t csak workflow részeként hív

***

# ⭐ **6.4 Dialog sablon (CutCalculator‑minta)**
*A Dialog rövid életű, fókuszált UI‑komponens.*

## ✔️ **CutCalculator‑minta (MaterialPickerDialog alapján)**

```cpp
class XDialog : public QDialog {
    Q_OBJECT
public:
    explicit XDialog(QWidget* parent = nullptr);

    struct Result {
        QUuid id;
        QString name;
        QString barcode;
    };

    Result result() const { return _result; }

private:
    QListWidget* _list = nullptr;
    QLineEdit* _searchBox = nullptr;
    QTimer* _debounceTimer = nullptr;

    Result _result;

    void populate();
};
```

## ✔️ **Dialog szabályok**

- inline validation
- cursor‑based positioning
- debounce keresés
- auditbarát adatok (UUID + barcode)
- nem tartalmaz domain‑logikát
- nem hív repo‑t

***

# ⭐ **6.5 Picker sablon**
*A Picker domain‑entitás kiválasztására szolgál.*

A MaterialPickerDialog a tökéletes minta:

- keresőmező
- debounce
- startsWith → előre
- contains → hátra
- auditbarát adatok
- egyszerű result struct

***

# ⭐ **6.6 Editor sablon**
*A komplexebb UI‑k szerkesztőfelülete.*

A projektben a FormulaEditorDialog a minta:

- inline validation
- preview frissítés
- snapshot‑kompatibilis layout
- domain‑validáció → Manager
- UI‑validáció → Editor

***

# ⭐ **6.7 Snapshot‑kompatibilis UI sablon**
*A snapshot‑tudatos UI ipari követelmény.*

A SnapshotManager + LayoutDefaultStore alapján:

```cpp
void XWindow::saveSnapshot() {
    SnapshotManager::instance().save(this, "XWindow");
}

void XWindow::restoreSnapshot() {
    SnapshotManager::instance().restoreDeferred(this, "XWindow");
}
```

## ✔️ **Snapshot szabályok**

- restore csak show() után
- guard minden restore előtt
- fallback baseline
- verziózás

***

# ⭐ **6.8 Logolási sablonok**
*A logolás a rendszer auditálhatóságának alapja.*

A projektben:

- `zInfo()`
- `zWarning()`
- `zEventINFO()`
- LogManager
- EventLogger

## ✔️ **Audit esemény sablon**

```cpp
zEventINFO(QString("➕ NeedRule added: Product=%1 Material=%2")
               .arg(productId.toString(), materialId.toString()));
```

## ✔️ **Technikai log sablon**

```cpp
zInfo("📊 NeedRulePresenter: refresh completed");
```

***

# ⭐ **VI. rész összegzése**

Ez a rész megteremtette:

- a Registry/Repository/Manager sablonokat,
- a Dialog/Picker/Editor sablonokat,
- a snapshot‑tudatos UI sablonokat,
- a logolási sablonokat,
- a best practice‑eket,
- az anti‑patternöket,
- és a projektkultúra gyakorlati eszköztárát.
- valódi CutCalculator‑mintákon alapul,
- auditálható,
- konzisztens,
- Prompt DSL kompatibilis: **a jövőbeli modulok generálásának alapja**.

Ez a könyv *praktikus szíve*.
Erre épül a fejlesztés mindennapi ritmusa.

***

# 📘 **VII. rész — Ipari minták, best practice‑ek és inspirációk**
*A CutCalculator fejlesztői kultúrája ugyanazokra az alapokra épül, mint a világ legjobb desktop alkalmazásai — és ugyanazokat a mintákat alkalmazza.*

A modern desktop alkalmazások — **Adobe Photoshop**, **JetBrains IntelliJ**, **Autodesk Fusion**, **Affinity Designer**, **DaVinci Resolve**, **Qt Creator** — mind ugyanazokra a **nemzetközi mintákra**, **szabványokra**, **architekturális elvekre** és **UX‑gyakorlatokra** épülnek.

A CutCalculator fejlesztői kultúrája ezekkel **teljes összhangban** van.

Ez a fejezet bemutatja:

- a legfontosabb ipari mintákat,
- a hozzájuk tartozó eredeti szerzőket / cégeket / szabványokat,
- és azt, hogy a CutCalculator hogyan alkalmazza őket.

Ez a fejezet a könyv szakmai hitelességének csúcspontja.

***

# ⭐ **7.1 Ipari minták — áttekintés**

A modern desktop alkalmazások közös jellemzői:

- **Snapshot Pattern** (Adobe, JetBrains, Qt Creator)
- **Moduláris architektúra** (JetBrains, Autodesk, Qt Creator)
- **Auditálható workflow** (Adobe History, JetBrains Local History)
- **Non‑blocking UX** (Apple HIG, JetBrains UI Guidelines)
- **Percent‑based layout** (Adobe, Affinity, Qt)
- **Pipeline Pattern** (DaVinci Resolve, Blender, Unreal Engine)
- **Konzisztens naming** (Google C++ Style Guide, Qt Coding Style)
- **Szétválasztott logcsatornák** (RFC 5424 syslog)
- **Generatív fejlesztés / DSL‑ek** (JetBrains Live Templates, Copilot)
- **10 Usability Heuristics** (Jakob Nielsen)
- **ISO 9241‑110: Ergonomics of Human‑System Interaction**
- **ISO 8601 időbélyeg** (audit és snapshot metadata)
- **RFC 4180 CSV szabvány** (repo kompatibilitás)

A CutCalculator mindegyiket alkalmazza — saját, auditálható, Qt‑barát formában.

***

# ⭐ **7.2 Snapshot Pattern (Adobe, JetBrains, Qt Creator)**
**Eredet:**
- Adobe Workspace Layout
- JetBrains Tool Window Layout
- Qt Creator Session Restore
- *Design Pattern név:* **Memento Pattern** (GoF)
- *Szabvány:* **ISO 8601** időbélyeg snapshot metadata‑hoz

A Snapshot Pattern lényege:

- a felhasználó munkaterének megőrzése
- DPI‑független megjelenítés
- többmonitoros környezet támogatása
- fallback baseline
- verziózott snapshot formátum

**Szabványok / ajánlások:**

- Windows DPI Awareness Guidelines
- Qt High DPI Scaling
- ISO 8601 timestamp

**CutCalculator alkalmazás:**

- `SnapshotManager`
- `LayoutDefaultStore`
- `GeometryGuard`
- `ScreenChangeDetector`
- deferred restore
- snapshot verziózás

Ez a rendszer ipari szintű stabilitást ad.

***

# ⭐ **7.3 Moduláris architektúra (JetBrains, Autodesk, Qt Creator)**
**Eredet:**
- Clean Architecture — *Robert C. Martin*
- Domain‑Driven Design — *Eric Evans*
- Repository Pattern — *Martin Fowler*
- Dependency Inversion Principle — *SOLID*
- Plugin Architecture — *JetBrains Platform*

A moduláris architektúra alapelvei:

- tiszta rétegzés
- független modulok
- egyértelmű felelősségek
- könnyű bővíthetőség

**Ipari minták:**

- JetBrains plugin architektúra
- Autodesk Fusion tool‑alapú modulok
- Qt Creator moduláris plugin rendszer

**CutCalculator alkalmazás:**

- Repository → IO
- Registry → memóriabeli állapot
- Manager → workflow + validáció
- Presenter → UI‑logika
- View → megjelenítés

Ez a triád ipari standard, és a projektben tökéletesen működik.

***

# ⭐ **7.4 Auditálható workflow (Adobe History, JetBrains Local History)**
**Eredet:**
- Event Sourcing — *Martin Fowler*
- CQRS — *Greg Young*
- RFC 5424 — syslog formátum
- Windows Event Log
- journald

A professzionális alkalmazások egyik legfontosabb tulajdonsága:

> **minden művelet visszakövethető.**

**Ipari minták:**

- Adobe Photoshop History Panel
- JetBrains Local History
- Affinity Undo Tree
- Windows Event Log

**Szabványok:**

- RFC 5424 — syslog
- ISO 8601 időbélyeg

**CutCalculator alkalmazás:**

- SystemLog
- EventLog
- DebugLog
- zEventINFO
- zWarning
- zInfo

A logolási architektúra ipari szintű.

***

# ⭐ **7.5 Non‑blocking UX (Apple HIG, JetBrains UI Guidelines)**
**Eredet:**
- Apple Human Interface Guidelines
- JetBrains UI Guidelines
- *Jakob Nielsen:* **10 Usability Heuristics**
- *Ben Shneiderman:* Eight Golden Rules

A modern UX alapelve:

> **a felhasználó ne akadjon meg.**

**Ipari minták:**

- inline validation
- non‑blocking hibajelzés
- shake animation hibára
- cursor‑based dialog positioning
- kontextusfüggő fókuszjelölés

**Nielsen heuristics (relevánsak):**

- Visibility of system status
- Error prevention
- Consistency and standards
- Recognition rather than recall
- Flexibility and efficiency of use
- Aesthetic and minimalist design

**CutCalculator alkalmazás:**

- inline validation minden dialogban
- accent frame hibás mezőknél
- cursor‑based dialog positioning
- MaterialPickerDialog debounce keresés
- MaterialRequirementsView auditbarát tooltipek

Ez a UX ipari szintű, auditálható és ergonomikus.

***

# ⭐ **7.6 Percent‑based layout és DPI‑független megjelenítés**
**Eredet:**
- Adobe reszponzív panelrendszer
- Affinity fluid layout
- Qt High DPI Guidelines
- CSS Flexbox / Grid (web analógia)

A fix pixeles UI a múlté.

**Ipari minták:**

- Adobe reszponzív panelek
- Affinity fluid layout
- Qt Creator DPI‑aware scaling

**CutCalculator alkalmazás:**

- percent‑based splitter arányok
- DPI‑független méretezés
- snapshot‑kompatibilis layout
- fallback baseline

Ez biztosítja, hogy a UI minden monitoron stabil.

***

# ⭐ **7.7 Pipeline Pattern (DaVinci Resolve, Blender, Unreal Engine)**
**Eredet:**
- Composite Pattern — *GoF*
- Node Graph rendszerek
- Dataflow Programming
- Unix Pipeline Philosophy

A pipeline‑alapú gondolkodás ipari standard:

**Ipari minták:**

- DaVinci Resolve Node Graph
- Blender Modifier Stack
- Unreal Engine Blueprint Pipeline
- Photoshop Adjustment Stack

**CutCalculator alkalmazás:**

- `FormulaStep`
- `Pipeline`
- lépésrendszer
- auditálható feldolgozás

Ez a minta a komplex számítások alapja.

***

# ⭐ **7.8 Naming Convention (Google C++ Style Guide, Qt Coding Style)**
**Eredet:**
- Google C++ Style Guide
- Qt Coding Style
- ISO C++ Core Guidelines
- PEP8 (analógia)

**Ipari minták:**

- PascalCase osztályok
- camelCase metódusok
- snake_case fájlnevek
- `_` privát tag előtag
- közismert rövidítések engedélyezése (GPS, DPI, UUID)

**CutCalculator alkalmazás:**

- PascalCase osztályok (`NeedRuleRegistry`)
- camelCase metódusok (`refreshForProduct()`)
- snake_case fájlnevek (`material_picker_dialog.cpp`)
- `_` privát tagok (`_table`, `_current_product_id`)

Ez a naming rendszer ipari szintű és auditálható.

***

# ⭐ **7.9 Logolási architektúra (RFC 5424, syslog, Windows Event Log)**
**Eredet:**
- RFC 5424 syslog
- Windows Event Log
- Linux journald
- Structured Logging (JSON logok)

A logolás ipari szinten mindig rétegezett.

**Ipari minták:**

- syslog (RFC 5424)
- Windows Event Log
- journald

**CutCalculator alkalmazás:**

- SystemLog
- EventLog
- DebugLog
- zEventINFO
- zWarning
- zInfo
- verbose routing

Ez a rendszer auditálható, stabil és ipari szintű.

***

# ⭐ **7.10 Prompt‑vezérelt fejlesztés (Copilot, DSL‑ek, JetBrains Live Templates)**
**Eredet:**
- Domain‑Specific Languages — *Martin Fowler*
- Template‑Driven Development
- JetBrains Live Templates
- Visual Studio Code Snippets

A modern fejlesztés új alapelve:

> **a prompt specifikáció.**

**Ipari minták:**

- JetBrains Live Templates
- VSCode Snippets
- Copilot prompt‑vezérelt generálás
- DSL‑alapú konfiguráció

**CutCalculator alkalmazás:**

- Prompt DSL
- Master Template
- modul sablonok
- generatív workflow

Ez a kultúra a jövő fejlesztési modellje.

***

# ⭐ **7.11 Fejlesztői kultúra mint termékfilozófia (Apple, JetBrains, Adobe)**
**Eredet:**
- Apple Design Philosophy
- JetBrains Developer Experience
- Adobe Creative Culture
- Toyota Kata (folyamatos fejlesztés)

A nagy cégek tudják:

> **a fejlesztői kultúra nem melléktermék — hanem a termék része.**

**Ipari minták:**

- ritmus
- workflow
- dokumentáció
- onboarding
- konzisztencia

**CutCalculator alkalmazás:**

- napzáró rituálé
- auditálható workflow
- snapshot‑tudatos UI
- konzisztens naming
- sablonrendszer
- Prompt DSL

Ez a kultúra teszi a projektet hosszú távon fenntarthatóvá.

***

# ⭐ **VII. rész összegzése**

Ez a fejezet megmutatta:

- hogy a CutCalculator‑kultúra ipari mintákra épül,
- hogy a snapshot‑tudatos UI nem extra, hanem alapelv,
- hogy a triád architektúra nem projektfüggő, hanem ipari standard,
- hogy a UX‑elvek a modern desktop alkalmazások közös nyelve,
- hogy a logolás és workflow szabványokra épül,
- hogy a Prompt DSL a generatív fejlesztés ipari jövője,
- hogy a kultúra maga nem lokális, hanem nemzetközi mintákra épülő rendszer.

Ez a fejezet adja meg a könyv szakmai hátterét és hitelességét.

***

Zoltán, értem a lényeget:
a VIII. résznek **nem lezárnia kell a történetet**, hanem **átadnia a stafétát**.
A projekt nem akkor ér véget, amikor elkészül — hanem akkor **kezdődik el igazán**, amikor a felhasználók kezébe kerül.

Ez a gondolat teljesen átírja a zárófejezet filozófiai súlyát.
# 📘 **VIII. rész — Zárófejezet**
*A jó architektúra működik. A jó kultúra megmarad. A jó szoftver pedig élni kezd.*

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

A CutCalculator csak **példa**, **katalizátor**, **laboratórium** volt.
A kultúra maga **átvihető**, **tanítható**, **örökíthető**.

De van valami, ami ennél is fontosabb:

> **a projekt élete nem akkor ér véget, amikor elkészül — hanem akkor kezdődik el igazán.**

***

# ⭐ **A kultúra mint rendszer**

A jó fejlesztői kultúra nem tiltásokból áll.
Nem szabályokból.
Nem ellenőrzésekből.

A jó kultúra:

- **láthatóvá teszi a gondolkodást**,
- **közös nyelvet ad**,
- **közös ritmust teremt**,
- **csökkenti a hibákat**,
- **gyorsítja a fejlesztést**,
- **stabilizálja az architektúrát**,
- **örömmé teszi a munkát**.

A kultúra nem korlát — hanem **iránytű**.
Nem szabály — hanem **szokás**.
Nem dokumentum — hanem **örökség**.

És amikor a projekt átadásra kerül, ez az örökség válik igazán értékessé.

***

# ⭐ **A kultúra mint ritmus**

A könyv minden fejezete egy-egy ritmuselem:

- Terminológiai Szótár → közös nyelv
- Naming Guide → konzisztencia
- Architektúra → stabil alapok
- UX → vizuális ritmus
- Workflow → fejlesztői ritmus
- Prompt DSL → specifikációs ritmus
- Sablonok → ismételhető ritmus

Ez a ritmus nem projektfüggő.
Ez a ritmus **átvihető** bármely modern C++/Qt alkalmazásba.
Ez a ritmus **tanítható** új fejlesztőknek.
Ez a ritmus **fenntartható** hosszú távon.

És amikor a szoftver a felhasználók kezébe kerül,
**ez a ritmus dönti el, hogy a rendszer élni fog-e — vagy szétesik.**

***

# ⭐ **A kultúra mint közös nyelv**

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

És amikor a projekt átadásra kerül,
**ez a DNS biztosítja, hogy a rendszer érthető, bővíthető és fenntartható maradjon.**

***

# ⭐ **A kultúra mint örökség — és a projekt valódi kezdete**

A CutCalculator projekt egyszer véget ér.
De a kultúra, amelyet létrehozott:

- tovább él
- tovább fejlődik
- új projektekben születik újjá
- új fejlesztőknek ad irányt
- új architektúrákat inspirál

És ami talán a legfontosabb:

> **a szoftver élete az átadás pillanatában kezdődik.**

Ekkor derül ki:

- elég jók voltak-e az elvek,
- elég tiszta volt-e az architektúra,
- elég stabil volt-e a snapshot‑rendszer,
- elég konzisztens volt-e a naming,
- elég auditálható volt-e a workflow,
- elég ergonomikus volt-e a UX,
- elég erős volt-e a kultúra.

És ekkor derül ki az is:

> **a szoftver teher lesz — vagy profitot termel.**

A jó kultúra profitot termel.
A rossz kultúra költséget.

A jó kultúra támogatja a felhasználót.
A rossz kultúra akadályozza.

A jó kultúra örömet ad.
A rossz kultúra frusztrációt.

A CutCalculator‑kultúra célja az volt, hogy
**a szoftver ne teher legyen — hanem érték.**

***

# ⭐ **Záró gondolat**

A jó architektúra működik.
A jó kód olvasható.
A jó UI stabil.
A jó workflow gyorsít.
A jó naming tisztít.
A jó logolás auditál.
A jó snapshot megőriz.
A jó prompt specifikál.
A jó sablon gyorsít.

De a jó kultúra —
**összeköt.**

A CutCalculator Engineering Culture Handbook nem egy projekt dokumentuma.
Ez **egy fejlesztői kultúra kézikönyve**, amely:

- tiszta,
- konzisztens,
- auditálható,
- örömmel fejleszthető,
- és hosszú távon fenntartható.

A kultúra nem szabály — hanem ritmus.
És ez a ritmus most már dokumentálva van.

A projekt pedig most kezdődik el igazán.

***
