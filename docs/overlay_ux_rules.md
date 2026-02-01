# Overlay UX Rules — Matrix v2

## 1. Alapelv
Az overlay NEM jelzi a mátrix állapotát.  
Az overlay kizárólag a repository állapotát mutatja:

- 🟥 Üres repository
- 🟨 Van adat, de nincs látható sor
- 🟩 Van adat és látható sorok

A mátrix állapotát a View réteg ikonlogikája jelzi.

## 2. DetailView ikonok
- ❌ Material missing  
- ❗ Invalid formula  
- 🟡 Unknown formula  
- 🟢 OK  

## 3. ModesView ikonok
- 🔴 Missing detail(s)  
- 🟡 Unknown formula(s)  
- 🟢 All details OK  

## 4. MaterialRequirementsView ikonok
- ❗ Material missing  
- ⚠️ Missing detail(s) for this material  
- 🟢 OK  

## 5. Navigáció
A Presenter automatikusan a legelső hibás sorra ugrik:
- material missing  
- invalid formula  
- unknown formula  
