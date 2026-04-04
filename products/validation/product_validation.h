/* ============================================================
 * 🧩 ProductValidation – domain-level validation API
 * ============================================================ */
#pragma once

#include <QString>
#include <QUuid>

namespace ProductValidation {

// Név validáció (üres, duplikált, parentId alatt)
bool validateName(const QString& name,
                  const QUuid& parent_id,
                  QString& out_error);

// Barcode validáció (UI dry-run, ledger módosítás nélkül)
bool validateBarcode_UI_DryRun(const QString& code,
                               const QString& entity_type,
                               const QUuid& id,
                               const QString& name,
                               QString& out_error);

// Barcode végleges UI-regisztráció (validate + ledger insert)
bool registerBarcode_UI(const QString& code,
                        const QString& entity_type,
                        const QUuid& id,
                        const QString& name,
                        QString& out_error);

} // namespace ProductValidation
