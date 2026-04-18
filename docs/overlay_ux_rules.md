Overlay UX Rules — Matrix v3.5 (PATCH 7–8)

1. Alapelv

Az overlay alapértelmezésben NEM jelzi a mátrix állapotát.
Az overlay továbbra is a repository + view állapotát mutatja:

-   🟥 EmptyRepo — üres repository
-   🟨 NoVisibleRows — van adat, de nincs látható sor
-   🟩 Normal — van adat és látható sorok

Újdonság (v3.5):
A Presenter kiegészítheti ezt egy negyedik állapottal:

-   🔵 Incomplete — a mátrix hiányos (MatrixValidator alapján)

Ez az állapot nem automatikus, a Presenter állítja be.

------------------------------------------------------------------------

2. Overlay állapotok (OverlayStatusHelper::State)

  -----------------------------------------------------------------------------------------------------------------------------------
  Állapot         Emoji         Jelentés           Ki dönti el?                         Audit esemény
  -----------------------------------------------------------------------------------------------------------------------------------
  EmptyRepo       🟥            nincs adat a       OverlayStatusHelper
                                repositoryban      

  NoVisibleRows   🟨            van adat, de a     OverlayStatusHelper
                                view nem mutat     
                                semmit             

  Normal          🟩            van adat és        OverlayStatusHelper
                                látható sorok      

  Incomplete      🔵            a mátrix hiányos   Presenter (MatrixValidator alapján)  Incomplete → „zEventWARN: matrix incomplete
                                                   
  -----------------------------------------------------------------------------------------------------------------------------------

------------------------------------------------------------------------

3. Sor‑szintű ikonok (nem overlay)

DetailView ikonok

-   ❌ Material missing
-   ❗ Invalid formula
-   🟡 Unknown formula
-   🟢 OK

ModesView ikonok

-   🔴 Missing detail(s)
-   🟡 Unknown formula(s)
-   🟢 All details OK

MaterialRequirementsView ikonok

-   ❗ Material missing
-   ⚠️ Missing detail(s) for this material
-   🟢 OK

Ezek NEM overlay ikonok, hanem sor‑szintű állapotjelzések.

------------------------------------------------------------------------

4. MatrixValidator integráció v3.1

A Presenter (pl. MaterialRequirementsPresenter) felelős a mátrix
állapotának lekérdezéséért.

4.1. A Presenter nem csak lekérdezi, hanem auditálja is az eredményt.
    bool complete = MatrixValidator::isProductMatrixComplete(productId);

4.2. A Presenter dönti el az overlay állapotát
    OverlayStatusHelper::State state;
    
  if (!complete)
        state = OverlayStatusHelper::State::Incomplete;
  else
        state = OverlayStatusHelper::computeState(repoCount, visibleRows);

4.3. A Presenter alkalmazza az overlayt
    OverlayStatusHelper::apply(_overlayWidget, state);


4.4 A Presenter felelős a navigációért (első hibás sorra ugrás).

------------------------------------------------------------------------

5) Overlay alkalmazási folyamat

Új felelősségi lánc:
Kód

Presenter
   ↓  (döntés: complete / incomplete)
OverlayStatusHelper
   ↓  (widget state mapping)
OverlayWidget

6) Sor‑szintű ikonok szakasz

Kiegészítés:
    Ezek nem overlay ikonok, és nem befolyásolják az overlay állapotát.
    A sor‑szintű ikonok a View saját logikáját követik.

7. Navigációs szabályok

A Presenter automatikusan a legelső hibás sorra ugrik:

-   material missing
-   invalid formula
-   unknown formula
-   missing detail

Ez változatlan a v2‑höz képest.

- A Presenter a MatrixValidator hibalistája alapján ugrik a hibás sorra.
- Ez a viselkedés kötelező, nem opcionális.

------------------------------------------------------------------------

8) Audit követelmények

Új szakasz vagy kiegészítés:

  Incomplete állapot beállításakor:
    zEventWARN("Matrix incomplete for productId=...")
  Normal/EmptyRepo/NoVisibleRows esetén:
        zEventINFO("Overlay state = ...")

9) BOMWorkbench overlay flow

A dokumentum jelenleg nem tartalmazza, de szükséges:
  - BOMWorkbench Presenter → overlay frissítés
  - overlay frissítés időzítése (restore után)
  - deferred overlay update, ha a view még nem ready      
  - Overlay csak akkor frissíthető, ha a Workbench már restore‑ready
    
Ha a view még nem stabil, akkor:
QtEventUtil::post(... applyOverlay ...)

6. Összefoglaló

-   Az overlay továbbra is repository‑policy, de kiegészült egy
    Presenter‑vezérelt „Incomplete” állapottal.
-   A mátrix állapotát nem az OverlayStatusHelper számolja ki.
-   A mátrix állapotát a Presenter dönti el, a MatrixValidator eredménye
    alapján.
-   A sor‑szintű ikonok továbbra is a View logikáját követik.
-   A rendszer így teljesen konzisztens a Matrix v2 logikával és a
    modern workflow‑architektúrával.

------------------------------------------------------------------------

