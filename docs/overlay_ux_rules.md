# Overlay UX Rules

## Base Emoji
- Always use 📄 as the base emoji for all repository-related overlays.

## States
- EmptyRepo → ❌
- NoVisibleRows → 🟡
- Normal → 🟢

## Presenter Responsibilities
- Call RepositoryOverlayWidget<T>::refresh(visibleRows)
- Ensure refresh is triggered:
  - after view data changes
  - after registry changes
  - after product or mode selection changes

## Helper Responsibilities
- OverlayStatusHelper::computeState(repoCount, visibleRows)
- OverlayStatusHelper::apply(widget, state)

## Widget Responsibilities
- Render base emoji
- Render overlay emoji in BottomRight corner
- No business logic
